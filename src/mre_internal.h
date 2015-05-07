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


typedef struct MreState MreState;
typedef struct MreRules MreRules;

/*
    It is illegal to construct a state table:
      - that accepts the empty string, or
      - that does not accept anything.
*/
struct MreState
{
    size_t accept;
    unsigned char first_goto, last_goto;
    size_t *some_gotos;
};

struct MreRules
{
    size_t refcount;
    /* TODO put character-class tables here. */
    size_t num_states;
    MreState *states;
};


Nfa *nfa_class_set(Pool *pool, CharBitSet *cbs);

MreRules *multi_nfa_to_dfa(MultiNfa *m);
