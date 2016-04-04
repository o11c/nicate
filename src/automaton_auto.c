#include "automaton.h"
#include "automaton-internal.h"
/*
    Copyright © 2015-2016 Ben Longbons

    This file is part of Nicate.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bitset.h"
#include "hashmap.h"
#include "pool.h"
#include "util.h"

/*
    This is a full Canonical LR(1) parser without merging.

    It seems to be tolerable on modern systems.
*/

typedef size_t RuleId;
typedef size_t SymbolId;
typedef SymbolId TerminalId;
typedef SymbolId NonterminalId;
typedef size_t StateId;

typedef BitSet *SymbolList;

typedef struct ActionList ActionList;
/*
    This always has size 0 or 1, unless there is a conflict.
*/
struct ActionList
{
    Action *actions;
    size_t actions_size, actions_cap;
};

typedef struct Item Item;
struct Item
{
    size_t rule, index;
    SymbolList lookahead;
};

typedef struct ItemSet ItemSet;
struct ItemSet
{
    /* implicit: size_t state_id - index within growing automaton */
    Item *items;
    size_t items_size;
    /* Indexed by any symbol, though the last half is gotos */
    ActionList *actions;
    ActionList def;
};

typedef struct Lr1Junk Lr1Junk;
struct Lr1Junk
{
    Grammar *grammar;

    /* Key is (interned) ItemList, value is StateId */
    Pool *kernels;

    ItemSet *states;
    size_t states_size, states_cap;

    /* Indexed by nonterminals */
    SymbolList *first;
    SymbolList *includes;
};


static void calc_first(Grammar *grammar, SymbolId sym, BitSet *terminals, BitSet *nonterminals)
{
    if (sym < grammar->num_symbols)
    {
        bitset_set(terminals, sym);
    }
    else
    {
        size_t shifted_sym = sym - grammar->num_symbols;
        if (!bitset_test(nonterminals, shifted_sym))
        {
            size_t rule = grammar->rules_by_nonterminal[shifted_sym];
            bitset_set(nonterminals, shifted_sym);
            for (; rule != grammar->num_rules && grammar->rules[rule].lhs == sym; ++rule)
            {
                size_t rhs0 = grammar->rules[rule].rhses[0];
                calc_first(grammar, rhs0, terminals, nonterminals);
            }
        }
    }
}

static void calc_includes_for(Grammar *grammar, const size_t sym, SymbolList *cache, HashMap *wip)
{
    size_t num_terminals = grammar->num_symbols;
    size_t num_nonterminals = grammar->num_nonterminals;
    HashKey key = {(unsigned char *)&sym, sizeof(sym)};
    if (map_entry(wip, key, SEARCH_ONLY))
        abort();
    /*
        This function is called in a loop to ensure that it is done for
        every symbol, but it also needs to be called recursively, so it
        is possible that it has already been calculated for the symbol.

        The cache either is NULL or a complete value. Incomplete values
        would only be possible if this function were still in the middle
        of being called, but that would be a cycle and it will abort
        by checking the wip stack.
    */
    if (cache[sym - num_terminals])
        return;
    map_entry(wip, key, INSERT_ONLY);
    cache[sym - num_terminals] = bitset_create(num_nonterminals);
    {
        size_t rule = grammar->rules_by_nonterminal[sym - num_terminals];
        for (; rule != grammar->num_rules && grammar->rules[rule].lhs == sym; ++rule)
        {
            size_t rhs0 = grammar->rules[rule].rhses[0];
            if (grammar->rules[rule].num_rhses != 1 || rhs0 < num_terminals)
                continue;
            /* Recurse for single nonterminals. */
            calc_includes_for(grammar, rhs0, cache, wip);
            bitset_or_eq(cache[sym - num_terminals], cache[rhs0 - num_terminals]);
            bitset_set(cache[sym - num_terminals], rhs0 - num_terminals);
        }
    }
    map_pop(wip);
}

static void do_state(size_t state, Lr1Junk *junk);
static size_t new_state(const Item *seeds, size_t num_states, Lr1Junk *junk);
static size_t next_state(size_t state, size_t sym, Lr1Junk *junk);

