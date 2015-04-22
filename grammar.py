#!/usr/bin/env python3
#   Copyright © 2015 Ben Longbons
#
#   This file is part of Nicate.
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Lesser General Public License as published
#   by the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.


import string

generated_copyright = '''
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
'''

class GrammarError(Exception):
    pass

class IdentifierCase:
    __slots__ = ('space', 'dash', 'lower', 'upper', 'camel', 'visual')

    def __init__(self, name, visual=None):
        for bit in name.split('-'):
            bit = bit.rstrip(string.digits)
            assert bit and all(c.islower() for c in bit), name
        self.dash = name
        space = self.space = name.replace('-', ' ')
        lower = self.lower = name.replace('-', '_')
        self.upper = lower.upper()
        self.camel = ''.join([x + ('_' if x[-1].isdigit() else '') for x in space.title().split()])
        self.visual = visual or name

    def __repr__(self):
        name = self.dash
        if self.visual != name:
            name = '%r, %r' % (name, self.visual)
        else:
            name = repr(name)
        return 'IdentifierCase(%s, <space=%r, lower=%r, upper=%r, camel=%r>)' % (name, self.space, self.lower, self.upper, self.camel)

    def __add__(self, other):
        self_dash = self.dash
        self_visual = self.visual
        if isinstance(other, str):
            other_dash = other
            other_visual = other
        else:
            other_dash = other.dash
            other_visual = other.visual
        return IdentifierCase('%s-%s' % (self_dash, other_dash), '%s-%s' % (self_visual, other_visual))

    def __radd__(self, other):
        self_dash = self.dash
        self_visual = self.visual
        if isinstance(other, str):
            other_dash = other
            other_visual = other
        else:
            other_dash = other.dash
            other_visual = other.visual
        return IdentifierCase('%s-%s' % (other_dash, self_dash), '%s-%s' % (other_visual, self_visual))

class Grammar:
    __slots__ = ('language', 'rules')

    def __init__(self, src):
        self.language = None
        self.rules = {}

        cur_name = None
        cur_rule = None

        for line in src:
            line = line.strip()
            if line.startswith('#') or not line:
                continue
            words = line.split()
            if words[0] == 'language':
                if self.language:
                    raise GrammarError('language specified twice')
                if len(words) != 2:
                    raise GrammarError('language takes 1 argument')
                self.language = IdentifierCase(words[1])
                continue
            if not self.language:
                raise GrammarError('language not specified')
            if cur_rule is None:
                if words[0] == 'keyword':
                    if len(words) != 2:
                        raise GrammarError('keyword takes 1 argument')
                    self.add_keyword(words[1])
                    continue
                elif words[0] == 'atom':
                    if len(words) != 2:
                        raise GrammarError('atom takes 1 argument')
                    self.add_atom(words[1])
                    continue
                elif words[0] == 'symbol':
                    if len(words) != 3:
                        raise GrammarError('symbol takes 2 argument')
                    self.add_symbol(words[1], words[2])
                    continue
            if line.endswith(':') and len(line) != 1 and len(words) == 1:
                if cur_rule is not None:
                    self.add_rule(cur_name, cur_rule)
                cur_name = line[:-1]
                cur_rule = []
            else:
                cur_rule.append(words)
        if cur_rule is None:
            raise GrammarError('no rules')
        self.add_rule(cur_name, cur_rule)

        for v in self.rules.values():
            if isinstance(v, (Alternative, Sequence)):
                for i, b in enumerate(v.bits):
                    if isinstance(b, IdentifierCase):
                        v.bits[i] = self.rules[b.visual]
            elif isinstance(v, Option):
                if isinstance(v.child, IdentifierCase):
                    v.child = self.rules[v.child.visual]

    def add_keyword(self, ty):
        self.rules[ty] = Term(IdentifierCase('kw-' + ty, ty), False)

    def add_atom(self, ty):
        self.rules[ty] = Term(IdentifierCase('atom-' + ty, ty), True)

    def add_symbol(self, sym, ty):
        self.rules[sym] = Term(IdentifierCase('sym-' + ty, sym), False)

    def add_rule(self, name, alts):
        if len(alts) == 0:
            raise GrammarError('empty rule')
        elif len(alts) == 1:
            if len(alts[0]) == 1:
                raise GrammarError('rules without alternative must have 2 or more in sequence')
            self.rules[name] = Sequence(name, [self.get_rule(a) for a in alts[0]])
        else:
            for a in alts:
                if len(a) != 1:
                    raise GrammarError('rule with alternative must not have sequences')
            self.rules[name] = Alternative(name, [self.get_rule(a) for a, in alts])

    def get_rule(self, vis):
        if vis.endswith('_opt'):
            return self.get_opt(vis)
        return self.rules.get(vis) or IdentifierCase(vis)

    def get_opt(self, vis):
        rv = self.rules.get(vis)
        if not rv:
            base = vis[:-4]
            maybe_tok = self.rules.get(base)
            if isinstance(maybe_tok, Term):
                base2 = maybe_tok.tag.dash.split('-', 1)[1]
            else:
                base2 = base
            rv = self.rules[vis] = Option(IdentifierCase('opt-' + base2, vis), self.get_rule(base))
        return rv

