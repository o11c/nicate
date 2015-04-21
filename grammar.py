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
        assert name.replace('-', '').islower(), name
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
        return IdentifierCase('%s-%s' % (self.dash, other.dash), '%s-%s' % (self.visual, other.visual))

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
        self.rules[ty] = Term(IdentifierCase('tok-' + ty, ty))

    def add_atom(self, ty):
        self.rules[ty] = Term(IdentifierCase('tok-' + ty, ty))

    def add_symbol(self, sym, ty):
        self.rules[sym] = Term(IdentifierCase('tok-' + ty, sym))

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
    __slots__ = ('tag',)
    def __init__(self, tag):
        self.tag = tag

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

def emit(grammar, header, source):
    def h(*args, **kwargs):
        print(*args, file=header, **kwargs)
    def c(*args, **kwargs):
        print(*args, file=source, **kwargs)
    def s(sig):
        h(sig, ';', sep='')
        c(sig)

    ast_values = sorted(grammar.rules.values(), key=lambda x: x.tag.space)

    h('/* Generated file, edit c89.gram instead. */')
    h('#pragma once')
    h(generated_copyright)
    h()
    c('/* Generated file, edit c89.gram instead. */')
    c('#include "c89.gen.h"')
    c(generated_copyright)
    c()
    c('#include <assert.h>')
    h('#include <stdbool.h>')
    c('#include <stddef.h>')
    h('#include <stdio.h>')
    c('#include <string.h>')
    h()
    h('#include "fwd.h"')
    c('#include "pool.h"')
    h()
    h()
    c()
    c()
    h('typedef struct C89_Nothing C89_Nothing;')
    for ast in ast_values:
        h('typedef struct C89_{0} C89_{0};'.format(ast.tag.camel))
    h()
    c('/* Concrete types only. */')
    c('typedef enum C89_AstType C89_AstType;')
    c('enum C89_AstType')
    c('{')
    c('    C89_NOTHING,')
    for ast in ast_values:
        if isinstance(ast, (Term, Sequence)):
            c('    C89_%s,' % ast.tag.upper)
    c('};')
    c()
    c('struct C89_Ast')
    c('{')
    c('    C89_AstType type;')
    c('    size_t total_tokens;')
    c('    size_t num_children;')
    c('    union { char *raw; C89_Ast **children; };')
    c('};')
    c()
    c('static C89_Ast c89_nothing = {C89_NOTHING, 0, 0, {""}};')
    c()
    h('/* Construction. */')
    s('C89_Nothing *c89_create_nothing(Pool *pool)')
    c('{')
    c('    (void)pool;')
    c('    return (C89_Nothing *)&c89_nothing;')
    c('}')
    c('static C89_Ast *c89_create_terminal(Pool *pool, C89_AstType type, const char *raw)')
    c('{')
    c('    C89_Ast rv;')
    c('    memset(&rv, 0, sizeof(rv));')
    c('    rv.type = type;')
    c('    rv.total_tokens = 1;')
    c('    rv.num_children = 0;')
    c('    rv.raw = (char *)pool_intern_string(pool, raw);')
    c('    return (C89_Ast *)(const C89_Ast *)pool_intern(pool, &rv, sizeof(rv));')
    c('}')
    c('static C89_Ast *c89_create_nonterminal(Pool *pool, C89_AstType type, size_t nargs, C89_Ast **args)')
    c('{')
    c('    C89_Ast rv;')
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
    c('    rv.children = (C89_Ast **)pool_intern(pool, args, nargs * sizeof(C89_Ast *));')
    c('    return (C89_Ast *)(const C89_Ast *)pool_intern(pool, &rv, sizeof(rv));')
    c('}')
    for ast in ast_values:
        if isinstance(ast, (Term, Sequence)):
            if isinstance(ast, Term):
                s('C89_%s *c89_create_%s(Pool *pool, const char *raw)' % (ast.tag.camel, ast.tag.lower))
                c('{')
                c('    return (C89_%s *)c89_create_terminal(pool, C89_%s, raw);' % (ast.tag.camel, ast.tag.upper))
                c('}')
            if isinstance(ast, Sequence):
                ba = ast.bits
                # this is needed for e.g. tree_for_statement, which has 3 opt_comma_expression and 2 tok_semicolon
                bn = ['%s%d' % (b.tag.lower, 1 + ba[:i].count(b)) if ba.count(b) > 1 else b.tag.lower for i, b in enumerate(ba)]
                assert len(ba) == len(set(bn))
                s('C89_%s *c89_create_%s(Pool *pool, %s)' % (ast.tag.camel, ast.tag.lower, ', '.join('C89_%s *%s' % (b.tag.camel, n) for b, n in zip(ba, bn))))
                c('{')
                c('    C89_Ast *args[%d] = {%s};' % (len(bn), ', '.join('(C89_Ast *)%s' % n for n in bn)))
                for b, n in zip(ba, bn):
                    c('    assert (c89_is_%s((C89_Ast *)%s));' % (b.tag.lower, n))
                c('    return (C89_%s *)c89_create_nonterminal(pool, C89_%s, %d, args);' % (ast.tag.camel, ast.tag.upper, len(bn)))
                c('}')
    h()
    h('/* Type checking. */')
    s('bool c89_is_nothing(C89_Ast *ast)')
    c('{')
    c('    return ast->type == C89_NOTHING;')
    c('}')
    for ast in ast_values:
        s('bool c89_is_%s(C89_Ast *ast)' % ast.tag.lower)
        c('{')
        if isinstance(ast, (Term, Sequence)):
            c('    return ast->type == C89_%s;' % ast.tag.upper)
        if isinstance(ast, Option):
            c('    return c89_is_nothing(ast) || c89_is_%s(ast);' % ast.child.tag.lower)
        if isinstance(ast, Alternative):
            c('    return %s;' % (' || '.join('c89_is_%s(ast)' % b.tag.lower for b in ast.bits)))
        c('}')
    h()
    h('/* Output. */')
    c('typedef struct IndentationInfo IndentationInfo;')
    c('struct IndentationInfo')
    c('{')
    c('    size_t indentation;')
    c('    const char *prev;')
    c('    C89_AstType prev_parent;')
    c('};')
    c('static void c89_emit_impl_impl(const char *next, C89_AstType next_parent, FILE *fp, IndentationInfo *info)')
    c('{')
    c('    const char *const prev = info->prev;')
    c('    C89_AstType const prev_parent = info->prev_parent;')
    c('    if (!*next)')
    c('        return;')
    c('    if (strcmp(next, "}") == 0)')
    c('        info->indentation--;')
    c('    if (prev)')
    c('    {')
    c('        bool newline = false;')
    c('        bool no_space = false;')
    c('        newline |= strcmp(prev, ";") == 0 && prev_parent != C89_TREE_FOR_STATEMENT;')
    c('        newline |= strcmp(next, "{") == 0 && next_parent != C89_TREE_LIST_INITIALIZER;')
    c('        newline |= strcmp(prev, "{") == 0 && prev_parent != C89_TREE_LIST_INITIALIZER;')
    c('        newline |= strcmp(next, "}") == 0 && next_parent != C89_TREE_LIST_INITIALIZER;')
    c('        newline |= strcmp(prev, "}") == 0 && prev_parent != C89_TREE_LIST_INITIALIZER && strcmp(next, ";") != 0;')
    c('        no_space |= strcmp(prev, "(") == 0;')
    c('        no_space |= strcmp(next, "(") == 0 && (next_parent == C89_TREE_CALL_EXPRESSION || next_parent == C89_TREE_FUNCTION_DECLARATOR || next_parent == C89_TREE_FUNCTION_ABSTRACT_DECLARATOR);')
    c('        no_space |= strcmp(next, ")") == 0;')
    c('        no_space |= strcmp(prev, "[") == 0;')
    c('        no_space |= strcmp(next, "[") == 0;')
    c('        no_space |= strcmp(next, "]") == 0;')
    c('        no_space |= strcmp(next, ",") == 0;')
    c('        no_space |= strcmp(next, ";") == 0;')
    c('        no_space |= strcmp(prev, "*") == 0 && (prev_parent == C89_TREE_POINTER_DECLARATOR || prev_parent == C89_TREE_LEAF_POINTER_ABSTRACT_DECLARATOR || prev_parent == C89_TREE_RECURSIVE_POINTER_ABSTRACT_DECLARATOR);')
    c('        no_space |= strcmp(next, ":") == 0 && (next_parent == C89_TREE_LABEL_STATEMENT || next_parent == C89_TREE_CASE_STATEMENT || next_parent == C89_TREE_DEFAULT_STATEMENT);')
    c('        no_space |= strcmp(prev, ".") == 0;')
    c('        no_space |= strcmp(next, ".") == 0;')
    c('        no_space |= strcmp(prev, "->") == 0;')
    c('        no_space |= strcmp(next, "->") == 0;')
    c('        no_space |= strcmp(next, "++") == 0 && next_parent == C89_TREE_POST_INCREMENT_EXPRESSION;')
    c('        no_space |= strcmp(next, "--") == 0 && next_parent == C89_TREE_POST_DECREMENT_EXPRESSION;')
    c('        no_space |= strcmp(prev, "++") == 0 && prev_parent == C89_TREE_PRE_INCREMENT_EXPRESSION;')
    c('        no_space |= strcmp(prev, "--") == 0 && prev_parent == C89_TREE_PRE_DECREMENT_EXPRESSION;')
    c('        no_space |= strcmp(prev, "&") == 0 && next[0] != \'&\' && prev_parent == C89_TREE_ADDRESSOF_EXPRESSION;')
    c('        no_space |= strcmp(prev, "*") == 0 && prev_parent == C89_TREE_DEREFERENCE_EXPRESSION;')
    c('        no_space |= strcmp(prev, "+") == 0 && next[0] != \'+\' && prev_parent == C89_TREE_UNARY_PLUS_EXPRESSION;')
    c('        no_space |= strcmp(prev, "-") == 0 && next[0] != \'-\' && prev_parent == C89_TREE_UNARY_MINUS_EXPRESSION;')
    c('        no_space |= strcmp(prev, "~") == 0 && prev_parent == C89_TREE_BITWISE_NOT_EXPRESSION;')
    c('        no_space |= strcmp(prev, "!") == 0 && prev_parent == C89_TREE_LOGICAL_NOT_EXPRESSION;')
    c('        no_space |= strcmp(prev, "sizeof") == 0 && strcmp(next, "(") == 0;')
    c('        no_space |= strcmp(prev, ")") == 0 && prev_parent == C89_TREE_CAST_EXPRESSION;')
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
    c('static void c89_emit_impl(C89_Ast *ast, C89_AstType parent, FILE *fp, IndentationInfo *info)')
    c('{')
    c('    size_t i;')
    c('    if (ast->num_children == 0)')
    c('    {')
    c('         c89_emit_impl_impl(ast->raw, parent, fp, info);')
    c('    }')
    c('    for (i = 0; i < ast->num_children; ++i)')
    c('    {')
    c('        c89_emit_impl(ast->children[i], ast->type, fp, info);')
    c('    }')
    c('}')
    s('void c89_emit(C89_Ast *ast, FILE *fp)')
    c('{')
    c('    IndentationInfo info;')
    c('    memset(&info, 0, sizeof(info));');
    c('    c89_emit_impl(ast, C89_NOTHING, fp, &info);')
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
    assert args[0] == '%s.gram' % g.language.dash
    hname = '%s.gen.h' % g.language.dash
    cname = '%s.gen.c' % g.language.dash
    with open(hname, 'w') as h, open(cname, 'w') as c:
        emit(g, h, c)

if __name__ == '__main__':
    main()
