#include "hashmap.h"
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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "PMurHash.h"
#include "util.h"


#define GROW 3/4

typedef HashIterator HashChain;
struct HashIterator
{
    HashChain *next;
    HashChain *iter;
    HashEntry entry;
    uint32_t hash;
};

struct HashMap
{
    HashChain **buckets;
    HashChain *iter;
    size_t num_buckets;
    size_t num_entries;
};

HashMap *map_create(void)
{
    HashMap *rv = (HashMap *)calloc(1, sizeof(HashMap));
    rv->num_entries = 0;
    rv->num_buckets = 16;
    rv->iter = NULL;
    rv->buckets = (HashChain **)calloc(rv->num_buckets, sizeof(HashChain *));
    return rv;
}

void map_destroy(HashMap *map)
{
    {
        size_t size = map->num_entries;
        HashChain *bucket = map->iter;
        while (bucket)
        {
            HashChain *next = bucket->iter;
            free(bucket->entry.key.data);
            /* value is not our problem */
            free(bucket);
            bucket = next;
            --size;
        }
        assert (size == 0);
    }
    free(map->buckets);
    free(map);
}

static uint32_t hash(HashKey key)
{
    /* This is a bug in upstream MurmurHash, but it's not worth forking yet. */
    assert(key.len <= (size_t)INT_MAX);
    return PMurHash32(0U, key.data, (int)key.len);
}

HashEntry *map_entry(HashMap *map, HashKey key, HashLookupPolicy policy)
{
    uint32_t h = hash(key);
    HashChain *bucket = map->buckets[h % map->num_buckets];
    while (bucket)
    {
        if (bucket->hash == h && bucket->entry.key.len == key.len)
        {
            if (memcmp(bucket->entry.key.data, key.data, key.len) == 0)
            {
                if (policy == INSERT_ONLY)
                    abort();
                return &bucket->entry;
            }
        }
        bucket = bucket->next;
    }
    if (policy == SEARCH_ONLY)
        return NULL;
    ++map->num_entries;
    if (map->num_entries > map->num_buckets * GROW)
    {
        size_t i;
        size_t old_num_buckets = map->num_buckets;
        size_t new_num_buckets = old_num_buckets * 2;
        HashChain **new_buckets = (HashChain **)realloc(map->buckets, new_num_buckets * sizeof(HashChain *));
        memset(new_buckets + old_num_buckets, '\0', old_num_buckets * sizeof(HashChain *));
        map->buckets = new_buckets;
        map->num_buckets = new_num_buckets;
        for (i = 0; i < old_num_buckets; ++i)
        {
            /*
                Since the number of buckets always doubles, it is always
                a power of two, so split the hash chain into two chains
                based on the next bit.
            */
            HashChain **old_chain = &new_buckets[i];
            HashChain **new_chain = &new_buckets[old_num_buckets + i];
            while (*old_chain)
            {
                if ((*old_chain)->hash & old_num_buckets)
                {
                    /*
                        Current element needs to be moved.
                        Note map_pop() requires the invariant that the most
                        recently added element (including after previous
                        pops) is always the top of the bucket, so we have
                        to preserve order rather than swapping.

                        O_o -> N_n -> O_n...
                        N_o -> null

                        O_o -> O_n...
                        N_o -> N_n -> null
                    */
                    HashChain *old_next = (*old_chain)->next;
                    assert (*new_chain == NULL);
                    *new_chain = *old_chain;
                    *old_chain = old_next;
                    new_chain = &(*new_chain)->next;
                    /* Will be overwritten except for the last. */
                    *new_chain = NULL;
                }
                else
                {
                    /*
                        Current element is already in the right bucket,
                        so just advance. Trivially maintains the invariant.

                        O_o -> O_n...
                    */
                    old_chain = &(*old_chain)->next;
                }
            }
        }
    }
    bucket = (HashChain *)calloc(1, sizeof(HashChain));
    bucket->next = map->buckets[h % map->num_buckets];
    bucket->iter = map->iter;
    bucket->entry.key.data = (unsigned char *)memdup(key.data, key.len);
    bucket->entry.key.len = key.len;
    bucket->entry.value.ptr = NULL;
    bucket->hash = h;
    map->buckets[h % map->num_buckets] = bucket;
    map->iter = bucket;
    return &bucket->entry;
}

HashValue map_pop(HashMap *map)
{
    HashValue rv = {NULL};
    if (map->iter)
    {
        HashChain *e = map->iter;
        HashChain **bucket = &map->buckets[e->hash % map->num_buckets];
        rv = e->entry.value;
        free(e->entry.key.data);
        map->iter = e->iter;
        assert (*bucket == e);
        *bucket = e->next;
        --map->num_entries;
    }
    return rv;
}

HashIterator *map_first(HashMap *map)
{
    return map->iter;
}
HashIterator *map_next(HashIterator *it)
{
    return it->iter;
}
HashEntry *map_deref(HashIterator *it)
{
    return &it->entry;
}

size_t map_size(HashMap *map)
{
    return map->num_entries;
}
