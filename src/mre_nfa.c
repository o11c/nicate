#include "mre.h"
#include "mre_internal.h"
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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "bitset.h"
#include "hashmap.h"
#include "pool.h"


/* These numbers are specially chosen and MultiNfa will break if changed. */
#define NFA_START 0
#define NFA_ACCEPT 1

typedef struct NfaPair NfaPair;
typedef struct NfaEpsilonKey NfaEpsilonKey;
typedef struct NfaCharKey NfaCharKey;

struct Nfa
{
    /*
        Bad, need to be able to enumerate all targets from node X.
        Since all new edges are guaranteed not to exist, why not just X?

        Workaround: just sort after iterating.
    */
    HashMap *epsilon_edges;
    HashMap *character_edges;
    size_t num_states;
};
/* Key fields must precede value fields. */
struct NfaEpsilonKey
{
    size_t from, to;
};
struct NfaCharKey
{
    /* avoid padding */
    size_t from, ch, to;
};
struct NfaPair
{
    Nfa *a;
    Nfa *b;
};
struct MultiNfa
{
    Pool *pool;
    Nfa *nfa;
    size_t num_accept;
};


static void nfa_free(void *ptr)
{
    Nfa *nfa = (Nfa *)ptr;
    map_destroy(nfa->character_edges);
    map_destroy(nfa->epsilon_edges);
    free(nfa);
}
static Nfa *nfa_alloc(Pool *pool)
{
    Nfa *rv = (Nfa *)calloc(1, sizeof(*rv));
    rv->epsilon_edges = map_create();
    rv->character_edges = map_create();
    rv->num_states = 2;
    pool_free(pool, nfa_free, rv);
    return rv;
}
static size_t nfa_alloc_id(Nfa *nfa)
{
    return nfa->num_states++;
}
static void nfa_add_epsilon(Nfa *nfa, size_t from, size_t to)
{
    NfaEpsilonKey str = {from, to};
    HashKey key = {(unsigned char *)&str, sizeof(str)};
    HashEntry *entry = map_entry(nfa->epsilon_edges, key);
    (void)entry;
}
static void nfa_add_char(Nfa *nfa, size_t from, size_t to, char ch)
{
    NfaCharKey str = {from, (unsigned char)ch, to};
    HashKey key = {(unsigned char *)&str, sizeof(str)};
    HashEntry *entry = map_entry(nfa->character_edges, key);
    (void)entry;
}
static void do_merge_epsilon(HashMap *dst, HashMap *src, size_t off)
{
    HashIterator *it;
    for (it = map_first(src); it; it = map_next(it))
    {
        HashEntry *e = map_deref(it);
        HashKey key = e->key;
        NfaEpsilonKey ekey = *(NfaEpsilonKey *)key.data;
        ekey.from += off;
        ekey.to += off;
        key.data = (unsigned char *)&ekey;
        (void)map_entry(dst, key);
        /* Remember we don't do values here. */
    }
}
static void do_merge_char(HashMap *dst, HashMap *src, size_t off)
{
    HashIterator *it;
    for (it = map_first(src); it; it = map_next(it))
    {
        HashEntry *e = map_deref(it);
        HashKey key = e->key;
        NfaCharKey ckey = *(NfaCharKey *)key.data;
        ckey.from += off;
        ckey.to += off;
        key.data = (unsigned char *)&ckey;
        (void)map_entry(dst, key);
        /* Remember we don't do values here. */
    }
}
static size_t nfa_merge(Nfa *nfa, Nfa *inner)
{
    size_t rv = nfa->num_states;
    do_merge_epsilon(nfa->epsilon_edges, inner->epsilon_edges, nfa->num_states);
    do_merge_char(nfa->character_edges, inner->character_edges, nfa->num_states);
    nfa->num_states += inner->num_states;
    return rv;
}

