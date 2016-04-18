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


struct Symbol
{
    const char *name;
    const char *regex;
};

Lexicon *lexicon_create(size_t num_symbols, Symbol *symbols);
void lexicon_destroy(Lexicon *lex);
const char *lexicon_name(Lexicon *lex, size_t idx);

Tokenizer *tokenizer_create(Lexicon *lex);
void tokenizer_destroy(Tokenizer *tok);
void tokenizer_feed(Tokenizer *tok, const char *str);
void tokenizer_feed_slice(Tokenizer *tok, const char *str, size_t len);
void tokenizer_feed_char(Tokenizer *tok, char c);
bool tokenizer_ready(Tokenizer *tok);
size_t tokenizer_sym(Tokenizer *tok);
const char *tokenizer_text_start(Tokenizer *tok);
size_t tokenizer_text_len(Tokenizer *tok);
void tokenizer_pop(Tokenizer *tok);
void tokenizer_reset(Tokenizer *tok);
