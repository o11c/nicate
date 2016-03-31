#include "pool.h"
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

#include <stdlib.h>
#include <string.h>

#include "hashmap.h"


/*
    TODO group by free_func, it's probably a huge win.

    Probably requires adding iterators to hashmap, which requires adding
    a linked-list field for order-of-insertion (to preserve determinism).
 */
typedef struct FreeEntry FreeEntry;
struct FreeEntry
{
    PoolFree free_func;
    void *ptr;
};

struct Pool
{
    HashMap *interns;
    HashMap *transforms;
    FreeEntry *free_list;
    size_t free_size;
    size_t free_cap;
};

Pool *pool_create(void)
{
    Pool *rv = (Pool *)calloc(1, sizeof(Pool));
    rv->interns = map_create();
    rv->transforms = map_create();
    rv->free_size = 0;
    rv->free_cap = 16;
    rv->free_list = (FreeEntry *)calloc(rv->free_cap, sizeof(*rv->free_list));
    return rv;
}

void pool_destroy(Pool *pool)
{
    size_t i = pool->free_size;
    while (i-- > 0)
    {
        FreeEntry *fli = &pool->free_list[i];
        fli->free_func(fli->ptr);
    }
    free(pool->free_list);
    map_destroy(pool->transforms);
    map_destroy(pool->interns);
    free(pool);
}

void pool_free(Pool *pool, PoolFree free_func, void *ptr)
{
    FreeEntry entry = {free_func, ptr};
    if (pool->free_size == pool->free_cap)
    {
        pool->free_cap *= 2;
        pool->free_list = (FreeEntry *)realloc(pool->free_list, pool->free_cap * sizeof(*pool->free_list));
    }
    pool->free_list[pool->free_size++] = entry;
}

const void *pool_intern(Pool *pool, const void *str, size_t len)
{
    HashKey key = {(unsigned char *)(void *)str, len};
    HashEntry *entry = map_entry(pool->interns, key, SEARCH_OR_INSERT);
    /* value is not used */
    return entry->key.data;
}

const char *pool_intern_string(Pool *pool, const char *str)
{
    return (const char *)pool_intern(pool, str, strlen(str) + 1);
}

const void *pool_intern_map(Pool *pool, PoolTransform transform, const void *str, size_t len, void *context)
{
    struct
    {
        PoolTransform transform_key;
        const void *interned_key;
    } new_key = {
        transform,
        (str = pool_intern(pool, str, len)),
    };
    HashKey key = {(unsigned char *)&new_key, sizeof(new_key)};
    HashEntry *entry = map_entry(pool->transforms, key, SEARCH_OR_INSERT);
    if (!entry->value.ptr)
    {
        entry->value.ptr = transform(pool, str, len, context);
    }
    return entry->value.ptr;
}
