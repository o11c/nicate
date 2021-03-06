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


/*
    `nothing` occurs at eof, as well as in any rules that expand to
    no tokens.
*/
struct Tree
{
    /*
        0 for nothing
        [1, num_terms] for terminals
        [num_terms + 1, num_terms + num_nonterms] for nonterminals.
    */
    size_t type;
    /*
        0 for terminals and nothing
        >=1 for nonterminals
    */
    size_t num_children;
    __extension__ union
    {
        /*
            these are valid only if num_children == 0
        */
        __extension__ struct
        {
            size_t token_length;
            char *token;
        };
        /*
            these are valid only if num_children > 0
        */
        __extension__ struct
        {
            size_t rule;
            Tree *children;
        };
    };
};


struct Grammar
{
    size_t num_symbols;
    size_t num_nonterminals;
    size_t num_rules;
    Rule *rules;
    size_t *rules_by_nonterminal;
};


struct Rule
{
    size_t lhs;
    size_t num_rhses;
    size_t *rhses;
};


size_t automaton_tree_count(Automaton *a);