class Rule:
    __slot__ = ()

    def __repr__(self):
        # note: do *not* include `bits` here, because that would recurse
        return '%s(%r)' % (self.__class__.__name__, self.tag)

class Term(Rule):
    __slots__ = ('tag', 'is_atom')
    def __init__(self, tag, is_atom):
        self.tag = tag
        self.is_atom = is_atom

class Option(Rule):
    __slots__ = ('tag', 'child')
    def __init__(self, tag, child):
        self.tag = tag
        self.child = child

class Sequence(Rule):
    __slots__ = ('tag', 'bits')
    def __init__(self, tag, bits):
        self.tag = IdentifierCase('tree-' + tag, tag)
        self.bits = list(bits)

class Alternative(Rule):
    __slots__ = ('tag', 'bits')
    def __init__(self, tag, bits):
        self.tag = IdentifierCase('any-' + tag, tag)
        self.bits = list(bits)

class FormatArgs:
    pass

def emit(grammar, header, source):
    f = FormatArgs()
    def h(arg=''):
        arg = arg % f.__dict__
        print(arg, file=header)
    def c(arg=''):
        arg = arg % f.__dict__
        print(arg, file=source)
    def s(sig):
        h(sig + ';')
        c(sig)

    ast_values = sorted(grammar.rules.values(), key=lambda x: x.tag.space)
    lang = grammar.language
    nothing = lang + 'nothing'

    f.dash = lang.dash
    f.upper = lang.upper
    f.Ast = (lang + 'ast').camel
    f.AstType = (lang + 'ast-type').camel
    f.Nothing = nothing.camel
    f.nothing = nothing.lower
    f.NOTHING = nothing.upper
    f.create_nothing = (lang + 'create-nothing').lower
    f.create_terminal = (lang + 'create-terminal').lower
    f.create_nonterminal = (lang + 'create-nonterminal').lower
    f.is_nothing = (lang + 'is-nothing').lower
    f.emit_impl_impl = (lang + 'emit-impl-impl').lower
    f.emit_impl = (lang + 'emit-impl').lower
    f.emit = (lang + 'emit').lower

    h('/* Generated file, edit %(dash)s.gram instead. */')
    h('#pragma once')
    h(generated_copyright)
    h()
    c('/* Generated file, edit %(dash)s.gram instead. */')
    c('#include "%(dash)s.gen.h"')
    c(generated_copyright)
    c()
    c('#include <assert.h>')
    h('#include <stdbool.h>')
    c('#include <stddef.h>')
    h('#include <stdio.h>')
    c('#include <string.h>')
    h()
    c()
    h('#include "fwd.h"')
    c('#include "pool.h"')
    h()
    h()
    c()
    c()
    h('typedef struct %(Ast)s %(Ast)s;')
    h()
    h('typedef struct %(Nothing)s %(Nothing)s;')
    for ast in ast_values:
        h('typedef struct {0} {0};'.format((lang + ast.tag).camel))
    h()
    c('/* Concrete types only. */')
    c('typedef enum %(AstType)s %(AstType)s;')
    c('enum %(AstType)s')
    c('{')
    c('    %(NOTHING)s,')
    for ast in ast_values:
        if isinstance(ast, (Term, Sequence)):
            c('    {0},'.format((lang + ast.tag).upper))
    c('};')
    c()
    c('struct %(Ast)s')
    c('{')
    c('    %(AstType)s type;')
    c('    size_t total_tokens;')
    c('    size_t num_children;')
    c('    union { const char *raw; %(Ast)s **children; };')
    c('};')
    c()
    c('static %(Ast)s %(nothing)s = {%(NOTHING)s, 0, 0, {""}};')
    c()
    h('/* Construction. */')
    s('%(Nothing)s *%(create_nothing)s(Pool *pool)')
    c('{')
    c('    (void)pool;')
    c('    return (%(Nothing)s *)&%(nothing)s;')
    c('}')
    c('static %(Ast)s *%(create_terminal)s(Pool *pool, %(AstType)s type, const char *raw, bool dup)')
    c('{')
    c('    %(Ast)s rv;')
    c('    memset(&rv, 0, sizeof(rv));')
    c('    rv.type = type;')
    c('    rv.total_tokens = 1;')
    c('    rv.num_children = 0;')
    c('    if (dup)')
    c('        raw = pool_intern_string(pool, raw);')
    c('    rv.raw = raw;')
    c('    return (%(Ast)s *)(const %(Ast)s *)pool_intern(pool, &rv, sizeof(rv));')
    c('}')
    c('static %(Ast)s *%(create_nonterminal)s(Pool *pool, %(AstType)s type, size_t nargs, %(Ast)s **args)')
    c('{')
    c('    %(Ast)s rv;')
    c('    size_t i;')
    c('    assert (nargs >= 2);')
    c('    memset(&rv, 0, sizeof(rv));')
    c('    rv.type = type;')
    c('    rv.total_tokens = args[0]->total_tokens;')
    c('    for (i = 1; i < nargs; ++i)')
    c('    {')
    c('        rv.total_tokens += args[i]->total_tokens;')
    c('    }')
    c('    rv.num_children = nargs;')
    c('    rv.children = (%(Ast)s **)pool_intern(pool, args, nargs * sizeof(%(Ast)s *));')
    c('    return (%(Ast)s *)(const %(Ast)s *)pool_intern(pool, &rv, sizeof(rv));')
    c('}')
    for ast in ast_values:
        if isinstance(ast, (Term, Sequence)):
            if isinstance(ast, Term):
                if ast.is_atom:
                    s('%s *%s(Pool *pool, const char *raw)' % ((lang + ast.tag).camel, (lang + 'create' + ast.tag).lower))
                    c('{')
                    c('    return (%s *)%%(create_terminal)s(pool, %s, raw, true);' % ((lang + ast.tag).camel, (lang + ast.tag).upper))
                    c('}')
                else:
                    raw = ast.tag.visual
                    raw = raw.replace('\\', '\\\\')
                    raw = raw.replace('"', '\\"')
                    raw = '"%s"' % raw
                    raw = raw.replace('%', '%%')
                    s('%s *%s(Pool *pool)' % ((lang + ast.tag).camel, (lang + 'create' + ast.tag).lower))
                    c('{')
                    c('    return (%s *)%%(create_terminal)s(pool, %s, %s, false);' % ((lang + ast.tag).camel, (lang + ast.tag).upper, raw))
                    c('}')
            if isinstance(ast, Sequence):
                ba = ast.bits
                # this is needed for e.g. tree_for_statement, which has 3 opt_comma_expression and 2 tok_semicolon
                bn = ['%s%d' % (b.tag.lower, 1 + ba[:i].count(b)) if ba.count(b) > 1 else b.tag.lower for i, b in enumerate(ba)]
                assert len(ba) == len(set(bn))
                args = ', '.join('%s *%s' % ((lang + b.tag).camel, n) for b, n in zip(ba, bn))
                s('%s *%s(Pool *pool, %s)' % ((lang + ast.tag).camel, (lang + 'create' + ast.tag).lower, args))
                c('{')
                args = ', '.join('(%%(Ast)s *)%s' % n for n in bn)
                c('    %%(Ast)s *args[%d] = {%s};' % (len(bn), args))
                for b, n in zip(ba, bn):
                    c('    assert (%s((%%(Ast)s *)%s));' % ((lang + 'is' + b.tag).lower, n))
                c('    return (%s *)%%(create_nonterminal)s(pool, %s, %d, args);' % ((lang + ast.tag).camel, (lang + ast.tag).upper, len(bn)))
                c('}')
    h()
    h('/* Type checking. */')
    s('bool %(is_nothing)s(%(Ast)s *ast)')
    c('{')
    c('    return ast->type == %(NOTHING)s;')
    c('}')
    for ast in ast_values:
        s('bool %s(%%(Ast)s *ast)' % (lang + 'is' + ast.tag).lower)
        c('{')
        if isinstance(ast, (Term, Sequence)):
            c('    return ast->type == %s;' % (lang + ast.tag).upper)
        if isinstance(ast, Option):
            c('    return %%(is_nothing)s(ast) || %s(ast);' % (lang + 'is' + ast.child.tag).lower)
        if isinstance(ast, Alternative):
            c('    return %s;' % (' || '.join('%s(ast)' % (lang + 'is' + b.tag).lower for b in ast.bits)))
        c('}')
    h()
    h('/* Output. */')
    c('typedef struct IndentationInfo IndentationInfo;')
    c('struct IndentationInfo')
    c('{')
    c('    size_t indentation;')
    c('    const char *prev;')
    c('    %(AstType)s prev_parent;')
    c('};')
    c('static void %(emit_impl_impl)s(const char *next, %(AstType)s next_parent, FILE *fp, IndentationInfo *info)')
    c('{')
    c('    const char *const prev = info->prev;')
    c('    %(AstType)s const prev_parent = info->prev_parent;')
    c('    if (!*next)')
    c('        return;')
    c('    if (strcmp(next, "}") == 0)')
    c('        info->indentation--;')
    c('    if (prev)')
    c('    {')
    c('        bool newline = false;')
    c('        bool no_space = false;')
    if lang.upper == 'C89':
        # sorry, other languages don't get pretty-printing yet.
        c('        newline |= strcmp(prev, ";") == 0 && prev_parent != %(upper)s_TREE_FOR_STATEMENT;')
        c('        newline |= strcmp(next, "{") == 0 && next_parent != %(upper)s_TREE_LIST_INITIALIZER;')
        c('        newline |= strcmp(prev, "{") == 0 && prev_parent != %(upper)s_TREE_LIST_INITIALIZER;')
        c('        newline |= strcmp(next, "}") == 0 && next_parent != %(upper)s_TREE_LIST_INITIALIZER;')
        c('        newline |= strcmp(prev, "}") == 0 && prev_parent != %(upper)s_TREE_LIST_INITIALIZER && strcmp(next, ";") != 0;')
        c('        no_space |= strcmp(prev, "(") == 0;')
        c('        no_space |= strcmp(next, "(") == 0 && (next_parent == %(upper)s_TREE_CALL_EXPRESSION || next_parent == %(upper)s_TREE_FUNCTION_DECLARATOR || next_parent == %(upper)s_TREE_FUNCTION_ABSTRACT_DECLARATOR);')
        c('        no_space |= strcmp(next, ")") == 0;')
        c('        no_space |= strcmp(prev, "[") == 0;')
        c('        no_space |= strcmp(next, "[") == 0;')
        c('        no_space |= strcmp(next, "]") == 0;')
        c('        no_space |= strcmp(next, ",") == 0;')
        c('        no_space |= strcmp(next, ";") == 0;')
        c('        no_space |= strcmp(prev, "*") == 0 && (prev_parent == %(upper)s_TREE_POINTER_DECLARATOR || prev_parent == %(upper)s_TREE_LEAF_POINTER_ABSTRACT_DECLARATOR || prev_parent == %(upper)s_TREE_RECURSIVE_POINTER_ABSTRACT_DECLARATOR);')
        c('        no_space |= strcmp(next, ":") == 0 && (next_parent == %(upper)s_TREE_LABEL_STATEMENT || next_parent == %(upper)s_TREE_CASE_STATEMENT || next_parent == %(upper)s_TREE_DEFAULT_STATEMENT);')
        c('        no_space |= strcmp(prev, ".") == 0;')
        c('        no_space |= strcmp(next, ".") == 0;')
        c('        no_space |= strcmp(prev, "->") == 0;')
        c('        no_space |= strcmp(next, "->") == 0;')
        c('        no_space |= strcmp(next, "++") == 0 && next_parent == %(upper)s_TREE_POST_INCREMENT_EXPRESSION;')
        c('        no_space |= strcmp(next, "--") == 0 && next_parent == %(upper)s_TREE_POST_DECREMENT_EXPRESSION;')
        c('        no_space |= strcmp(prev, "++") == 0 && prev_parent == %(upper)s_TREE_PRE_INCREMENT_EXPRESSION;')
        c('        no_space |= strcmp(prev, "--") == 0 && prev_parent == %(upper)s_TREE_PRE_DECREMENT_EXPRESSION;')
        c('        no_space |= strcmp(prev, "&") == 0 && next[0] != \'&\' && prev_parent == %(upper)s_TREE_ADDRESSOF_EXPRESSION;')
        c('        no_space |= strcmp(prev, "*") == 0 && prev_parent == %(upper)s_TREE_DEREFERENCE_EXPRESSION;')
        c('        no_space |= strcmp(prev, "+") == 0 && next[0] != \'+\' && prev_parent == %(upper)s_TREE_UNARY_PLUS_EXPRESSION;')
        c('        no_space |= strcmp(prev, "-") == 0 && next[0] != \'-\' && prev_parent == %(upper)s_TREE_UNARY_MINUS_EXPRESSION;')
        c('        no_space |= strcmp(prev, "~") == 0 && prev_parent == %(upper)s_TREE_BITWISE_NOT_EXPRESSION;')
        c('        no_space |= strcmp(prev, "!") == 0 && prev_parent == %(upper)s_TREE_LOGICAL_NOT_EXPRESSION;')
        c('        no_space |= strcmp(prev, "sizeof") == 0 && strcmp(next, "(") == 0;')
        c('        no_space |= strcmp(prev, ")") == 0 && prev_parent == %(upper)s_TREE_CAST_EXPRESSION;')
    c('        if (newline)')
    c('        {')
    c('            size_t i;')
    c('            fputc(\'\\n\', fp);')
    c('            for (i = 0; i < info->indentation; ++i)')
    c('                fputs("    ", fp);')
    c('        }')
    c('        else if (!no_space)')
    c('            fputc(\' \', fp);')
    c('    }')
    c('    if (strcmp(next, "{") == 0)')
    c('        info->indentation++;')
    c('    info->prev = next;')
    c('    info->prev_parent = next_parent;')
    c('    fputs(next, fp);')
    c('}')
    c('static void %(emit_impl)s(%(Ast)s *ast, %(AstType)s parent, FILE *fp, IndentationInfo *info)')
    c('{')
    c('    size_t i;')
    c('    if (ast->num_children == 0)')
    c('    {')
    c('         %(emit_impl_impl)s(ast->raw, parent, fp, info);')
    c('    }')
    c('    for (i = 0; i < ast->num_children; ++i)')
    c('    {')
    c('        %(emit_impl)s(ast->children[i], ast->type, fp, info);')
    c('    }')
    c('}')
    s('void %(emit)s(%(Ast)s *ast, FILE *fp)')
    c('{')
    c('    IndentationInfo info;')
    c('    memset(&info, 0, sizeof(info));');
    c('    %(emit_impl)s(ast, %(NOTHING)s, fp, &info);')
    c('    fputc(\'\\n\', fp);')
    c('}')

def main(args=None):
    if args is None:
        import sys
        args = sys.argv[1:]
    if len(args) != 1:
        print('Usage: ./gram.py foo.gram')
    with open(args[0]) as f:
        g = Grammar(f)
    assert args[0] == '%s.gram' % g.language.dash, args[0]
    hname = '%s.gen.h' % g.language.dash
    cname = '%s.gen.c' % g.language.dash
    with open(hname, 'w') as h, open(cname, 'w') as c:
        emit(g, h, c)

if __name__ == '__main__':
    main()
