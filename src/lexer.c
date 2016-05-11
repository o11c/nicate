#include "lexer.h"
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
#include <string.h>

#include "mre.h"
#include "pool.h"


struct Lexicon
{
    char **names;
    size_t num_names;
    MreRuntime *runtime;
};

struct Tokenizer
{
    MreRuntime *runtime;
    char *buffer;
    size_t buffer_start;
    size_t buffer_end;
    size_t buffer_cap;
};


static MreRuntime *build_runtime(size_t num_symbols, Symbol *symbols)
{
    MreRuntime *rv;
    Pool *p = pool_create();
    MultiNfa *m = multi_nfa_create();
    size_t i;
    for (i = 0; i < num_symbols; ++i)
    {
        Nfa *n = nfa_parse_regex(p, symbols[i].regex);
        size_t j = multi_nfa_add(m, n);
        (void)j;
        assert (i + 1 == j);
    }
    rv = mre_runtime_create(m, NULL);
    multi_nfa_destroy(m);
    pool_destroy(p);
    return rv;
}

Lexicon *lexicon_create(size_t num_symbols, Symbol *symbols)
{
    size_t i;
    Lexicon *rv = (Lexicon *)calloc(1, sizeof(*rv));
    rv->names = (char **)calloc(num_symbols + 1, sizeof(char *));
    rv->names[0] = (char *)"error";
    for (i = 1; i <= num_symbols; ++i)
    {
        rv->names[i] = strdup(symbols[i - 1].name);
    }
    rv->num_names = num_symbols + 1;
    rv->runtime = build_runtime(num_symbols, symbols);
    return rv;
}

void lexicon_destroy(Lexicon *lex)
{
    size_t i;
    mre_runtime_destroy(lex->runtime);
    for (i = lex->num_names - 1; i > 0; i--)
    {
        free(lex->names[i]);
    }
    free(lex->names);
    free(lex);
}

const char *lexicon_name(Lexicon *lex, size_t idx)
{
    return lex->names[idx];
}


Tokenizer *tokenizer_create(Lexicon *lex)
{
    Tokenizer *rv = (Tokenizer *)calloc(1, sizeof(*rv));
    rv->runtime = mre_runtime_clone(lex->runtime);
    rv->buffer_cap = 4096;
    rv->buffer = (char *)calloc(rv->buffer_cap, 1);
    rv->buffer_start = 0;
    rv->buffer_end = 0;
    return rv;
}

Tokenizer *tokenizer_clone(Tokenizer *tok)
{
    Tokenizer *rv = (Tokenizer *)calloc(1, sizeof(*rv));
    rv->runtime = mre_runtime_clone(tok->runtime);
    rv->buffer_cap = 4096;
    rv->buffer = (char *)calloc(rv->buffer_cap, 1);
    rv->buffer_start = 0;
    rv->buffer_end = 0;
    return rv;
}

void tokenizer_destroy(Tokenizer *tok)
{
    free(tok->buffer);
    mre_runtime_destroy(tok->runtime);
    free(tok);
}

static void refeed(Tokenizer *tok, size_t from)
{
    /*
        Note: If called from `tokenizer_feed_slice`, it is possible that we
        have not actually fed all the characters before `from`. However, in
        this case, `mre_runtime_hopeful` must have returned false.
    */
    while (from < tok->buffer_end && mre_runtime_hopeful(tok->runtime))
    {
        mre_runtime_step(tok->runtime, tok->buffer[from++]);
    }
}

void tokenizer_feed(Tokenizer *tok, const char *str)
{
    tokenizer_feed_slice(tok, str, strlen(str));
}

static void recap(Tokenizer *tok, size_t len)
{
    if (tok->buffer_start == tok->buffer_end)
    {
        tok->buffer_start = 0;
        tok->buffer_end = 0;
    }
    if (tok->buffer_end + len <= tok->buffer_cap)
    {
        return;
    }
    if (tok->buffer_start)
    {
        if (tok->buffer_end - tok->buffer_start + len < tok->buffer_cap)
        {
            memmove(tok->buffer, tok->buffer + tok->buffer_start, tok->buffer_end - tok->buffer_start);
            tok->buffer_end -= tok->buffer_start;
            tok->buffer_start = 0;
            return;
        }
    }

    {
        char *new_buffer;
        size_t new_cap = tok->buffer_cap;
        size_t min_cap = len + (tok->buffer_end - tok->buffer_start);
        while (min_cap > new_cap)
        {
            new_cap *= 2;
        }
        assert (new_cap > tok->buffer_cap);
        if (!tok->buffer_start)
        {
            new_buffer = (char *)realloc(tok->buffer, new_cap);
        }
        else
        {
            new_buffer = (char *)calloc(new_cap, 1);
            memcpy(new_buffer, tok->buffer + tok->buffer_start, tok->buffer_end - tok->buffer_start);
            tok->buffer_end -= tok->buffer_start;
            tok->buffer_start = 0;
            free(tok->buffer);
        }
        tok->buffer = new_buffer;
        return;
    }
}

void tokenizer_feed_slice(Tokenizer *tok, const char *str, size_t len)
{
    size_t old_buffer_end;
    recap(tok, len);
    memcpy(tok->buffer + tok->buffer_end, str, len);
    old_buffer_end = tok->buffer_end;
    tok->buffer_end += len;
    refeed(tok, old_buffer_end);
}

void tokenizer_feed_char(Tokenizer *tok, char c)
{
    tokenizer_feed_slice(tok, &c, 1);
}

bool tokenizer_ready(Tokenizer *tok)
{
    return !mre_runtime_hopeful(tok->runtime);
}

size_t tokenizer_sym(Tokenizer *tok)
{
    return mre_runtime_match_id(tok->runtime);
}

const char *tokenizer_text_start(Tokenizer *tok)
{
    return tok->buffer + tok->buffer_start;
}

size_t tokenizer_text_len(Tokenizer *tok)
{
    if (tok->buffer_start == tok->buffer_end)
    {
        return 0;
    }
    return mre_runtime_match_len(tok->runtime);
}

void tokenizer_pop(Tokenizer *tok)
{
    tok->buffer_start += tokenizer_text_len(tok);
    /*
        Could jiggle the buffer here, but we probably *don't* want to in
        the case where they feed us the whole buffer up front, and if being
        fed incrementally, they will call `tokenize_feed_slice` soon.
    */
    mre_runtime_reset(tok->runtime);
    refeed(tok, tok->buffer_start);
}

void tokenizer_reset(Tokenizer *tok)
{
    tok->buffer_start = 0;
    tok->buffer_end = 0;
    mre_runtime_reset(tok->runtime);
}
