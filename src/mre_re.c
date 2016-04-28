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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitset.h"


/*
    TODO implement this using nicate itself as a lexer/parser.

    REGULAR EXPRESSION SYNTAX:

    a       itself
    _       itself
    \n      special escape
    \\      quote escape
    ''      quote everything except '
    ""      quote everything except " and escapes
    []      class
    .       any latin1 character
    a|b     alternation
    a?      optional
    a+      repetition
    a*      optional repetition
    a{1}    fixed repetition
    a{1,2}  range repetiion
    a{,2}   as a{0,2}
    a{2,}   as a{2}a*
    (a)     grouping

    REGULAR EXPRESSION GRAMMAR:
    regexes must be entirely ascii printable.

    oct: any octal digit
    dec1: any decimal digit except 0
    dec: any decimal digit
    hex: any hexadecimal digit
    control: any character between 0x40 (@) and 0x5f (_)
    alnum: any uppercase or lowercase letter, or number
    any-sym: anything except alnum
    safe-sym: any symbol that is not used or reserved
    class-char: any character except \, -, ^, or ]
    sq-char: any character except '
    dq-char: any character except \ or "

    regex:
        cat
        cat | regex
    cat:
        empty
        repeat cat
    repeat:
        atom
        atom ?
        atom *
        atom +
        atom { int }
        atom { int_opt , int_opt }
    int:
        0
        dec1 dec_opt
    atom:
        alnum
        safe-sym
        single-quote
        double-quote
        escape
        group
        class
    single-quote:
        ' sq-chars_opt '
    double-quote:
        " dq-chars_opt "
    sq-chars:
        sq-chars_opt sq-char
    dq-chars:
        dq-chars_opt dq-char-or-escape
    dq-char-or-escape:
        dq-char
        escape
    escape:
        \ any-sym
        \ a
        \ b
        \ e
        \ f
        \ n
        \ r
        \ t
        \ v
        \ oct
        \ oct oct
        \ oct oct oct
        \ c ?
        \ c control
        \ x hex hex
    group:
        ( regex )
    class:
        [ ^_opt ] class-chars_opt -_opt ]
        [ ^_opt - class-chars_opt ]
        [ ^ class-chars -_opt ]
        [ escape range_opt class-chars_opt -_opt ]
        [ class-char range_opt class-chars_opt -_opt ]
    class-chars:
        class-chars_opt any-class-char range_opt
    any-class-char:
        ^
        class-char
        escape
    range:
        - any-class-char
        - -
*/

static bool class_oct(unsigned char c)
{
    return '0' <= c && c <= '7';
}

static bool class_hex(unsigned char c)
{
    return ('0' <= c && c <= '9') || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f');
}

static bool class_digit(unsigned char c)
{
    return '0' <= c && c <= '9';
}

static bool class_upper(unsigned char c)
{
    return 'A' <= c && c <= 'Z';
}

static bool class_lower(unsigned char c)
{
    return 'a' <= c && c <= 'z';
}

static bool class_alpha(unsigned char c)
{
    return class_upper(c) || class_lower(c);
}

static bool class_alnum(unsigned char c)
{
    return class_digit(c) || class_alpha(c);
}

static bool class_ascii(unsigned char c)
{
    return 0x20 <= c && c <= 0x7e;
}

static bool class_sym(unsigned char c)
{
    return class_ascii(c) && !class_alnum(c);
}

static bool class_safe_sym(unsigned char c)
{
    if (!class_ascii(c))
        return false;
    switch (c)
    {
    case ' ':
        return false;
    case '!':
        return true;
    case '"':
        return false;
    case '#':
        return false;
    case '$':
        return false;
    case '%':
        return true;
    case '&':
        return true;
    case '\'':
        return false;
    case '(':
        return false;
    case ')':
        return false;
    case '*':
        return false;
    case '+':
        return false;
    case ',':
        return true;
    case '-':
        return true;
    case '.':
        return false;
    case '/':
        return false;
    case ':':
        return true;
    case ';':
        return true;
    case '<':
        return true;
    case '=':
        return true;
    case '>':
        return true;
    case '?':
        return false;
    case '@':
        return true;
    case '[':
        return false;
    case '\\':
        return false;
    case ']':
        return false;
    case '^':
        return false;
    case '_':
        return true;
    case '`':
        return true;
    case '{':
        return false;
    case '|':
        return false;
    case '}':
        return false;
    case '~':
        return true;
    }
    abort();
}

static bool class_atom(unsigned char c)
{
    return class_alnum(c) || class_safe_sym(c);
}

static bool class_class(unsigned char c)
{
    switch (c)
    {
    case '\\':
    case '-':
    case '^':
    case ']':
        return false;
    default:
        return class_ascii(c);
    }
}