static void *transform_text(Pool *pool, const void *str, size_t len)
{
    Nfa *rv = nfa_alloc(pool);
    const char *s = (const char *)str;
    if (!len)
    {
        nfa_add_epsilon(rv, NFA_START, NFA_ACCEPT);
    }
    else
    {
        size_t from = NFA_START;
        while (--len)
        {
            char c = *s++;
            size_t to = nfa_alloc_id(rv);
            nfa_add_char(rv, from, to, c);
            from = to;
        }
        nfa_add_char(rv, from, NFA_ACCEPT, *s);
    }
    return rv;
}
static void *transform_class(Pool *pool, const void *str, size_t len)
{
    CharBitSet *cbs = (CharBitSet *)str;
    Nfa *rv = nfa_alloc(pool);
    size_t i;
    assert (sizeof(*cbs) == len);
    for (i = 0; i < 256; ++i)
    {
        if (char_bitset_test(cbs, i))
        {
            nfa_add_char(rv, NFA_START, NFA_ACCEPT, i);
        }
    }
    return rv;
}
static void *transform_alt(Pool *pool, const void *str, size_t len)
{
    Nfa *rv = nfa_alloc(pool);
    NfaPair p = (assert (len == sizeof(p)), *(NfaPair *)str);
    size_t a_offset = nfa_merge(rv, p.a);
    size_t b_offset = nfa_merge(rv, p.b);
    nfa_add_epsilon(rv, NFA_START, a_offset + NFA_START);
    nfa_add_epsilon(rv, NFA_START, b_offset + NFA_START);
    nfa_add_epsilon(rv, a_offset + NFA_ACCEPT, NFA_ACCEPT);
    nfa_add_epsilon(rv, b_offset + NFA_ACCEPT, NFA_ACCEPT);
    return rv;
}
static void *transform_cat(Pool *pool, const void *str, size_t len)
{
    Nfa *rv = nfa_alloc(pool);
    NfaPair p = (assert (len == sizeof(p)), *(NfaPair *)str);
    size_t left_offset = nfa_merge(rv, p.a);
    size_t right_offset = nfa_merge(rv, p.b);
    nfa_add_epsilon(rv, NFA_START, left_offset + NFA_START);
    nfa_add_epsilon(rv, left_offset + NFA_ACCEPT, right_offset + NFA_START);
    nfa_add_epsilon(rv, right_offset + NFA_ACCEPT, NFA_ACCEPT);
    return rv;
}
/* TODO figure out which of these should be kept */
static void *transform_opt(Pool *pool, const void *str, size_t len)
{
    Nfa *rv = nfa_alloc(pool);
    Nfa *inner = (assert (len == sizeof(inner)), *(Nfa **)str);
    size_t inner_offset = nfa_merge(rv, inner);
    /*
        Snew -----> Anew
          |           ^
          v           |
        Sold========Snew
    */
    nfa_add_epsilon(rv, NFA_START, NFA_ACCEPT);
    nfa_add_epsilon(rv, NFA_START, inner_offset + NFA_START);
    nfa_add_epsilon(rv, inner_offset + NFA_ACCEPT, NFA_ACCEPT);
    return rv;
}
static void *transform_star(Pool *pool, const void *str, size_t len)
{
    Nfa *rv = nfa_alloc(pool);
    Nfa *inner = (assert (len == sizeof(inner)), *(Nfa **)str);
    size_t inner_offset = nfa_merge(rv, inner);
    /*
        Snew -----> Anew
          |           ^
          v           |
        Sold========Snew
          ^           |
           \---------/
    */
    nfa_add_epsilon(rv, NFA_START, NFA_ACCEPT);
    nfa_add_epsilon(rv, NFA_START, inner_offset + NFA_START);
    nfa_add_epsilon(rv, inner_offset + NFA_ACCEPT, inner_offset + NFA_START);
    nfa_add_epsilon(rv, inner_offset + NFA_ACCEPT, NFA_ACCEPT);
    return rv;
}
static void *transform_plus(Pool *pool, const void *str, size_t len)
{
    Nfa *rv = nfa_alloc(pool);
    Nfa *inner = (assert (len == sizeof(inner)), *(Nfa **)str);
    size_t inner_offset = nfa_merge(rv, inner);
    /*
        Snew        Anew
          |           ^
          v           |
        Sold========Snew
          ^           |
           \---------/
    */
    nfa_add_epsilon(rv, NFA_START, inner_offset + NFA_START);
    nfa_add_epsilon(rv, inner_offset + NFA_ACCEPT, inner_offset + NFA_START);
    nfa_add_epsilon(rv, inner_offset + NFA_ACCEPT, NFA_ACCEPT);
    return rv;
}