static void add_action(ActionList *list, Action act)
{
    size_t old_cap = list->actions_cap;
    if (list->actions_size == old_cap)
    {
        size_t new_cap = old_cap * 2 + !old_cap;
        list->actions = (Action *)realloc(list->actions, new_cap * sizeof(Action));
        list->actions_cap = new_cap;
        if (!list->actions)
        {
            abort();
        }
        memset(list->actions + old_cap, '\0', (new_cap - old_cap) * sizeof(Action));
    }
    list->actions[list->actions_size++] = act;
}
/*
    Fill the action tables for a freshly-allocated and closed state.

    This involves recursively creating and filling the successors if
    they don't exist yet.
*/
void do_state(size_t state, Lr1Junk *junk)
{
    size_t num_terminals = junk->grammar->num_symbols;
    size_t items_size = junk->states[state].items_size;
    size_t i;
    BitSet *seen = bitset_create(junk->grammar->num_symbols + junk->grammar->num_nonterminals);
    for (i = 0; i < items_size; ++i)
    {
        Item *it = &junk->states[state].items[i];
        Rule *rule = &junk->grammar->rules[it->rule];
        if (it->index == rule->num_rhses)
        {
            size_t j;
            Action act;
            act.type = REDUCE;
            act.value = it->rule;
            for (j = 0; j < num_terminals; ++j)
            {
                if (bitset_test(it->lookahead, j))
                {
                    add_action(&junk->states[state].actions[j], act);
                }
            }
        }
        else
        {
            size_t sym = rule->rhses[it->index];
            size_t succ_state;
            Action act;
            if (bitset_test(seen, sym))
                continue;
            bitset_set(seen, sym);
            succ_state = next_state(state, sym, junk);
            act.type = SHIFT; /* == GOTO */
            act.value = succ_state;
            add_action(&junk->states[state].actions[sym], act);
        }
    }
    bitset_destroy(seen);
}

/*
    Allocate a new state and return its index in the `junk->states` array.

    The initial items are only the kernel; additional items will be
    generated by closure.

    It is *not* this function's responsibility to intern the kernel.
*/
size_t new_state(const Item *seeds, size_t num_seeds, Lr1Junk *junk)
{
    Grammar *grammar = junk->grammar;
    SymbolList *first = junk->first;
    SymbolList *includes = junk->includes;
    size_t num_terminals = grammar->num_symbols;
    size_t num_nonterminals = grammar->num_nonterminals;
    /* Allocate a new state. */
    size_t rv = junk->states_size;
    ItemSet *item_set;
    if (junk->states_size == junk->states_cap)
    {
        size_t old_cap = junk->states_cap;
        size_t new_cap = old_cap ? old_cap * 2 : 1;
        ItemSet *new_states = (ItemSet *)realloc(junk->states, new_cap * sizeof(ItemSet));
        if (!new_states) abort();
        memset(new_states + old_cap, '\0', (new_cap - old_cap) * sizeof(ItemSet));
        junk->states = new_states;
        junk->states_cap = new_cap;
    }
    junk->states_size++;
    /*
        Fill in the new state; duplicate what we directly borrowed.
        We do *not* duplicate the bitsets, the lender must have.
    */
    item_set = &junk->states[rv];
    item_set->items = (Item *)memdup(seeds, num_seeds * sizeof(Item));
    item_set->items_size = num_seeds;
    item_set->actions = (ActionList *)calloc(num_terminals + num_nonterminals, sizeof(ActionList));
    (void)item_set->def;
    /* Perform closure. */
    {
        /*
            When we have rules of the form:

            A → B • C D ∥ α;
            E → F • C ∥ β;

            If C is not a terminal, we must add rules of the form:

            C → • H I ∥ (first(D) ∪ β);
            C → • J K ∥ (first(D) ∪ β);

            Note that with the exception of the top-level rule, the
            position can *only* be at the front outside of the kernel.

            These must be grown together, but even though β terms may exist,
            it should not be shared with them, as that would only be
            possible with a reduce-reduce conflict (not for C, but A/E).

            At least, I *think* that's how this works.
        */
        size_t i;
        /*
            Contents are borrowed pointers or NULL.
            The shared owners are all the rules with that LHS.
        */
        BitSet **lookaheads = (BitSet **)calloc(num_nonterminals, sizeof(BitSet *));
        for (i = 0; i < item_set->items_size; ++i)
        {
            /*
                Important note!

                We modify the container while iterating over it, and we
                *do* want to hit the items just added it.
            */
            Item *it = &item_set->items[i];
            /*
                Allocate the needed lookups, and incref ahead of time.
            */
            Rule *rule = &grammar->rules[it->rule];
            size_t sym1;
            if (it->index == rule->num_rhses)
                continue;
            sym1 = rule->rhses[it->index];
            if (sym1 < num_terminals)
                continue;
            if (!lookaheads[sym1 - num_terminals])
            {
                lookaheads[sym1 - num_terminals] = bitset_create(num_terminals);
                size_t rule_idx = grammar->rules_by_nonterminal[sym1 - num_terminals];
                for (; rule_idx < grammar->num_rules && grammar->rules[rule_idx].lhs == sym1; ++rule_idx)
                {
                    size_t item_number = item_set->items_size;
                    ++item_set->items_size;
                    /* TODO capacity? For now, hope realloc is geometric. */
                    item_set->items = (Item *)realloc(item_set->items, item_set->items_size * sizeof(Item));
                    if (!item_set->items) abort();
                    item_set->items[item_number].rule = rule_idx;
                    item_set->items[item_number].index = 0;
                    /* Set in third loop. */
                    item_set->items[item_number].lookahead = NULL;
                    if (rule_idx != grammar->rules_by_nonterminal[sym1 - num_terminals])
                    {
                        (void)bitset_incref(lookaheads[sym1 - num_terminals]);
                    }
                }
            }
        }
        /* No more allocation from here on. */

        for (i = 0; i < item_set->items_size; ++i)
        {
            Item *it = &item_set->items[i];
            /*
                Look at next-next-syms, which are the lookahead for
                shifting a given next-sym.

                This may be NULL for the case where the lookahead will
                be assigned below, which is why we need the separate loops.
            */
            Rule *rule = &grammar->rules[it->rule];
            size_t sym1, index, j;
            /* Borrowed. */
            BitSet *nn_multi = NULL;
            size_t nn_single = (size_t)-1;
            if (it->index == rule->num_rhses)
                continue;
            sym1 = rule->rhses[it->index];
            if (sym1 < num_terminals)
                continue;

            index = it->index + 1;
            if (index == rule->num_rhses)
            {
                nn_multi = it->lookahead;
                if (nn_multi == NULL)
                    continue;
            }
            else
            {
                size_t sym2 = rule->rhses[index];
                if (sym2 < num_terminals)
                {
                    nn_single = sym2;
                }
                else
                {
                    nn_multi = first[sym2 - num_terminals];
                    assert (nn_multi != NULL);
                }
            }
            for (j = 0; j < num_nonterminals; ++j)
            {
                if (j + num_terminals == sym1 || bitset_test(includes[sym1 - num_terminals], j))
                {
                    if (j + num_terminals != sym1)
                        assert (lookaheads[j] != NULL);
                    if (nn_multi)
                        bitset_or_eq(lookaheads[j], nn_multi);
                    else
                        bitset_set(lookaheads[j], nn_single);
                }
            }
        }
        for (i = num_seeds; i < item_set->items_size; ++i)
        {
            Item *it = &item_set->items[i];
            /*
                Assign the lookaheads for non-kernel items.
            */
            size_t sym1 = grammar->rules[it->rule].lhs;
            it->lookahead = lookaheads[sym1 - num_terminals];
        }
    }
    return rv;
}