__attribute__((noreturn))
static void fail(const char *msg)
{
    fprintf(stderr, "fatal nicate regex error: %s\n", msg);
    abort();
}

typedef struct Result Result;
struct Result
{
    Nfa *nfa;
    const char *limit;
    bool not_empty;
};

typedef struct IntResult IntResult;
struct IntResult
{
    int value;
    const char *limit;
};

typedef struct CharResult CharResult;
struct CharResult
{
    char ch;
    const char *limit;
};

static Nfa *do_fixed_repeat(Pool *pool, Nfa *nfa, int count)
{
    Nfa *rv = nfa;
    assert (count > 0);

    while (count-- > 1)
    {
        rv = nfa_cat(pool, rv, nfa);
    }
    return rv;
}

static Nfa *do_opt_repeat(Pool *pool, Nfa *nfa, int max)
{
    Nfa *rv = nfa_opt(pool, nfa);
    assert (max > 0);

    while (max-- > 1)
    {
        rv = nfa_opt(pool, nfa_cat(pool, nfa, rv));
    }
    return rv;
}

static Nfa *do_repeat(Pool *pool, Nfa *nfa, int min, int max)
{
    if (min < 0 && max < 0)
    {
        fail("must specify min or max");
    }
    if (min < 0)
    {
        min = 0;
    }
    if (max == 0)
    {
        fail("max must be at least 1");
    }
    if (max < 0)
    {
        Nfa *tail = nfa_star(pool, nfa);
        if (min == 0)
        {
            return tail;
        }

        {
            Nfa *head = do_fixed_repeat(pool, nfa, min);
            return nfa_cat(pool, head, tail);
        }
    }
    if (max < min)
    {
        fail("max must be at least min");
    }

    if (min == 0)
    {
        return do_opt_repeat(pool, nfa, max);
    }

    if (min == max)
    {
        return do_fixed_repeat(pool, nfa, min);
    }

    {
        Nfa *head = do_fixed_repeat(pool, nfa, min);
        Nfa *tail = do_opt_repeat(pool, nfa, max - min);
        return nfa_cat(pool, head, tail);
    }
}

static IntResult parse_opt_int(const char *re, const char *re_end)
{
    if (re != re_end && class_digit(*re))
    {
        if (*re == '0')
        {
            if (re + 1 != re_end)
            {
                if (class_digit(re[1]))
                {
                    fail("there is no octal");
                }
                if (re[1] == 'x' || re[1] == 'X')
                {
                    fail("there is no hexadecimal");
                }
            }

            {
                IntResult rv = {0, re + 1};
                return rv;
            }
        }

        {
            int len = 1;
            int val = *re - '0';
            while (re + len != re_end && class_digit(re[len]))
            {
                val = val * 10 + (re[len] - '0');
                ++len;
                if (val > 99)
                {
                    fail("don't repeat that much please");
                }
            }

            {
                IntResult rv = {val, re + len};
                return rv;
            }
        }
    }

    {
        IntResult rv = {-1, re};
        return rv;
    }
}

static CharResult char_after_bs(const char *re, const char *re_end)
{
    unsigned char c;
    size_t len;
    assert (re != re_end && *re == '\\');
    if (re + 1 == re_end)
    {
        fail("incomplete escape");
    }

    len = 2;
    if (class_sym(re[1]))
    {
        c = re[1];
    }
    else if (class_oct(re[1]))
    {
        if (re + 2 != re_end && class_oct(re[2]))
        {
            if (re + 3 != re_end && class_oct(re[3]))
            {
                if (re[1] & 4)
                {
                    fail("octal constant out of range");
                }
                len = 4;
                c = ((re[1] - '0') << 6) | ((re[2] - '0') << 3) | ((re[3] - '0') << 0);
            }
            else
            {
                len = 3;
                c = ((re[1] - '0') << 3) | ((re[2] - '0') << 0);
            }
        }
        else
        {
            c = ((re[1] - '0') << 0);
        }
    }
    else if (re[1] == 'c')
    {
        if (re + 2 == re_end)
        {
            fail("short control escape");
        }
        len = 3;
        if (re[2] == '?')
        {
            c = '\x7f';
        }
        else if (0x40 <= re[2] && re[2] < 0x60)
        {
            c = re[2] & 0x1f;
        }
        else
        {
            fail("bad char escape");
        }
    }
    else if (re[1] == 'x')
    {
        if (re + 2 == re_end || re + 3 == re_end)
        {
            fail("short hex escape");
        }
        if (!class_hex(re[2]) || !class_hex(re[3]))
        {
            fail("bad hex escape");
        }
        c = ((re[2] & 0xf) + (re[2] & 0x10 ? 0 : 9)) << 4;
        c |= ((re[3] & 0xf) + (re[3] & 0x10 ? 0 : 9)) << 0;
        len = 4;
    }
    else
    {
        switch (re[1])
        {
        case 'a':
            c = '\a';
            break;
        case 'b':
            c = '\b';
            break;
        case 'e':
            c = '\x1b';
            break;
        case 'f':
            c = '\f';
            break;
        case 'n':
            c = '\n';
            break;
        case 'r':
            c = '\r';
            break;
        case 't':
            c = '\t';
            break;
        case 'v':
            c = '\v';
            break;
        default:
            fail("unknown escape");
        }
    }

    {
        CharResult cr = {c, re + len};
        return cr;
    }
}
static Result parse_after_bs(Pool *pool, const char *re, const char *re_end)
{
    CharResult cr = char_after_bs(re, re_end);
    Nfa *nfa = nfa_char(pool, cr.ch);
    Result rv = {nfa, cr.limit, true};
    return rv;
}