static CharBitSet char_bitset_from(const char *str, size_t len)
{
    CharBitSet rv;
    size_t i;
    char_bitset_erase(&rv);

    for (i = 0; i < len; ++i)
    {
        unsigned char c = str[i];
        char_bitset_set(&rv, c);
    }

    return rv;
}

Nfa *nfa_fail(Pool *pool)
{
    return nfa_class_slice(pool, "", 0);
}
Nfa *nfa_epsilon(Pool *pool)
{
    return nfa_text_slice(pool, "", 0);
}
Nfa *nfa_char(Pool *pool, char c)
{
    return nfa_text_slice(pool, &c, 1);
}
Nfa *nfa_any_char(Pool *pool)
{
    return nfa_class_compl(pool, "");
}
Nfa *nfa_text(Pool *pool, const char *str)
{
    return nfa_text_slice(pool, str, strlen(str));
}
Nfa *nfa_class(Pool *pool, const char *str)
{
    return nfa_class_slice(pool, str, strlen(str));
}
Nfa *nfa_class_compl(Pool *pool, const char *str)
{
    return nfa_class_compl_slice(pool, str, strlen(str));
}
Nfa *nfa_text_slice(Pool *pool, const char *str, size_t len)
{
    return (Nfa *)pool_intern_map(pool, transform_text, str, len);
}
Nfa *nfa_class_slice(Pool *pool, const char *str, size_t len)
{
    CharBitSet cbs = char_bitset_from(str, len);
    return nfa_class_set(pool, &cbs);
}
Nfa *nfa_class_compl_slice(Pool *pool, const char *str, size_t len)
{
    CharBitSet cbs = char_bitset_from(str, len);
    char_bitset_invert(&cbs);
    return nfa_class_set(pool, &cbs);
}
Nfa *nfa_class_set(Pool *pool, CharBitSet *cbs)
{
    return (Nfa *)pool_intern_map(pool, transform_class, cbs, sizeof(*cbs));
}
Nfa *nfa_alt(Pool *pool, Nfa *a, Nfa *b)
{
    NfaPair p;
    p.a = a;
    p.b = b;
    return (Nfa *)pool_intern_map(pool, transform_alt, &p, sizeof(p));
}
Nfa *nfa_cat(Pool *pool, Nfa *left, Nfa *right)
{
    NfaPair p;
    p.a = left;
    p.b = right;
    return (Nfa *)pool_intern_map(pool, transform_cat, &p, sizeof(p));
}
Nfa *nfa_opt(Pool *pool, Nfa *inner)
{
    return (Nfa *)pool_intern_map(pool, transform_opt, &inner, sizeof(inner));
}
Nfa *nfa_star(Pool *pool, Nfa *inner)
{
    return (Nfa *)pool_intern_map(pool, transform_star, &inner, sizeof(inner));
}
Nfa *nfa_plus(Pool *pool, Nfa *inner)
{
    return (Nfa *)pool_intern_map(pool, transform_plus, &inner, sizeof(inner));
}


MultiNfa *multi_nfa_create(void)
{
    MultiNfa *rv = (MultiNfa *)calloc(1, sizeof(*rv));
    rv->pool = pool_create();
    rv->nfa = nfa_alloc(rv->pool);
    /* TODO check the first merging */
    rv->num_accept = 0;
    return rv;
}

void multi_nfa_destroy(MultiNfa *m)
{
    pool_destroy(m->pool);
    free(m);
}