static void *bitset_self(Pool *pool, const void *data, size_t len, void *context)
{
    (void)pool;
    (void)data;
    (void)len;
    return (BitSet *)context;
}

static BitSet *pool_intern_bitset(Pool *pool, BitSet *b)
{
    HashKey key = bitset_as_key(b);
    return (BitSet *)pool_intern_map(pool, bitset_self, key.data, key.len, b);
}

static size_t next_state_kernel_transform_really(const Item *items, size_t items_size, Lr1Junk *junk);

static void *next_state_kernel_transform(Pool *pool, const void *str, size_t len, void *context)
{
    (void)pool;
    return (void *)next_state_kernel_transform_really((const Item *)str, len / sizeof(Item), (Lr1Junk *)context);
}

/*
    Calculate a state's successor under the given symbol.

    This calculates the kernel, then checks whether there is an existing
    state that uses that kernel and returns it if it exists.

    Otherwise, it allocates a new one and then fills in its actions.
*/
size_t next_state(size_t state, size_t sym, Lr1Junk *junk)
{
    Pool *kernels = junk->kernels;
    ItemSet *old_item_set = &junk->states[state];
    /* Will only be partially filled. */
    Item *seeds = (Item *)calloc(old_item_set->items_size, sizeof(Item));
    size_t num_seeds = 0;
    size_t i;
    size_t rv;
    size_t rv_if_new = junk->states_size;
    for (i = 0; i < old_item_set->items_size; ++i)
    {
        Item *it = &old_item_set->items[i];
        Rule *rule = &junk->grammar->rules[it->rule];
        if (it->index == rule->num_rhses)
            continue;
        if (rule->rhses[it->index] != sym)
            continue;
        seeds[num_seeds].rule = it->rule;
        seeds[num_seeds].index = it->index + 1;
        /* This is not owned until we verify the transform is new. */
        seeds[num_seeds].lookahead = pool_intern_bitset(kernels, it->lookahead);
        num_seeds++;
    }
    assert (num_seeds != 0);
    rv = (size_t)pool_intern_map(kernels, next_state_kernel_transform, seeds, num_seeds * sizeof(*seeds), junk);
    if (rv == rv_if_new)
    {
        /*
            Important: this call *must* happen after the kernel interning
            is complete (but only if the intern is fresh).
        */
        do_state(rv, junk);
    }
    return rv;
}

