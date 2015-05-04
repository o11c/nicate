#include "mre.h"
#include "mre_internal.h"
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

#include <assert.h>
#include <stdlib.h>


struct MreRuntime
{
    MreRules *states;
    size_t cur_state;
    size_t cur_len;
    size_t last_match;
    size_t match_len;
};


MreRuntime *mre_runtime_create(MultiNfa *m, void *unused)
{
    MreRuntime *rv = (MreRuntime *)calloc(1, sizeof(*rv));
    (void)unused;
    rv->states = multi_nfa_to_dfa(m);
    mre_runtime_reset(rv);
    return rv;
}
MreRuntime *mre_runtime_clone(MreRuntime *old)
{
    MreRuntime *rv = (MreRuntime *)calloc(1, sizeof(*rv));
    *rv = *old;
    rv->states->refcount++;
    return rv;
}
void mre_runtime_reset(MreRuntime *run)
{
    run->cur_state = 1;
    run->cur_len = 0;
    run->last_match = 0;
    run->match_len = 1;
}
static void free_rules(MreRules *rul)
{
    if (!--rul->refcount)
    {
        free(rul->states);
        free(rul);
    }
}
void mre_runtime_destroy(MreRuntime *run)
{
    free_rules(run->states);
    free(run);
}

void mre_runtime_step(MreRuntime *run, char c)
{
    size_t ci = (unsigned char)c;
    size_t si = run->states->states[run->cur_state].gotos[ci];
    size_t sa = run->states->states[si].accept;
    run->cur_state = si;
    run->cur_len++;
    if (sa)
    {
        run->last_match = sa;
        run->match_len = run->cur_len;
    }
}
bool mre_runtime_hopeful(MreRuntime *run)
{
    return run->cur_state != 0;
}
size_t mre_runtime_match_id(MreRuntime *run)
{
    return run->last_match;
}
size_t mre_runtime_match_len(MreRuntime *run)
{
    return run->match_len;
}