static Result parse_regex(Pool *pool, const char *re, const char *re_end);

static Result parse_atom(Pool *pool, const char *re, const char *re_end)
{
    unsigned char c;
    assert (re != re_end);
    c = *re;
    if (!class_ascii(c))
    {
        fail("non-ascii character in regex");
    }
    if (class_atom(c))
    {
        Nfa *nfa = nfa_char(pool, c);
        Result rv = {nfa, re + 1, true};
        return rv;
    }
    switch (c)
    {
    case '\\':
        return parse_after_bs(pool, re, re_end);
    case '.':
        {
            Nfa *nfa = nfa_any_char(pool);
            Result rv = {nfa, re + 1, true};
            return rv;
        }
        break;
    case '\'':
        {
            const char *end = (const char *)memchr(re + 1, '\'', re_end - (re + 1));
            if (!end)
            {
                fail("missing `'`");
            }

            {
                Nfa *nfa = nfa_text_slice(pool, re + 1, end - (re + 1));
                Result rv = {nfa, end + 1, end != re + 1};
                return rv;
            }
        }
    case '"':
        {
            bool got_anything = false;
            Nfa *nfa = nfa_epsilon(pool);
            const char *wip = re + 1;
            const char *maybe_end = NULL;
            while (true)
            {
                const char *bs;
                if (!maybe_end)
                {
                    maybe_end = (const char *)memchr(wip, '"', re_end - wip);
                    if (!maybe_end)
                    {
                        fail("missing `\"`");
                    }
                }
                bs = (const char *)memchr(wip, '\\', maybe_end - wip);
                if (!bs)
                {
                    Nfa *tmp = nfa_text_slice(pool, wip, maybe_end - wip);
                    nfa = nfa_cat(pool, nfa, tmp);
                    got_anything |= wip != maybe_end;
                    {
                        Result rv = {nfa, maybe_end + 1, got_anything};
                        return rv;
                    }
                }
                if (maybe_end == bs + 1)
                {
                    maybe_end = NULL;
                }
                got_anything = true;
                {
                    Nfa *tmp = nfa_text_slice(pool, wip, bs - wip);
                    nfa = nfa_cat(pool, nfa, tmp);
                }
                {
                    Result bs_rv = parse_after_bs(pool, bs, re_end);
                    nfa = nfa_cat(pool, nfa, bs_rv.nfa);
                    wip = bs_rv.limit;
                }
            }
        }
    case '(':
        {
            Result rv = parse_regex(pool, re + 1, re_end);
            if (rv.limit == re_end || rv.limit[0] != ')')
            {
                fail("missing )");
            }
            rv.limit++;
            return rv;
        }
        break;
    case '[':
        {
            bool invert = false;
            const char *wip = re + 1;
            CharBitSet cbs;
            char_bitset_erase(&cbs);
            if (wip != re_end && *wip == '^')
            {
                invert = true;
                ++wip;
            }
            if (wip != re_end)
            {
                if (*wip == ']' || *wip == '-')
                {
                    char_bitset_set(&cbs, *wip);
                    ++wip;
                }
            }
            while (true)
            {
                unsigned char c;
                if (wip == re_end)
                {
                    fail("missing ]");
                }
                if (*wip == ']')
                {
                    if (invert)
                    {
                        char_bitset_invert(&cbs);
                    }

                    {
                        Nfa *nfa = nfa_class_set(pool, &cbs);
                        Result rv = {nfa, wip + 1, true};
                        return rv;
                    }
                }
                if (*wip == '\\')
                {
                    CharResult cr = char_after_bs(wip, re_end);
                    c = cr.ch;
                    wip = cr.limit;
                }
                else if ((*wip == '-' && wip + 1 != re_end && wip[1] == ']') || (*wip == '^' || class_class(*wip)))
                {
                    c = *wip++;
                }
                else
                {
                    fail("unexpected char in class");
                }

                /* Always add, even if it's the start of a range. */
                char_bitset_set(&cbs, c);
                if (wip != re_end && *wip == '-')
                {
                    if (wip + 1 == re_end)
                    {
                        fail("incomplet range");
                    }
                    if (wip[1] != ']')
                    {
                        unsigned char d;
                        ++wip;
                        if (*wip == '\\')
                        {
                            CharResult cr = char_after_bs(wip, re_end);
                            d = cr.ch;
                            wip = cr.limit;
                        }
                        else if (*wip == '-' || *wip == '^' || class_class(*wip))
                        {
                            d = *wip++;
                        }
                        else
                        {
                            fail("unexpected char at end of range in class");
                        }
                        if (c > d)
                        {
                            fail("backwards range");
                        }
                        for (; c++ != d;)
                        {
                            char_bitset_set(&cbs, c);
                        }
                    }
                }
            }
        }
    default:
        fail("unknown bad character");
    case ' ':
        fail("unquoted space not allowed");
    case '#':
        fail("unquoted #");
    case '$':
        fail("unsupported $");
    case ')':
        abort();
    case '*':
        fail("unexpected repeat *");
    case '+':
        fail("unexpected repeat +");
    case '/':
        fail("unquoted /");
    case '?':
        fail("unexpected repeat ?");
    case ']':
        fail("unquoted ]");
    case '^':
        fail("unsupported ^");
    case '{':
        fail("unexpected repeat {");
    case '|':
        abort();
    case '}':
        fail("unquoted }");
    }
}

