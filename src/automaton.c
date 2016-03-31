#include "automaton.h"
#include "automaton-internal.h"
/*
    Copyright © 2015 Ben Longbons

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

#include <sys/types.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "bitset.h"
#include "util.h"


struct State
{
    ssize_t def;
    /*
        Can be DEFAULT, ERROR, ACCEPT, SHIFT(state), or REDUCE(rule).
        DEFAULT is above, usually a REDUCE or else ERROR.
        ERROR, if not default, occurs only due to %nonassoc, otherwise it
            would be harmless to reduce and then error before shifting.
        ACCEPT is basically REDUCE(0), but is implicit after shifting an EOF.
        SHIFT's state cannot be 0, so encode ERROR there.
        REDUCE's rule cannot be 0, but see ACCEPT.

        Therefore, use the following encoding:
        * positive numbers are SHIFT.
        * zero is ERROR.
        * negative numbers are REDUCE.
    */
    size_t first_term;
    size_t last_term;
    ssize_t *acts;
    /*
        Can only be DEFAULT or GOTO(state).
        DEFAULT is ERROR but should be unreachable from table construction.
        GOTO's state cannot be 0, so use it for ERROR.
    */
    size_t first_nonterm;
    size_t last_nonterm;
    size_t *gotos;
};

struct Automaton
{
    /* mutable state */
    size_t state_stack_top;
    size_t *state_stack;
    Tree *tree_stack;
    size_t stacks_size;
    size_t stacks_cap;

    /* fixed references */
    size_t alloc_states;
    State *states;
    Grammar *grammar;
};


Rule *rule_create(size_t lhs, size_t num_rhses, size_t *rhses)
{
    Rule rv;
    rv.lhs = lhs;
    assert(num_rhses != 0);
    rv.num_rhses = num_rhses;
    rv.rhses = (size_t *)memdup(rhses, num_rhses * sizeof(*rhses));
    return (Rule *)memdup(&rv, sizeof(rv));
}


Grammar *grammar_create(size_t num_symbols, size_t num_nonterminals, size_t num_rules, Rule **rules)
{
    size_t i;
    Grammar rv;
    rv.rules_by_nonterminal = (size_t *)calloc(num_nonterminals, sizeof(size_t));
    {
        size_t prev_nonterminal = (size_t)-1;
        BitSet *seen_nonterminals = bitset_create(num_nonterminals);
        if (!num_rules || rules[0]->lhs != num_symbols)
            abort();
        bitset_set(seen_nonterminals, num_symbols - num_symbols);
        i = 0;
        rv.rules_by_nonterminal[num_symbols - num_symbols] = i;
        for (i = 1; i < num_rules; ++i)
        {
            size_t cur_nonterminal = rules[i]->lhs;
            if (cur_nonterminal == prev_nonterminal)
                continue;
            if (bitset_test(seen_nonterminals, cur_nonterminal - num_symbols))
                abort();
            bitset_set(seen_nonterminals, cur_nonterminal - num_symbols);
            rv.rules_by_nonterminal[cur_nonterminal - num_symbols] = i;
            prev_nonterminal = cur_nonterminal;
        }
        if (!bitset_all(seen_nonterminals))
            abort();
        bitset_destroy(seen_nonterminals);
    }
    rv.num_symbols = num_symbols;
    rv.num_nonterminals = num_nonterminals;
    rv.num_rules = num_rules;
    rv.rules = (Rule *)malloc(sizeof(Rule) * num_rules);
    for (i = 0; i < num_rules; ++i)
    {
        rv.rules[i] = *rules[i];
        free(rules[i]);
    }
    return (Grammar *)memdup(&rv, sizeof(rv));
}

void grammar_destroy(Grammar *g)
{
    size_t i;
    for (i = g->num_rules; i--; )
    {
        free(g->rules[i].rhses);
    }
    free(g->rules_by_nonterminal);
    free(g->rules);
    free(g);
}