size_t multi_nfa_add(MultiNfa *m, Nfa *nfa)
{
    size_t old_start, middle_start;
    size_t i;
    Nfa *merged = nfa_alloc(m->pool);
    m->num_accept++;
    merged->num_states = m->num_accept + 1;
    old_start = nfa_merge(merged, m->nfa);
    middle_start = nfa_merge(merged, nfa);
    m->nfa = merged;
    assert (old_start == m->num_accept + 1);

    /*
        S
        |
        S1======A1--1
    */
    /*
        S---S1======A1--1
        |   S2======A2--2
        |   S3======A3--3
        |
        S4==========A4--4
    */
    if (old_start != 1)
        nfa_add_epsilon(merged, NFA_START, old_start);
    nfa_add_epsilon(merged, NFA_START, middle_start);
    for (i = 1; i < m->num_accept; ++i)
        nfa_add_epsilon(merged, old_start + i, NFA_START + i);
    nfa_add_epsilon(merged, middle_start + NFA_ACCEPT, NFA_START + m->num_accept);
    return m->num_accept;
}

/* Here be dragons. */

typedef struct EpsilonTransitions EpsilonTransitions;
typedef struct CharTransitions CharTransitions;
typedef struct StateMap StateMap;

struct EpsilonTransitions
{
    NfaEpsilonKey *begin, *end;
};

static int epsilon_key_compare(const void *a, const void *b)
{
    return memcmp(a, b, sizeof(NfaEpsilonKey));
}

static EpsilonTransitions *epsilon_transitions_create(Nfa *nfa)
{
    HashIterator *it;
    size_t i;
    size_t size = map_size(nfa->epsilon_edges);
    EpsilonTransitions *rv = (EpsilonTransitions *)calloc(1, sizeof(*rv));
    rv->begin = (NfaEpsilonKey *)calloc(size, sizeof(*rv->begin));
    rv->end = rv->begin + size;
    /* Both iterating backwards, so likely to end semi-sorted. */
    for (i = size, it = map_first(nfa->epsilon_edges); it && i--; it = map_next(it))
    {
        HashEntry *e = map_deref(it);
        memcpy(&rv->begin[i], e->key.data, sizeof(rv->begin[i]));
    }
    assert (!i && !it);
    qsort(rv->begin, size, sizeof(*rv->begin), epsilon_key_compare);
    return rv;
}

static void epsilon_transitions_destroy(EpsilonTransitions *et)
{
    free(et->begin);
    free(et);
}

