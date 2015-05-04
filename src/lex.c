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
#include <stdio.h>
#include <string.h>

#include "mre.h"
#include "pool.h"


const char *names[] =
{
    "error",
    "a",
    "b",
};

static MreRuntime *build()
{
    MreRuntime *rv;
    MultiNfa *m = multi_nfa_create();
    Pool *p = pool_create();

    Nfa *tok_a = nfa_char(p, 'a');
    size_t id_a = multi_nfa_add(m, tok_a);
    Nfa *tok_b = nfa_char(p, 'b');
    size_t id_b = multi_nfa_add(m, tok_b);

    (void)id_a;
    (void)id_b;

    rv = mre_runtime_create(m, NULL);
    multi_nfa_destroy(m);
    return rv;
}

static void play(MreRuntime *r)
{
    size_t fed = 0;
    size_t size = 0;
    char buffer[80];
    int rv;

    while (true)
    {
        if (size == sizeof(buffer))
        {
            puts("buffer size exceeded");
            return;
        }
        if (fed == size)
        {
            rv = getchar();
            if (rv == EOF)
            {
                size_t match = mre_runtime_match_id(r);
                puts(names[match]);
                return;
            }
            buffer[size++] = rv;
        }
        mre_runtime_step(r, buffer[fed++]);
        if (mre_runtime_hopeful(r))
        {
            continue;
        }
        {
            size_t match = mre_runtime_match_id(r);
            size_t len = mre_runtime_match_len(r);
            puts(names[match]);
            size -= len;
            fed = 0;
            memmove(buffer, buffer + len, size);
            mre_runtime_reset(r);
        }
    }
}

int main()
{
    MreRuntime *r = build();
    play(r);
    mre_runtime_destroy(r);

    return 0;
}
