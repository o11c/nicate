#pragma once

#include <stddef.h>

#include "fwd.h"


struct HashKey
{
    unsigned char *data;
    size_t len;
};

struct HashValue
{
    void *ptr;
};

struct HashEntry
{
    HashKey key;
    HashValue value;
};

HashMap *map_create(void);
void map_destroy(HashMap *map);
HashEntry *map_entry(HashMap *map, HashKey key);