static void epsilon_check(EpsilonTransitions *et, size_t from, NfaEpsilonKey *lower, NfaEpsilonKey *upper)
{
    if (et->begin == et->end)
    {
        assert (lower == upper);
        assert (lower == et->begin);
        return;
    }
    assert (lower <= upper);
    assert (lower >= et->begin);
    assert (upper <= et->end);
    if (lower != upper)
    {
        assert (lower == et->begin || memcmp(&lower[-1].from, &from, sizeof(size_t)) < 0);
        assert (upper == et->end || memcmp(&upper->from, &from, sizeof(size_t)) > 0);
        assert (lower->from == from);
        assert (upper[-1].from == from);
    }
}
static void *average(void *lower, void *upper, size_t element_size)
{
    char *l = (char *)lower;
    char *u = (char *)upper;
    return l + element_size * ((u - l) / element_size / 2);
    /* return lower + (upper - lower) / 2; */
}
static bool adjacent(void *lower, void *upper, size_t element_size)
{
    char *l = (char *)lower;
    char *u = (char *)upper;
    return l + element_size == u;
    /* return lower + 1 == upper; */
}
static void *prev(void *upper, size_t element_size)
{
    char *u = (char *)upper;
    return u - element_size;
    /* return &upper[-1]; */
}
static void double_binary_search(void **begin, void **end, size_t element_size, void *key, size_t key_size)
{
    /*
        Returns, if successful:
            *begin < *end
            (*begin)->from == from
            (*end)[-1].from == from
            *end is singular or (*end)->from > from
        Returns, if not successful:
            *begin == *end
            *end is singular or (*end)->from > from
    */
    void *lower, *middle, *upper;
    int cmp;
    lower = *begin;
    upper = *end;

    if (lower == upper)
    {
        *begin = *end = lower;
        return;
    }
    if (memcmp(lower, key, key_size) > 0)
    {
        *begin = *end = lower;
        return;
    }
    if (memcmp(prev(upper, element_size), key, key_size) < 0)
    {
        *begin = *end = upper;
        return;
    }
    /*
        Invariants:
        lower < end
        lower is not singular and lower->from <= from.
        lower - 1 is singular or lower[-1] < from
        upper - 1 is not singular and upper[-1].from >= from.
        upper is singular or upper->from > from.
    */
    while (true)
    {
        if (adjacent(lower, upper, element_size))
        {
            if (memcmp(lower, key, key_size) == 0)
            {
                *begin = lower;
                *end = upper;
                return;
            }
            else
            {
                /* lower->from < from */
                *begin = *end = upper;
                return;
            }
        }
        /*
            lower < middle < upper
            middle <= upper - 1
            lower->from <= middle->from <= upper[-1].from
            lower->from <= from
            upper[-1].from >= from
        */
        middle = average(lower, upper, element_size);
        cmp = memcmp(middle, key, key_size);
        if (cmp < 0)
        {
            lower = middle;
            continue;
        }
        if (cmp > 0)
        {
            upper = middle;
            continue;
        }
        /*
            middle->from == from
            Split and do each half's search separately.
        */
        break;
    }
    if (memcmp(lower, key, key_size) != 0)
    {
        void *lower_middle;
        void *lower_upper = middle;
        while (true)
        {
            if (adjacent(lower, lower_upper, element_size))
            {
                lower = lower_upper;
                break;
            }
            /*
                lower < lower_middle < lower_upper
                lower->from < from
                lower_upper->from == from
                lower->from <= lower_middle->from <= lower_upper->from
            */
            lower_middle = average(lower, lower_upper, element_size);
            if (memcmp(lower_middle, key, key_size) == 0)
            {
                lower_upper = lower_middle;
            }
            else
            {
                lower = lower_middle;
            }
        }
    }
    if (memcmp(prev(upper, element_size), key, key_size) != 0)
    {
        void *upper_middle;
        void *upper_lower = middle;
        while (true)
        {
            if (adjacent(upper_lower, upper, element_size))
            {
                break;
            }
            /*
                upper_lower < upper_middle < upper
                upper_lower->from == from
                upper[-1].from > from
                upper_lower->from <= upper_middle->from <= upper[-1].from
            */
            upper_middle = average(upper_lower, upper, element_size);
            if (memcmp(upper_middle, key, key_size) == 0)
            {
                upper_lower = upper_middle;
            }
            else
            {
                upper = upper_middle;
            }
        }
    }

    *begin = lower;
    *end = upper;
    return;
}
static void epsilon_transitions_lookup(EpsilonTransitions *et, size_t from, NfaEpsilonKey **begin, NfaEpsilonKey **end)
{
    size_t key[1] = {from};
    *begin = et->begin;
    *end = et->end;
    double_binary_search((void **)begin, (void **)end, sizeof(**begin), &key, sizeof(key));
    epsilon_check(et, from, *begin, *end);
}


struct CharTransitions
{
    NfaCharKey *begin, *end;
};

static int char_key_compare(const void *a, const void *b)
{
    return memcmp(a, b, sizeof(NfaCharKey));
}

static CharTransitions *char_transitions_create(Nfa *nfa)
{
    HashIterator *it;
    size_t i;
    size_t size = map_size(nfa->character_edges);
    CharTransitions *rv = (CharTransitions *)calloc(1, sizeof(*rv));
    rv->begin = (NfaCharKey *)calloc(size, sizeof(*rv->begin));
    rv->end = rv->begin + size;
    for (i = size, it = map_first(nfa->character_edges); it && i--; it = map_next(it))
    {
        HashEntry *e = map_deref(it);
        memcpy(&rv->begin[i], e->key.data, sizeof(rv->begin[i]));
    }
    assert (!i && !it);
    qsort(rv->begin, size, sizeof(*rv->begin), char_key_compare);
    return rv;
}

