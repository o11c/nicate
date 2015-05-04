#pragma once
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

HashIterator *map_first(HashMap *map);
HashIterator *map_next(HashIterator *it);
HashEntry *map_deref(HashIterator *it);
size_t map_size(HashMap *map);