static ssize_t encode(Action act, bool is_nonterm)
{
    switch (act.type)
    {
    case SHIFT: /* also GOTO */
    case REDUCE:
        assert ((ssize_t)act.value > 0);
        break;
    case ERROR:
    case ACCEPT:
        assert (act.value == 0);
        act.type -= 2;
        break;
    default:
        abort();
    }
    if (act.type == REDUCE)
    {
        assert (!is_nonterm);
        return (ssize_t)-act.value;
    }
    /* Note that ERROR and ACCEPT both return 0. */
    return (ssize_t)act.value;
}
static size_t skip_forward(const void *elt, size_t elt_size, const void *arr, size_t arr_elts)
{
    const char *cur;
    size_t i;
    for (i = 0, cur = (const char *)arr; i < arr_elts; ++i, cur += elt_size)
    {
        if (memcmp(elt, cur, elt_size) != 0)
        {
            break;
        }
    }
    return i;
}

static size_t skip_backward(const void *elt, size_t elt_size, const void *arr, size_t arr_elts)
{
    const char *cur;
    size_t i;
    for (i = arr_elts, cur = elt_size * arr_elts + (const char *)arr; i-- && (cur -= elt_size, 1); )
    {
        if (memcmp(elt, cur, elt_size) != 0)
        {
            break;
        }
    }
    return i;
}

State *state_create(Grammar *g, Action def, Action *term_acts, Action *nonterm_acts)
{
    const Action error = {ERROR, 0};
    size_t i;
    State rv;
    rv.def = encode(def, false);
    rv.first_term = skip_forward(&def, sizeof(def), term_acts, g->num_symbols);
    if (rv.first_term == g->num_symbols)
    {
        rv.last_term = 0;
        rv.acts = NULL;
    }
    else
    {
        rv.last_term = skip_backward(&def, sizeof(def), term_acts, g->num_symbols);
        rv.acts = (ssize_t *)malloc((rv.last_term - rv.first_term + 1) * sizeof(*rv.acts));
        for (i = rv.first_term; i <= rv.last_term; ++i)
        {
            rv.acts[i - rv.first_term] = encode(term_acts[i], false);
        }
    }
    rv.first_nonterm = skip_forward(&error, sizeof(error), nonterm_acts, g->num_nonterminals);
    if (rv.first_nonterm == g->num_nonterminals)
    {
        rv.last_nonterm = 0;
        rv.gotos = NULL;
    }
    else
    {
        rv.last_nonterm = skip_backward(&error, sizeof(error), nonterm_acts, g->num_nonterminals);
        rv.gotos = (size_t *)malloc((rv.last_nonterm - rv.first_nonterm + 1) * sizeof(*rv.acts));
        for (i = rv.first_nonterm; i <= rv.last_nonterm; ++i)
        {
            rv.gotos[i - rv.first_nonterm] = encode(nonterm_acts[i], true);
        }
    }
    rv.first_nonterm += g->num_symbols;
    rv.last_nonterm += g->num_symbols;
    return (State *)memdup(&rv, sizeof(rv));
}


Automaton *automaton_create(Grammar *g, size_t num_states, State **states)
{
    size_t i;
    Automaton rv;

    assert (g->rules[0].num_rhses == 2);
    assert (g->rules[0].rhses[1] == 0);

    rv.state_stack_top = 0;
    rv.stacks_size = 0;
    rv.stacks_cap = 16;
    rv.state_stack = (size_t *)malloc(rv.stacks_cap * sizeof(*rv.state_stack));
    rv.tree_stack = (Tree *)malloc(rv.stacks_cap * sizeof(*rv.tree_stack));
    rv.alloc_states = num_states;
    rv.states = (State *)malloc(num_states * sizeof(*rv.states));
    for (i = 0; i < num_states; ++i)
    {
        rv.states[i] = *states[i];
        free(states[i]);
    }
    rv.grammar = g;
    return (Automaton *)memdup(&rv, sizeof(rv));
}