size_t next_state_kernel_transform_really(const Item *items, size_t items_size, Lr1Junk *junk)
{
    size_t i;
    size_t state;
    for (i = 0; i < items_size; ++i)
    {
        (void)bitset_incref(items[i].lookahead);
    }
    state = new_state(items, items_size, junk);
    return state;
}

static void automaton_begin_lr1(Lr1Junk *junk)
{
    Grammar *grammar = junk->grammar;
    size_t num_terminals = grammar->num_symbols;
    size_t num_nonterminals = grammar->num_nonterminals;
    size_t i;
    /* first */
    junk->first = (SymbolList *)malloc(num_nonterminals * sizeof(SymbolList));
    for (i = 0; i < num_nonterminals; ++i)
    {
        BitSet *terminals = bitset_create(num_terminals);
        BitSet *nonterminals = bitset_create(num_nonterminals);
        calc_first(grammar, i + num_terminals, terminals, nonterminals);
        junk->first[i] = terminals;
        bitset_destroy(nonterminals);
    }
    /* includes */
    junk->includes = (SymbolList *)calloc(num_nonterminals, sizeof(SymbolList));
    {
        /*
            key: sym, value: none
            This is used as a stack, but with fast membership check.
        */
        HashMap *wip = map_create();
        for (i = 0; i < num_nonterminals; ++i)
        {
            calc_includes_for(grammar, i + num_terminals, junk->includes, wip);
        }
        map_destroy(wip);
    }
    /* states */
    {
        /*
            Initial state kernel:
            $accept → • start $end ∥ ∅;

            Does not need to be cached since it cannot be a successor.
        */
        Item seeds[1];
        seeds[0].rule = 0;
        seeds[0].index = 0;
        seeds[0].lookahead = bitset_create(num_terminals);
        do_state(new_state(seeds, 1, junk), junk);
    }
}

static Action only(ActionList a, Action def)
{
    if (!a.actions_size)
    {
        return def;
    }
    if (a.actions_size == 1)
    {
        return a.actions[0];
    }
    fprintf(stderr, "Conflict!\n");
    exit(1);
}

static Automaton *automaton_finish(Lr1Junk *junk)
{
    /* TODO Also perform merging at this point. */
    Grammar *g = junk->grammar;
    size_t num_states = junk->states_size;
    State **states = (State **)malloc(num_states * sizeof(State *));
    Action *actions = (Action *)malloc((g->num_symbols + g->num_nonterminals) * sizeof(Action));
    Automaton *rv;
    size_t i, j;
    for (i = 0; i < num_states; ++i)
    {
        ItemSet *state = &junk->states[i];
        static const Action error = {ERROR, 0};
        Action def = only(state->def, error);
        for (j = 0; j < g->num_symbols + g->num_nonterminals; ++j)
        {
            actions[j] = only(state->actions[j], def);
        }
        states[i] = state_create(g, def, actions, actions + g->num_symbols);
    }
    rv = automaton_create(g, num_states, states);
    free(actions);
    free(states);
    return rv;
}

static void free_item_set(ItemSet s, size_t num_symbols)
{
    size_t i;
    free(s.def.actions);
    for (i = num_symbols; i--; )
    {
        free(s.actions[i].actions);
    }
    free(s.actions);
    for (i = s.items_size; i--; )
    {
        bitset_destroy(s.items[i].lookahead);
    }
    free(s.items);
}

static void free_junk(Lr1Junk junk)
{
    size_t i;
    for (i = junk.grammar->num_nonterminals; i--; )
    {
        bitset_destroy(junk.includes[i]);
    }
    free(junk.includes);
    for (i = junk.grammar->num_nonterminals; i--; )
    {
        bitset_destroy(junk.first[i]);
    }
    free(junk.first);
    /* Grammar is *not* freed, it owns itself. */
    for (i = junk.states_size; i--; )
    {
        free_item_set(junk.states[i], junk.grammar->num_symbols + junk.grammar->num_nonterminals);
    }
    free(junk.states);
    pool_destroy(junk.kernels);
}

Automaton *automaton_create_auto(Grammar *g)
{
    Automaton *rv;
    Lr1Junk junk;
    memset(&junk, '\0', sizeof(junk));
    junk.grammar = g;
    junk.kernels = pool_create();
    automaton_begin_lr1(&junk);
    rv = automaton_finish(&junk);
    free_junk(junk);
    return rv;
}
