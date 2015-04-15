#include "pool.h"

#include <stdlib.h>
#include <string.h>

#include "hashmap.h"


struct Pool
{
    HashMap *interns;
    HashMap *single_quoted;
    HashMap *double_quoted;
    void **free_list;
    size_t free_size;
    size_t free_cap;
};

Pool *pool_create(void)
{
    Pool *rv = (Pool *)calloc(1, sizeof(Pool));
    rv->interns = map_create();
    rv->single_quoted = map_create();
    rv->double_quoted = map_create();
    rv->free_list = (void **)calloc(16, sizeof(void *));
    rv->free_size = 0;
    rv->free_cap = 16;
    return rv;
}

void pool_destroy(Pool *pool)
{
    size_t i = pool->free_size;
    while (i-- > 0)
    {
        free(pool->free_list[i]);
    }
    free(pool->free_list);
    map_destroy(pool->double_quoted);
    map_destroy(pool->single_quoted);
    map_destroy(pool->interns);
    free(pool);
}

static void pool_free(Pool *pool, void *ptr)
{
    if (pool->free_size == pool->free_cap)
    {
        pool->free_cap *= 2;
        pool->free_list = (void **)realloc(pool->free_list, pool->free_cap * sizeof(void *));
    }
    pool->free_list[pool->free_size++] = ptr;
}

const void *pool_intern(Pool *pool, const void *str, size_t len)
{
    HashKey key = {(unsigned char *)(void *)str, len};
    HashEntry *entry = map_entry(pool->interns, key);
    /* value is not used */
    return entry->key.data;
}

const char *pool_intern_string(Pool *pool, const char *str)
{
    return (const char *)pool_intern(pool, str, strlen(str) + 1);
}

static char *really_do_quote(const char *s, size_t l, char q)
{
    size_t a = 3 + l;
    size_t i;
    char *m;
    char *p;
    for (i = 0; i < l; ++i)
    {
        unsigned char c = s[i];
        switch (c)
        {
        case '\a':
        case '\b':
        case '\f':
        case '\n':
        case '\r':
        case '\t':
        case '\v':
            a++;
            continue;
        }
        if (c == q || c == '\\')
            a++;
        else if (c < 0x20 || c >= 0x7f)
            a += 3;

    }
    p = m = (char *)malloc(a);
    *p++ = q;
    for (i = 0; i < l; ++i)
    {
        unsigned char c = s[i];
        switch (c)
        {
        case '\a':
            *p++ = '\\';
            *p++ = 'a';
            continue;
        case '\b':
            *p++ = '\\';
            *p++ = 'b';
            continue;
        case '\f':
            *p++ = '\\';
            *p++ = 'f';
            continue;
        case '\n':
            *p++ = '\\';
            *p++ = 'n';
            continue;
        case '\r':
            *p++ = '\\';
            *p++ = 'r';
            continue;
        case '\t':
            *p++ = '\\';
            *p++ = 't';
            continue;
        case '\v':
            *p++ = '\\';
            *p++ = 'v';
            continue;
        }
        if (c == q || c == '\\')
        {
            *p++ = '\\';
            *p++ = c;
            continue;
        }
        if (c < 0x20 || c >= 0x7f)
        {
            /* octal because \x is not limited to 2 digits */
            *p++ = '\\';
            *p++ = '0' + ((c >> 6) & 3);
            *p++ = '0' + ((c >> 3) & 7);
            *p++ = '0' + ((c >> 0) & 7);
            continue;
        }
        *p++ = c;
    }
    *p++ = q;
    *p = '\0';
    return m;
}

static const char *do_quote(Pool *pool, HashMap *map, const char *str, size_t len, char q)
{
    HashKey key = {(unsigned char *)(char *)str, len};
    HashEntry *entry = map_entry(map, key);
    if (!entry->value.ptr)
    {
        entry->value.ptr = really_do_quote(str, len, q);
        pool_free(pool, entry->value.ptr);
    }
    return (const char *)entry->value.ptr;
}

const char *pool_single_quote(Pool *pool, const char *str, size_t len)
{
    return do_quote(pool, pool->single_quoted, str, len, '\'');
}

const char *pool_double_quote(Pool *pool, const char *str, size_t len)
{
    return do_quote(pool, pool->double_quoted, str, len, '"');
}
