#include "hashmap.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "PMurHash.h"


#define GROW 3/4

typedef struct HashChain HashChain;
struct HashChain
{
    HashChain *next;
    HashEntry entry;
    uint32_t hash;
};

struct HashMap
{
    HashChain **buckets;
    size_t num_buckets;
    size_t num_entries;
};

HashMap *map_create(void)
{
    HashMap *rv = (HashMap *)calloc(1, sizeof(HashMap));
    rv->num_entries = 0;
    rv->num_buckets = 16;
    rv->buckets = (HashChain **)calloc(rv->num_buckets, sizeof(HashChain *));
    return rv;
}

void map_destroy(HashMap *map)
{
    size_t i;
    for (i = 0; i < map->num_buckets; ++i)
    {
        HashChain *bucket = map->buckets[i];
        while (bucket)
        {
            HashChain *next = bucket->next;
            free(bucket->entry.key.data);
            /* value is not our problem, but there is no iteration ... */
            free(bucket);
            bucket = next;
        }
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

HashEntry *map_entry(HashMap *map, HashKey key)
{
    uint32_t h = hash(key);
    HashChain *bucket = map->buckets[h % map->num_buckets];
    while (bucket)
    {
        if (bucket->hash == h && bucket->entry.key.len == key.len)
        {
            if (memcmp(bucket->entry.key.data, key.data, key.len) == 0)
            {
                return &bucket->entry;
            }
        }
    }
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
            HashChain **old_chain = &new_buckets[i];
            HashChain **new_chain = &new_buckets[old_num_buckets + i];
            while (*old_chain)
            {
                if ((*old_chain)->hash & old_num_buckets)
                {
                    HashChain *old_next = (*old_chain)->next;
                    (*old_chain)->next = *new_chain;
                    *new_chain = *old_chain;
                    *old_chain = old_next;
                }
                else
                {
                    old_chain = &(*old_chain)->next;
                }
            }
        }
    }
    bucket = (HashChain *)calloc(1, sizeof(HashChain));
    bucket->next = map->buckets[h % map->num_buckets];
    bucket->entry.key.data = (unsigned char *)malloc(key.len);
    memcpy(bucket->entry.key.data, key.data, key.len);
    bucket->entry.key.len = key.len;
    bucket->entry.value.ptr = NULL;
    bucket->hash = h;
    map->buckets[h % map->num_buckets] = bucket;
    return &bucket->entry;
}
