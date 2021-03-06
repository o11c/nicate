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


typedef void *(*PoolTransform)(Pool *pool, const void *str, size_t len, void *context);
typedef void (*PoolFree)(void *);

Pool *pool_create(void);
void pool_destroy(Pool *pool);
void pool_free(Pool *pool, PoolFree free_func, void *ptr);
const void *pool_intern(Pool *pool, const void *str, size_t len);
const char *pool_intern_string(Pool *pool, const char *str);
const void *pool_intern_map(Pool *pool, PoolTransform transform, const void *str, size_t len, void *context);