static void free_trees(Tree *trees, size_t num);
static void free_tree(Tree *t)
{
    if (t->type == 0)
    {
        return;
    }
    if (t->num_children == 0)
    {
        free(t->token);
    }
    else
    {
        free_trees(t->children, t->num_children);
    }
}

static void free_trees(Tree *trees, size_t num)
{
    size_t i;
    for (i = num; i--; )
    {
        free_tree(&trees[i]);
    }
    free(trees);
}

void automaton_destroy(Automaton *a)
{
    size_t i;
    (void)a->grammar;
    for (i = a->alloc_states; i--; )
    {
        free(a->states[i].gotos);
        free(a->states[i].acts);
    }
    free(a->states);
    free_trees(a->tree_stack, a->stacks_size);
    free(a->state_stack);
    free(a);
}

void automaton_reset(Automaton *a)
{
    size_t i;
    for (i = a->stacks_size; i--; )
    {
        free_tree(&a->tree_stack[i]);
    }
    a->stacks_size = 0;
    a->state_stack_top = 0;
}


static ssize_t get_act(State *state, size_t sym)
{
    if (sym < state->first_term || sym > state->last_term)
    {
        return state->def;
    }
    return state->acts[sym - state->first_term];
}

static size_t get_goto(State *state, size_t sym)
{
    if (sym < state->first_nonterm || sym > state->last_nonterm)
    {
        return 0;
    }
    return state->gotos[sym - state->first_nonterm];
}

static Tree make_tree(size_t sym, Tree *trees, size_t num_trees)
{
    Tree rv;
    rv.type = sym;
    rv.num_children = num_trees;
    rv.xxx_total_tokens_or_maybe_total_token_length_or_maybe_even_rule = 0;
    rv.children = (Tree *)memdup(trees, num_trees * sizeof(*trees));
    return rv;
}

bool automaton_feed_term(Automaton *a, size_t sym, const char *str, size_t len)
{
    Tree term;
    term.type = sym;
    term.num_children = 0;
    term.token_length = len;
    if (sym != 0)
    {
        term.token = strdup(str);
        assert (len != 0);
    }
    else
    {
        term.token = NULL;
        assert (len == 0);
    }
    while (true)
    {
        size_t state = a->state_stack_top;
        ssize_t act = get_act(&a->states[state], sym);
        if (!act)
        {
            return false;
        }
        if (act < 0)
        {
            /* Note: this code is wrong if you have empty rules. */
            size_t rule_no = -(size_t)act;
            Rule *rule = &a->grammar->rules[rule_no];
            size_t lhs = rule->lhs;
            size_t count = rule->num_rhses;
            size_t new_size = a->stacks_size - count;
            Tree *trees = a->tree_stack + new_size;
            Tree new_tree = make_tree(lhs, trees, count);
            size_t old_new_top = a->state_stack[new_size];
            size_t new_new_top = get_goto(&a->states[old_new_top], lhs);
            assert (new_new_top != 0);
            *trees = new_tree;
            /* a->state_stack[new_size] = old_new_top */
            a->state_stack_top = new_new_top;
            a->stacks_size = new_size + 1;
            continue;
        }
        else
        {
            size_t state_no = (size_t)act;
            size_t idx = a->stacks_size++;
            if (idx == a->stacks_cap)
            {
                size_t new_cap = idx * 2;
                a->stacks_cap = new_cap;
                a->state_stack = (size_t *)realloc(a->state_stack, new_cap * sizeof(*a->state_stack));
                a->tree_stack = (Tree *)realloc(a->tree_stack, new_cap * sizeof(*a->tree_stack));
            }
            a->state_stack[idx] = state; /* old top */
            a->state_stack_top = state_no;
            a->tree_stack[idx] = term;
            return true;
        }
    }
}

Tree *automaton_result(Automaton *a)
{
    return &a->tree_stack[0];
}

size_t automaton_tree_count(Automaton *a)
{
    return a->stacks_size;
}
