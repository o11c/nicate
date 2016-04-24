#!/usr/bin/env python3
#   Copyright © 2015-2016 Ben Longbons
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


import collections
import string

generated_copyright = '''
/*
    Copyright © 2015-2016 Ben Longbons

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
    def __init__(self, a, b):
        super().__init__(self, '%s: %s' % (a, b))

class IdentifierCase:
    __slots__ = ('space', 'dash', 'lower', 'upper', 'camel', 'visual')

    def __init__(self, name, visual=None):
        for bit in unquote(name).split('-'):
            bit = bit.rstrip(string.digits)
            assert bit and all(c.islower() for c in bit), name
        self.dash = name
        space = self.space = name.replace('-', ' ')
        lower = self.lower = name.replace('-', '_')
        self.upper = lower.upper()
        self.camel = ''.join([x + ('_' if x[-1].isdigit() else '') for x in space.title().split()]).rstrip('_')
        self.visual = visual or name

    def __repr__(self):
        name = self.dash
        if self.visual != name:
            name = '%r, %r' % (name, self.visual)
        else:
            name = repr(name)
        return 'IdentifierCase(%s, <space=%r, lower=%r, upper=%r, camel=%r>)' % (name, self.space, self.lower, self.upper, self.camel)

    def __eq__(self, other):
        return (self.dash, self.visual) == (other.dash, other.visual)

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

def re_escape(s):
    return "'%s'" % s.replace("'", "'\\''")

def unquote(s):
    if s[0] == "'" == s[-1]:
        s = s[1:-1]
    assert "'" not in s
    return s

class Grammar:
    __slots__ = ('filename', 'language', 'rules', 'start', 'patterns')

    def __init__(self, filename, src):
        self.filename = filename
        self.language = None
        self.rules = collections.OrderedDict()
        self.start = None
        self.patterns = []

        whitespace = None
        cur_name = None
        cur_rule = None

        for i, orig_line in enumerate(src, 1):
            line = orig_line.strip()
            if line.startswith('#') or not line:
                continue
            words = line.split()
            if words[0] == 'language':
                if self.language:
                    raise GrammarError(i, 'language specified twice')
                if len(words) != 2:
                    raise GrammarError(i, 'language takes 1 argument')
                self.language = IdentifierCase(words[1])
                continue
            if not self.language:
                raise GrammarError(i, 'language not specified')
            if words[0] == 'whitespace':
                if whitespace:
                    raise GrammarError(i, 'whitespace specified twice')
                if len(words) != 2:
                    raise GrammarError(i, 'whitespace takes 1 argument')
                whitespace = words[1]
                continue
            if not whitespace:
                raise GrammarError(i, 'whitespace not specified')
            if self.start is None:
                if words[0] == 'keyword':
                    if len(words) != 2:
                        raise GrammarError(i, 'keyword takes 1 argument')
                    self.add_keyword(words[1])
                    continue
                elif words[0] == 'atom':
                    if len(words) != 3:
                        if len(words) != 2:
                            raise GrammarError(i, 'atom takes 1 or 2 arguments')
                        words.append(None)
                    self.add_atom(words[1], words[2])
                    continue
                elif words[0] == 'symbol':
                    if len(words) != 3:
                        raise GrammarError(i, 'symbol takes 2 argument')
                    self.add_symbol(words[1], words[2])
                    continue
                elif words[0] == 'start':
                    if len(words) != 2:
                        raise GrammarError(i, 'start takes 1 argument')
                    self.start = IdentifierCase(words[1])
                    continue
                raise GrammarError(i, 'expected start')
            tag = None
            if words[-1].startswith('#'):
                tag = words.pop()[1:]
                assert tag != ''
            if line.endswith(':'):
                assert len(line) != 1 and len(words) == 1 and tag is None
                if not orig_line[0].strip():
                    raise GrammarError(i, 'Rule names should not be indented')
                if cur_rule is not None:
                    self.add_rule(cur_name, cur_rule, cur_tags)
                cur_name = line[:-1]
                cur_rule = []
                cur_tags = []
            else:
                if orig_line[0].strip():
                    raise GrammarError(i, 'Rule bodies should be indented')
                cur_rule.append(words)
                cur_tags.append(tag)
        if cur_rule is None:
            raise GrammarError('eof', 'no rules')
        self.add_rule(cur_name, cur_rule, cur_tags)
        self.patterns.append(Term(IdentifierCase('whitespace'), True, regex=whitespace))

        print('%s: %d rules parsed; resolving cross-references ...' % (self.language.visual, len(self.rules)))
        assert self.start is not None
        self.start = self.rules[self.start.visual]
        undefined = set()
        used = {self.start.tag.visual}
        for v in sorted(self.rules.values(), key=lambda x: x.tag.space):
            if isinstance(v, (Alternative, Sequence)):
                for i, b in enumerate(v.bits):
                    if isinstance(b, IdentifierCase):
                        used.add(b.visual)
                        try:
                            v.bits[i] = self.rules[b.visual]
                        except KeyError:
                            undefined.add(b.visual)
                    else:
                        used.add(b.tag.visual)
            elif isinstance(v, (Alias, Option, Separator, Delimiter, Star, Plus)):
                if isinstance(v.child, IdentifierCase):
                    used.add(v.child.visual)
                    try:
                        v.child = self.rules[v.child.visual]
                    except KeyError:
                        undefined.add(v.child.visual)
                else:
                    used.add(v.child.tag.visual)
            else:
                assert isinstance(v, Term), type(v).__name__
        nullable = [k for (k, v) in self.rules.items() if not isinstance(v, (Option, Separator, Delimiter, Alias, Star)) and v.nullable()]
        if nullable:
            nullable.sort()
            print('Warning: Nullable rules (%d):' % len(nullable))
            for v in nullable:
                print('-', v)
        unused = set(self.rules.keys()) - used
        if unused:
            print('Warning: Unused rules (%d):' % len(unused))
            for v in sorted(unused):
                print('-', v)
        if undefined:
            print('Error: Undefined rules (%d):' % len(undefined))
            for v in sorted(undefined):
                print('-', v)
            raise KeyError('%d rules' % len(undefined))

    def set_rule(self, k, v):
        if isinstance(v, Term):
            self.patterns.append(v)
        if self.rules.setdefault(k, v) is not v:
            raise KeyError(k)
        assert self.start is not None or isinstance(v, Term)
        return v

    def add_keyword(self, ty):
        nouscore = ty.strip('_').replace('_', '-')
        if ty.startswith('_'):
            nouscore = nouscore.lower()
        regex = re_escape(ty)
        self.set_rule(ty, Term(IdentifierCase('kw-' + nouscore, ty), False, regex=regex))

    def add_atom(self, ty, regex):
        self.set_rule(ty, Term(IdentifierCase('atom-' + ty, ty), True, regex=regex))

    def add_symbol(self, sym, ty):
        regex = re_escape(unquote(sym))
        self.set_rule(sym, Term(IdentifierCase('sym-' + ty, sym), False, regex=regex))

    def add_rule(self, name, alts, tags):
        assert len(alts) == len(tags)
        if len(alts) == 0:
            raise GrammarError(name, 'empty rule')
        elif len(alts) == 1:
            assert tags[0] is None, tags[0]
            if len(alts[0]) == 1:
                self.set_rule(name, Alias(name, self.get_rule(alts[0][0])))
            else:
                self.set_rule(name, Sequence(name, [self.get_rule(a) for a in alts[0]]))
        else:
            for i, a in enumerate(alts):
                if len(a) != 1:
                    alts[i] = [self.anon_seq(alts[i], tags[i])]
                else:
                    assert tags[i] is None
            self.set_rule(name, Alternative(name, [self.get_rule(a) for a, in alts]))

    def anon_seq(self, bits, tag):
        rule_bits = [self.get_rule(a) for a in bits]
        if tag:
            tag = 'tag-' + tag
        else:
            tag = '-'.join([r.tag.dash.split('-', 1)[1] if isinstance(r, Rule) else r.dash for r in rule_bits])
            tag = 'anon-' + tag
        if tag in self.rules:
            print('Warning: coalescing multiple tagged or anonymous sequences: %s.' % tag)
            assert isinstance(self.rules[tag], Sequence)
            assert self.rules[tag].bits == rule_bits
        else:
            self.set_rule(tag, Sequence(tag, rule_bits))
        return tag

    def get_rule(self, vis):
        if vis.endswith('?'):
            return self.get_opt(vis, Option, 'opt-')
        if vis.endswith('&'):
            return self.get_opt(vis, Separator, 'sep-')
        if vis.endswith('!'):
            return self.get_opt(vis, Delimiter, 'delim-')
        if vis.endswith('*'):
            return self.get_opt(vis, Star, 'star-')
        if vis.endswith('+'):
            return self.get_opt(vis, Plus, 'plus-')
        return self.rules.get(vis) or IdentifierCase(vis)

    def get_opt(self, vis, cls, pfx):
        rv = self.rules.get(vis)
        if not rv:
            base = vis[:-1]
            maybe_tok = self.rules.get(base)
            if isinstance(maybe_tok, Term):
                base2 = maybe_tok.tag.dash.split('-', 1)[1]
            else:
                base2 = base
            rv = self.set_rule(vis, cls(IdentifierCase(pfx + base2, vis), self.get_rule(base)))
        return rv

class Rule:
    __slot__ = ()

    def __repr__(self):
        # note: do *not* include `bits` here, because that would recurse
        return '%s(%r)' % (self.__class__.__name__, self.tag)

    def struct_tag(self):
        return self.tag

class Term(Rule):
    __slots__ = ('tag', 'is_atom', 'regex')
    def __init__(self, tag, is_atom, *, regex):
        self.tag = tag
        self.is_atom = is_atom
        self.regex = regex

    def nullable(self, stack=None):
        return False

    def comment(self, lang):
        return 'Term'

class Option(Rule):
    __slots__ = ('tag', 'child')
    def __init__(self, tag, child):
        self.tag = tag
        self.child = child

    def nullable(self, stack=None):
        if not isinstance(self.child, IdentifierCase):
            assert not self.child.nullable(stack)
        return True

    def comment(self, lang):
        return '%s | %s' % ((lang + self.child.tag).camel, (lang + 'nothing').camel)

    def struct_tag(self):
        child_tag = self.child.struct_tag()
        child_tag = IdentifierCase(child_tag.dash.split('-', 1)[1])
        return 'opt' + child_tag

# A separator must *always* be used as the last member of a sequence, which
# in turn must be in some repeating context.
class Separator(Rule):
    __slots__ = ('tag', 'child')
    def __init__(self, tag, child):
        assert False, 'NYI'
        self.tag = tag
        self.child = child

    def nullable(self, stack=None):
        if not isinstance(self.child, IdentifierCase):
            assert not self.child.nullable(stack)
        return True

    def comment(self, lang):
        return '%s | %s' % ((lang + self.child.tag).camel, (lang + 'nothing').camel)

    def struct_tag(self):
        return 'sep' + self.child.struct_tag()

# For our purposes, "delimiter" is a node that can either act as a
# terminator or a separator.
#
# For example, the comma in C can be used like:
#
#   int x[] = {1, 2};
#   int x[] = {1, 2, };
#
# Note that a node that can only act as a terminator needs no special
# handling.
class Delimiter(Rule):
    __slots__ = ('tag', 'child')
    def __init__(self, tag, child):
        assert False, 'NYI'
        self.tag = tag
        self.child = child

    def nullable(self, stack=None):
        if not isinstance(self.child, IdentifierCase):
            assert not self.child.nullable(stack)
        return True

    def comment(self, lang):
        return '%s | %s' % ((lang + self.child.tag).camel, (lang + 'nothing').camel)

    def struct_tag(self):
        return 'delim' + self.child.struct_tag()

class Star(Rule):
    __slots__ = ('tag', 'child')
    def __init__(self, tag, child):
        assert False, 'NYI'
        self.tag = tag
        self.child = child

    def nullable(self, stack=None):
        if not isinstance(self.child, IdentifierCase):
            assert not self.child.nullable(stack)
        return True

    def comment(self, lang):
        return '%s | %s' % ((lang + 'repeat' + self.child.tag).camel, (lang + 'nothing').camel)

    def struct_tag(self):
        return 'star' + self.child.struct_tag()

class Plus(Rule):
    __slots__ = ('tag', 'child')
    def __init__(self, tag, child):
        assert False, 'NYI'
        self.tag = tag
        self.child = child

    def nullable(self, stack=None):
        if not isinstance(self.child, IdentifierCase):
            assert not self.child.nullable(stack)
        return False

    def comment(self, lang):
        return (lang + 'repeat' + self.child.tag).camel

    def struct_tag(self):
        return 'plus' + self.child.struct_tag()

class Alias(Rule):
    __slots__ = ('tag', 'child')
    def __init__(self, tag, child):
        self.tag = IdentifierCase('alias-' + tag, tag)
        self.child = child

    def nullable(self, stack=None):
        if isinstance(self.child, IdentifierCase):
            return False
        return self.child.nullable(stack)

    def comment(self, lang):
        return 'Alias'

    def struct_tag(self):
        return self.child.struct_tag()

class Sequence(Rule):
    __slots__ = ('tag', 'bits')
    def __init__(self, tag, bits):
        self.tag = IdentifierCase('tree-' + tag, tag)
        self.bits = list(bits)

    def nullable(self, stack=None):
        stack = stack or []
        if self in stack:
            return False
        stack.append(self)
        try:
            return all(not isinstance(b, IdentifierCase) and b.nullable(stack) for b in self.bits)
        finally:
            stack.pop()

    def comment(self, lang):
        return ' '.join((lang + b.tag).camel for b in self.bits)

class Alternative(Rule):
    __slots__ = ('tag', 'bits')
    def __init__(self, tag, bits):
        self.tag = IdentifierCase('any-' + tag, tag)
        self.bits = list(bits)

    def nullable(self, stack=None):
        stack = stack or []
        if self in stack:
            return False
        stack.append(self)
        try:
            return any(not isinstance(b, IdentifierCase) and b.nullable(stack) for b in self.bits)
        finally:
            stack.pop()

    def comment(self, lang):
        return ' | '.join((lang + b.tag).camel for b in self.bits)

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

    f.input_filename = grammar.filename
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

    h('/* Generated file, edit %(input_filename)s instead. */')
    h('#pragma once')
    h(generated_copyright)
    h()
    c('/* Generated file, edit %(input_filename)s instead. */')
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
    h()
    for ast in ast_values:
        h('/* %s */' % ast.comment(lang))
        h('typedef struct {} {};'.format((lang + ast.struct_tag()).camel, (lang + ast.tag).camel))
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
    c('    __extension__ union { const char *raw; %(Ast)s **children; };')
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
    c('    size_t i, only = (size_t)-2;')
    c('    assert (nargs >= 2);')
    c('    for (i = 0; i < nargs; ++i)')
    c('    {')
    c('        if (!%(is_nothing)s(args[i]))')
    c('        {')
    c('            if (only == (size_t)-2)')
    c('            {')
    c('                only = i;')
    c('            }')
    c('            else')
    c('            {')
    c('                only = (size_t)-1;')
    c('                break;')
    c('            }')
    c('        }')
    c('')
    c('    }')
    c('    assert (only != (size_t)-2);')
    c('    if (only != (size_t)-1)')
    c('    {')
    c('        return args[only];')
    c('    }')
    c('    memset(&rv, 0, sizeof(rv));')
    c('    rv.type = type;')
    c('    rv.total_tokens = 0;')
    c('    for (i = 0; i < nargs; ++i)')
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
                    raw = unquote(ast.tag.visual)
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
            head = 'return'
            if isinstance(ast, Sequence):
                nn = []
                for bit in ast.bits:
                    if not bit.nullable():
                        nn.append(bit)
                assert len(nn) != 0
                if len(nn) == 1:
                    head = 'return %s(ast) ||' % (lang + 'is' + nn[0].tag).lower
            c('    %s ast->type == %s;' % (head, (lang + ast.tag).upper))
        if isinstance(ast, Option):
            c('    return %%(is_nothing)s(ast) || %s(ast);' % (lang + 'is' + ast.child.tag).lower)
        if isinstance(ast, Alias):
            c('    return %s(ast);' % (lang + 'is' + ast.child.tag).lower)
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
    if lang.upper == 'GNU_C':
        # sorry, other languages don't get pretty-printing yet.
        c('        newline |= strcmp(prev, ";") == 0 && prev_parent != %(upper)s_TREE_TAG_FOR_EXPR_STATEMENT && prev_parent != %(upper)s_TREE_TAG_FOR_EXPR_STATEMENT_EXCEPT_IF && prev_parent != %(upper)s_TREE_TAG_FOR_DECL_STATEMENT && prev_parent != %(upper)s_TREE_TAG_FOR_DECL_STATEMENT_EXCEPT_IF;')
        c('        newline |= strcmp(next, "{") == 0 && next_parent != %(upper)s_TREE_TAG_BRACED_INITIALIZER_COMMA && next_parent != %(upper)s_TREE_TAG_BRACED_INITIALIZER;')
        c('        newline |= strcmp(prev, "{") == 0 && prev_parent != %(upper)s_TREE_TAG_BRACED_INITIALIZER_COMMA && prev_parent != %(upper)s_TREE_TAG_BRACED_INITIALIZER;')
        c('        newline |= strcmp(next, "}") == 0 && next_parent != %(upper)s_TREE_TAG_BRACED_INITIALIZER_COMMA && next_parent != %(upper)s_TREE_TAG_BRACED_INITIALIZER;')
        c('        newline |= strcmp(prev, "}") == 0 && prev_parent != %(upper)s_TREE_TAG_BRACED_INITIALIZER_COMMA && prev_parent != %(upper)s_TREE_TAG_BRACED_INITIALIZER && strcmp(next, ";") != 0;')
        c('        no_space |= strcmp(prev, "(") == 0;')
        c('        no_space |= strcmp(next, "(") == 0 && (next_parent == %(upper)s_TREE_TAG_CALL_EXPRESSION || next_parent == %(upper)s_TREE_TAG_LEGACY_FUN_DIRECT_DECLARATOR || next_parent == %(upper)s_TREE_TAG_FORWARD_PARAM_FUN_DIRECT_DECLARATOR || next_parent == %(upper)s_TREE_TAG_FUN_DIRECT_DECLARATOR || next_parent == %(upper)s_TREE_TAG_FORWARD_PARAM_FUN_DIRECT_ABSTRACT_DECLARATOR || next_parent == %(upper)s_TREE_TAG_FUN_DIRECT_ABSTRACT_DECLARATOR);')
        c('        no_space |= strcmp(next, ")") == 0;')
        c('        no_space |= strcmp(prev, "[") == 0;')
        c('        no_space |= strcmp(next, "[") == 0;')
        c('        no_space |= strcmp(next, "]") == 0;')
        c('        no_space |= strcmp(next, ",") == 0;')
        c('        no_space |= strcmp(next, ";") == 0;')
        c('        no_space |= strcmp(prev, "*") == 0 && (prev_parent == %(upper)s_TREE_POINTER || prev_parent == %(upper)s_TREE_DECLARATOR);')
        c('        no_space |= strcmp(next, ":") == 0 && (next_parent == %(upper)s_TREE_TAG_NAMED_LABEL || next_parent == %(upper)s_TREE_TAG_CASE_LABEL || next_parent == %(upper)s_TREE_TAG_CASE_RANGE_LABEL || next_parent == %(upper)s_TREE_TAG_DEFAULT_LABEL || next_parent == %(upper)s_TREE_TAG_DEFAULT_GENERIC_ASSOCIATION || next_parent == %(upper)s_TREE_TAG_TYPE_GENERIC_ASSOCIATION);')
        c('        no_space |= strcmp(prev, ".") == 0;')
        c('        no_space |= strcmp(next, ".") == 0;')
        c('        no_space |= strcmp(prev, "->") == 0;')
        c('        no_space |= strcmp(next, "->") == 0;')
        c('        no_space |= strcmp(next, "++") == 0 && next_parent == %(upper)s_TREE_TAG_POST_INCR_EXPRESSION;')
        c('        no_space |= strcmp(next, "--") == 0 && next_parent == %(upper)s_TREE_TAG_POST_DECR_EXPRESSION;')
        c('        no_space |= strcmp(prev, "++") == 0 && prev_parent == %(upper)s_TREE_TAG_PRE_INCR_EXPRESSION;')
        c('        no_space |= strcmp(prev, "--") == 0 && prev_parent == %(upper)s_TREE_TAG_PRE_DECR_EXPRESSION;')
        c('        no_space |= strcmp(prev, "&") == 0 && next[0] != \'&\' && prev_parent == %(upper)s_TREE_TAG_ADDRESSOF_EXPRESSION;')
        c('        no_space |= strcmp(prev, "*") == 0 && prev_parent == %(upper)s_TREE_TAG_DEREF_EXPRESSION;')
        c('        no_space |= strcmp(prev, "+") == 0 && next[0] != \'+\' && prev_parent == %(upper)s_TREE_TAG_UNARY_PLUS_EXPRESSION;')
        c('        no_space |= strcmp(prev, "-") == 0 && next[0] != \'-\' && prev_parent == %(upper)s_TREE_TAG_UNARY_MINUS_EXPRESSION;')
        c('        no_space |= strcmp(prev, "~") == 0 && prev_parent == %(upper)s_TREE_TAG_BIT_NOT_EXPRESSION;')
        c('        no_space |= strcmp(prev, "!") == 0 && prev_parent == %(upper)s_TREE_TAG_LOG_NOT_EXPRESSION;')
        c('        no_space |= strcmp(prev, "sizeof") == 0 && strcmp(next, "(") == 0;')
        c('        no_space |= strcmp(prev, ")") == 0 && (prev_parent == %(upper)s_TREE_TAG_COMPOUND_LITERAL_COMMA || prev_parent == %(upper)s_TREE_TAG_COMPOUND_LITERAL || prev_parent == %(upper)s_TREE_TAG_ACTUAL_CAST_EXPRESSION);')
    else:
        c('        (void)prev_parent;')
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
    c('    memset(&info, 0, sizeof(info));')
    c('    %(emit_impl)s(ast, %(NOTHING)s, fp, &info);')
    c('    fputc(\'\\n\', fp);')
    c('}')

def main(args=None):
    import os.path
    import sys
    if args is None:
        args = sys.argv[1:]
    if len(args) != 3:
        sys.exit('Usage: ./gram.py foo.gram foo.c foo.h')
    with open(args[0]) as f:
        g = Grammar(args[0], f)
    assert os.path.basename(args[0]) == '%s.gram' % g.language.dash, args[0]
    hname = args[2]
    cname = args[1]
    with open(hname, 'w') as h, open(cname, 'w') as c:
        emit(g, h, c)

if __name__ == '__main__':
    main()
