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


enum ActionType
{
    SHIFT,
    GOTO = SHIFT,
    REDUCE,
    ERROR,
    ACCEPT,
};
typedef enum ActionType ActionType;
struct Action
{
    size_t type;
    size_t value;
};


Rule *rule_create(size_t lhs, size_t num_rhses, size_t *rhses);

Grammar *grammar_create(size_t num_symbols, size_t num_nonterminals, size_t num_rules, Rule **rules);
void grammar_destroy(Grammar *g);

State *state_create(Grammar *g, Action def, Action *term_acts, Action *nonterm_acts);

Automaton *automaton_create(Grammar *g, size_t num_states, State **states);
Automaton *automaton_create_auto(Grammar *g);
void automaton_destroy(Automaton *a);
void automaton_reset(Automaton *a);
Automaton *automaton_clone(Automaton *a);

bool automaton_feed_term(Automaton *a, size_t sym, const char *str, size_t len);
Tree *automaton_result(Automaton *a);