static Result parse_repeat(Pool *pool, const char *re, const char *re_end)
{
    Result rv1 = parse_atom(pool, re, re_end);
    if (rv1.limit != re_end)
    {
        switch (*rv1.limit)
        {
        case '?':
            {
                Result rv = {nfa_opt(pool, rv1.nfa), rv1.limit + 1, false};
                return rv;
            }
        case '*':
            {
                Result rv = {nfa_star(pool, rv1.nfa), rv1.limit + 1, false};
                return rv;
            }
        case '+':
            {
                Result rv = {nfa_plus(pool, rv1.nfa), rv1.limit + 1, rv1.not_empty};
                return rv;
            }
        case '{':
            {
                IntResult rp1 = parse_opt_int(rv1.limit + 1, re_end);
                if (rp1.limit != re_end)
                {
                    if (*rp1.limit == '}')
                    {
                        Nfa *nfa = do_repeat(pool, rv1.nfa, rp1.value, rp1.value);
                        Result rv = {nfa, rp1.limit + 1, rp1.value > 0};
                        return rv;
                    }
                    else if (*rp1.limit == ',')
                    {
                        IntResult rp2 = parse_opt_int(rp1.limit + 1, re_end);
                        if (rp2.limit != re_end && *rp2.limit == '}')
                        {
                            Nfa *nfa = do_repeat(pool, rv1.nfa, rp1.value, rp2.value);
                            Result rv = {nfa, rp2.limit + 1, rv1.not_empty && rp1.value > 0};
                            return rv;
                        }
                    }
                }
            }
            fail("missing '}' after repeat count");
        }
    }
    return rv1;
}

static Result parse_cat(Pool *pool, const char *re, const char *re_end)
{
    if (re == re_end || *re == '|' || *re == ')')
    {
        Result rv = {nfa_epsilon(pool), re, false};
        return rv;
    }
    else
    {
        Result rv1 = parse_repeat(pool, re, re_end);
        Result rv2 = parse_cat(pool, rv1.limit, re_end);
        Result rv = {nfa_cat(pool, rv1.nfa, rv2.nfa), rv2.limit, rv1.not_empty || rv2.not_empty};
        return rv;
    }
}

static Result parse_regex(Pool *pool, const char *re, const char *re_end)
{
    Result rv1 = parse_cat(pool, re, re_end);
    if (rv1.limit != re_end && *rv1.limit == '|')
    {
        Result rv2 = parse_regex(pool, rv1.limit + 1, re_end);
        Result rv = {nfa_alt(pool, rv1.nfa, rv2.nfa), rv2.limit, rv1.not_empty && rv2.not_empty};
        return rv;
    }
    else
    {
        return rv1;
    }
}

Nfa *nfa_parse_regex(Pool *pool, const char *re)
{
    if (re == NULL)
    {
        return nfa_fail(pool);
    }
    return nfa_parse_regex_slice(pool, re, strlen(re));
}

Nfa *nfa_parse_regex_slice(Pool *pool, const char *re, size_t re_len)
{
    Result rv = parse_regex(pool, re, re + re_len);
    assert (rv.not_empty);
    if (rv.limit != re + re_len)
    {
        fail("unexpected char in regex");
    }
    return rv.nfa;
}

