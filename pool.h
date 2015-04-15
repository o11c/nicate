#pragma once

#include <stddef.h>

#include "fwd.h"


Pool *pool_create(void);
void pool_destroy(Pool *pool);
const void *pool_intern(Pool *pool, const void *str, size_t len);
const char *pool_intern_string(Pool *pool, const char *str);
const char *pool_single_quote(Pool *pool, const char *str, size_t len);
const char *pool_double_quote(Pool *pool, const char *str, size_t len);