static void char_transitions_destroy(CharTransitions *ct)
{
    free(ct->begin);
    free(ct);
}

static void char_transitions_lookup_all(CharTransitions *ct, size_t from, NfaCharKey **begin, NfaCharKey **end)
{
    size_t key[1] = {from};
    *begin = ct->begin;
    *end = ct->end;
    double_binary_search((void **)begin, (void **)end, sizeof(**begin), &key, sizeof(key));
}

static void char_transitions_lookup_one(CharTransitions *ct, size_t from, size_t ch, NfaCharKey **begin, NfaCharKey **end)
{
    size_t key[2] = {from, ch};
    *begin = ct->begin;
    *end = ct->end;
    double_binary_search((void **)begin, (void **)end, sizeof(**begin), &key, sizeof(key));
}


struct StateMap
{
    HashMap *hash;
    HashKey *vec;
    size_t vec_cap;
};
static StateMap *statemap_create(void)
{
    StateMap *rv = (StateMap *)calloc(1, sizeof(*rv));
    rv->hash = map_create();
    rv->vec_cap = 16;
    rv->vec = (HashKey *)calloc(rv->vec_cap, sizeof(*rv->vec));
    return rv;
}

static void statemap_destroy(StateMap *sm)
{
    map_destroy(sm->hash);
    free(sm->vec);
    free(sm);
}

static size_t statemap_intern(StateMap *sm, BitSet *bs)
{
    HashKey key = bitset_as_key(bs);
    size_t old_size = map_size(sm->hash);
    HashEntry *entry = map_entry(sm->hash, key);
    size_t new_size = map_size(sm->hash);
    if (old_size != new_size)
    {
        entry->value.ptr = (void *)old_size;
        if (old_size == sm->vec_cap)
        {
            size_t old_cap = sm->vec_cap;
            size_t new_cap = old_cap * 2;
            HashKey *new_vec = (HashKey *)realloc(sm->vec, new_cap * sizeof(*new_vec));
            memset(new_vec + old_cap, '\0', old_cap * sizeof(*new_vec));
            sm->vec = new_vec;
            sm->vec_cap = new_cap;
        }
        sm->vec[old_size] = entry->key;
    }
    return (size_t)entry->value.ptr;
}

static void statemap_index(StateMap *sm, size_t i, BitSet *bs)
{
    HashKey in_key = sm->vec[i];
    HashKey out_key = bitset_as_key(bs);
    /* Yes, it really is that simple. */
    memcpy(out_key.data, in_key.data, out_key.len);
}

static size_t statemap_size(StateMap *sm)
{
    return map_size(sm->hash);
}


static size_t calc_accept(BitSet *states, size_t num_accept)
{
    size_t i;
    for (i = 1; i <= num_accept; ++i)
    {
        if (bitset_test(states, i))
        {
            return i;
        }
    }
    return 0;
}
static void fill_epsilons_from(BitSet *states, EpsilonTransitions *transitions, size_t from)
{
    NfaEpsilonKey *begin, *end, *it;
    epsilon_transitions_lookup(transitions, from, &begin, &end);
    for (it = begin; it != end; ++it)
    {
        size_t to = it->to;
        bool was = bitset_test(states, to);
        if (!was)
        {
            bitset_set(states, to);
            if (to < from)
            {
                fill_epsilons_from(states, transitions, to);
            }
        }
    }
}
static void fill_epsilons(BitSet *states, size_t num_states, size_t num_accept, EpsilonTransitions *transitions, CharTransitions *char_transitions)
{
    size_t si;
    for (si = 0; si < num_states; ++si)
    {
        if (bitset_test(states, si))
        {
            fill_epsilons_from(states, transitions, si);
        }
    }

    for (si = 0; si < num_states; ++si)
    {
        if (bitset_test(states, si))
        {
            NfaCharKey *begin, *end;
            if (si > 0 && si <= num_accept)
            {
                continue;
            }
            char_transitions_lookup_all(char_transitions, si, &begin, &end);
            if (begin == end)
            {
                bitset_unset(states, si);
            }
        }
    }
}
static void fill_chars(BitSet *current, BitSet *next, size_t num_states, CharTransitions *transitions, size_t c)
{
    size_t ci;
    for (ci = 0; ci < num_states; ++ci)
    {
        if (bitset_test(current, ci))
        {
            NfaCharKey *begin, *end, *it;
            char_transitions_lookup_one(transitions, ci, c, &begin, &end);
            for (it = begin; it != end; ++it)
            {
                size_t to = it->to;
                bitset_set(next, to);
            }
        }
    }
}

