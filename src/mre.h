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

#include <stdbool.h>
#include <stddef.h>

#include "fwd.h"


Nfa *nfa_fail(Pool *pool);
Nfa *nfa_epsilon(Pool *pool);
Nfa *nfa_char(Pool *pool, char c);
Nfa *nfa_any_char(Pool *pool);
Nfa *nfa_text(Pool *pool, const char *str);
Nfa *nfa_class(Pool *pool, const char *str);
Nfa *nfa_class_compl(Pool *pool, const char *str);
Nfa *nfa_text_slice(Pool *pool, const char *str, size_t len);
Nfa *nfa_class_slice(Pool *pool, const char *str, size_t len);
Nfa *nfa_class_compl_slice(Pool *pool, const char *str, size_t len);
Nfa *nfa_alt(Pool *pool, Nfa *a, Nfa *b);
Nfa *nfa_cat(Pool *pool, Nfa *left, Nfa *right);
Nfa *nfa_opt(Pool *pool, Nfa *inner);
Nfa *nfa_star(Pool *pool, Nfa *inner);
Nfa *nfa_plus(Pool *pool, Nfa *inner);
/* There is no public delete function; the pool takes care of that. */


/*
    Usage:

    First call `multi_nfa_create` to create a set of rules.

    Then repeatedly call `multi_nfa_add` with an NFA constructed
    using the above functions. Once added here, the NFA can safely be
    destroyed (i.e. by destroying the Pool).

    Remember the return value of `multi_nfa_add` so you know which regex
    is matching. The return value will be a unique small integer but not 0.
*/
MultiNfa *multi_nfa_create(void);
void multi_nfa_destroy(MultiNfa *m);
size_t multi_nfa_add(MultiNfa *m, Nfa *nfa);

/*
    Usage:

    First call `mre_runtime_create`. After this, the `MultiNfa` object
    may be destroyed.

    Then repeatedly call `mre_runtime_step` with each character of input,
    until `mre_runtime_hopeful` returns false or you run out of input.

    Use `mre_runtime_match_len` to determine how much of the input was
    accepted, and `mre_runtime_match_id` to determine which rule matched.
    Both functions will return 0 if (and only if) nothing matched.

    Finally, call `mre_runtime_reset` to prepare the state machine for the
    next token.
*/
MreRuntime *mre_runtime_create(MultiNfa *m, void *future_extension_for_now_just_pass_null);
MreRuntime *mre_runtime_clone(MreRuntime *old);
void mre_runtime_reset(MreRuntime *run);
void mre_runtime_destroy(MreRuntime *run);

void mre_runtime_step(MreRuntime *run, char c);
bool mre_runtime_hopeful(MreRuntime *run);
size_t mre_runtime_match_id(MreRuntime *run);
size_t mre_runtime_match_len(MreRuntime *run);