static MreState *multi_nfa_to_dfa_impl(MultiNfa *m)
{
    /*
        Input:
        state 0 is start
        states 1 thorugh m->num_accept are accept

        Output:
        state 0 is fail
        state 1 is start
        any state may be accept
     */
    const size_t num_accept = m->num_accept;
    const size_t num_states = m->nfa->num_states;
    MreState *rv;


    /* setup */
    BitSet *did_accept = bitset_create(num_accept);
    EpsilonTransitions *epsilon_transitions = epsilon_transitions_create(m->nfa);
    CharTransitions *char_transitions = char_transitions_create(m->nfa);
    BitSet *current_states = bitset_create(num_states);
    BitSet *next_states = bitset_create(num_states);
    StateMap *state_map = statemap_create();

    /* fail = 0 */
    (void)statemap_intern(state_map, current_states);
    bitset_set(current_states, NFA_START);
    fill_epsilons(current_states, num_states, num_accept, epsilon_transitions, char_transitions);
    if (calc_accept(current_states, num_accept))
        abort();
    /* start = 1*/
    if (!statemap_intern(state_map, current_states))
        abort();


    /* main loop */
    {
        size_t rv_cap = 16;
        rv = (MreState *)calloc(rv_cap, sizeof(*rv));
        size_t i;
        /* statemap keeps growing as we go */
        for (i = 1; i < statemap_size(state_map); ++i)
        {
            MreState *rvi;
            size_t c;
            if (i == rv_cap)
            {
                size_t old_rv_bytes = rv_cap * sizeof(*rv);
                size_t new_rv_cap = rv_cap * 2;
                MreState *new_rv = (MreState *)realloc(rv, new_rv_cap * sizeof(*rv));
                memset((char *)new_rv + old_rv_bytes, '\0', old_rv_bytes);
                rv_cap = new_rv_cap;
                rv = new_rv;
            }
            rvi = &rv[i];
            statemap_index(state_map, i, current_states);
            rvi->accept = calc_accept(current_states, num_accept);
            if (rvi->accept)
            {
                bitset_set(did_accept, rvi->accept - 1);
            }
            for (c = 0; c < 256; ++c)
            {
                bitset_erase(next_states);
                fill_chars(current_states, next_states, num_states, char_transitions, c);
                fill_epsilons(next_states, num_states, num_accept, epsilon_transitions, char_transitions);
                rvi->gotos[c] = statemap_intern(state_map, next_states);
            }
        }
    }


    /* teardown */
    if (!bitset_any(did_accept))
    {
        /* TODO warn about partials */
        abort();
    }

    statemap_destroy(state_map);
    bitset_destroy(next_states);
    bitset_destroy(current_states);
    char_transitions_destroy(char_transitions);
    epsilon_transitions_destroy(epsilon_transitions);
    bitset_destroy(did_accept);

    /* TODO Do a final merging step here? Only useful on pedantic input? */

    return rv;
}

MreRules *multi_nfa_to_dfa(MultiNfa *m)
{
    MreRules *rv = (MreRules *)calloc(1, sizeof(*rv));
    rv->refcount = 1;
    rv->num_rules = m->num_accept;
    rv->states = multi_nfa_to_dfa_impl(m);
    return rv;
}
