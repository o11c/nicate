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

import cffi
from collections import namedtuple

from . import grammar
from .util import u2b, b2u, eprint as print
from .loc import LocationTracker


def load():
    import sys
    assert sys.version_info[0] >= 3

    ffi = cffi.FFI()
    ffi.cdef('typedef uint64_t uintmax_t;')
    for fn in [
            'src/fwd.h',
            'src/builder.h',
            'src/lexer.h',
            'src/automaton.h',
            'src/automaton-internal.h',
    ]:
        with open(fn) as f:
            x = [l.replace(' __extension__ ', ' ') for l in f if not l.startswith('#')]
            ffi.cdef('\n'.join(x))
    return ffi

nicate_ffi = load()
nicate_library = nicate_ffi.dlopen('lib/libnicate.so')

class Builder:
    __slots__ = ('_c_builder',)

    def __init__(self):
        self._c_builder = nicate_library.builder_create()

    def __del__(self):
        nicate_library.builder_destroy(self._c_builder)

class OwnedObjectBase:
    __slots__ = ('_c_object', '_owner')

    def __init__(self, *args):
        raise TypeError('Do not instantiate directly, call methods on Builder')

def make_class(k):
    class Foo(OwnedObjectBase):
        __slots__ = ()
    Foo.__name__ = k
    Foo.__qualname__ = k
    return Foo

def desc_ty(ty):
    # since this is all undocumented, fail a lot
    if type(ty) == cffi.model.PrimitiveType:
        if ty.name == '_Bool':
            return 'bool'
        if ty.name == 'double':
            return 'float'
        if ty.name in {'char', 'uint64_t', 'unsigned int'}:
            return ty.name
        assert False, ty.name
    if type(ty) == cffi.model.ConstPointerType:
        if type(ty.totype) == cffi.model.PrimitiveType:
            if ty.totype.name == 'char':
                return 'str'
            assert False, ty.name
    if type(ty) == cffi.model.PointerType:
        if type(ty.totype) == cffi.model.PointerType:
            if type(ty.totype.totype) == cffi.model.StructType:
                name = ty.totype.totype.name
                if name.startswith('Build'):
                    if name != 'Builder':
                        name = name[5:]
                else:
                    assert False, name
                return 'list[%s]' % name
        elif type(ty.totype) == cffi.model.StructType:
            name = ty.totype.name
            if name.startswith('Build'):
                if name != 'Builder':
                    name = name[5:]
            else:
                assert False, name
            return name
        elif type(ty.totype) == cffi.model.PrimitiveType:
            if ty.totype.name == 'char':
                return 'str'
            assert False, ty.name
    assert False, '%s of %s' % (ty, type(ty))

def make_fun(sk, lk, v):
    if isinstance(v, tuple) and len(v) == 2:
        v = v[0]
    r = v.result
    a = v.args
    rd = desc_ty(r)
    ad = ['self'] + [desc_ty(i) for i in a[1:] if not (isinstance(i, cffi.model.PrimitiveType) and i.name == 'size_t')]

    needs_slice = lk in {'build_expr_multi_char', 'build_expr_string'}
    if needs_slice:
        lk += '_slice'
    c = getattr(nicate_library, lk)

    def f(b, *args):
        assert isinstance(b, Builder)
        actuals = [b._c_builder]
        for a in args:
            # FIXME arguably this is all wrong and it should be like `ad`.
            if isinstance(a, OwnedObjectBase):
                actuals.append(a._c_object)
            elif isinstance(a, list):
                actuals.append(len(a))
                actuals.append([i._c_object for i in a])
            elif isinstance(a, str):
                s = u2b(a)
                actuals.append(s)
            elif isinstance(a, (float, int)): #including bool
                actuals.append(a)
            else:
                raise TypeError(str(type(a)))
        if needs_slice:
            assert isinstance(a, str)
            actuals.append(len(s))
        r = c(*actuals)
        assert r is not None
        assert isinstance(r, nicate_ffi.CData)
        t = nicate_ffi.typeof(r)
        assert t.kind == 'pointer'
        n = t.item.cname
        assert n.isalnum()
        assert n.startswith('Build')
        n = n[5:]
        w = globals()[n]
        assert issubclass(w, OwnedObjectBase)
        x = object.__new__(w)
        x._owner = b
        x._c_object = r
        return x
    f.__name__ = sk
    f.__qualname__ = 'Builder.%s' % sk
    f.__doc__ = '(%s) -> %s' % (', '.join(ad), rd)
    return f

# yay mucking with internals
for k, v in nicate_ffi._parser._declarations.items():
    if k.startswith('typedef'):
        k = k[8:]
        if k == 'Builder' or not k.startswith('Build'):
            continue
        k = k[5:]
        globals()[k] = make_class(k)
    if k.startswith('function'):
        k = k[9:]
        if k.startswith('builder_'):
            getattr(nicate_library, k)
            continue
        if k.startswith('build_'):
            if k.endswith('_slice'):
                # actually called from the other things
                continue
            sk = k[6:]
            setattr(Builder, sk, make_fun(sk, k, v))

def emit_to_file(tu, file):
    nicate_library.builder_emit_tu_to_file(tu._c_object, file)
TranslationUnit.emit_to_file = emit_to_file

def emit_to_string(tu):
    ptr = nicate_library.builder_emit_tu_to_string(tu._c_object)
    rv = nicate_ffi.string(ptr)
    nicate_library.builder_free_string(ptr);
    return b2u(rv)
TranslationUnit.emit_to_string = emit_to_string

del k, sk, v
del make_class, make_fun, load
del emit_to_file, emit_to_string


def new_string(u):
    if u is None:
        return nicate_ffi.NULL
    b = u2b(u)
    return nicate_ffi.new('char[]', b)

Symbol = namedtuple('Symbol', ('name', 'regex'))

class Lexicon:
    __slots__ = ('_c_lexicon', '_names', '_regexes')

    def __init__(self, symbols):
        syms = [(new_string(s.name), new_string(s.regex)) for s in symbols]
        self._c_lexicon = nicate_library.lexicon_create(len(syms), nicate_ffi.new('Symbol[]', syms))
        self._names = ['error'] + [s.name for s in symbols]
        self._regexes = ['(.)'] + [s.regex for s in symbols]
        for i in range(len(symbols)):
            assert self.__name(i) == self.name(i)

    def __del__(self):
        nicate_library.lexicon_destroy(self._c_lexicon)

    def __name(self, idx):
        c = nicate_library.lexicon_name(self._c_lexicon, idx)
        b = nicate_ffi.string(c)
        return b2u(b)

    def name(self, idx):
        return self._names[idx]

class Tokenizer:
    __slots__ = ('_c_tokenizer', '_py_lexicon')

    def __init__(self, lexicon):
        self._c_tokenizer = nicate_library.tokenizer_create(lexicon._c_lexicon)
        self._py_lexicon = lexicon

    def __del__(self):
        nicate_library.tokenizer_destroy(self._c_tokenizer)

    def clone(self):
        rv = object.__new__(Tokenizer)
        rv._c_tokenizer = nicate_library.tokenizer_clone(self._c_tokenizer)
        rv._py_lexicon = self._py_lexicon
        return rv

    def reset(self):
        nicate_library.tokenizer_reset(self._c_tokenizer)

    def feed(self, u):
        b = u2b(u)
        nicate_library.tokenizer_feed_slice(self._c_tokenizer, b, len(b))

    def get(self, at_eof):
        t = self._c_tokenizer
        if not at_eof and not nicate_library.tokenizer_ready(t):
            return None
        i = nicate_library.tokenizer_sym(t)
        b = nicate_library.tokenizer_text_start(t)
        l = nicate_library.tokenizer_text_len(t)
        s = b2u(nicate_ffi.buffer(b, l)[:])
        nicate_library.tokenizer_pop(t)
        return (self._py_lexicon.name(i), s)

class Grammar:
    __slots__ = ('_names', '_indices', '_num_terminals', '_num_nonterminals', '_c_grammar', '_derivs')

    def __init__(self, terminals, nonterminals, rules, *, derivs=None):
        terminals = list(terminals)
        nonterminals = list(nonterminals)
        rules = [(l, [i for i in r]) for (l, r) in rules]
        terminal_set = set(terminals)
        nonterminal_set = set(nonterminals)
        assert len(terminals) == len(terminal_set)
        assert len(nonterminals) == len(nonterminal_set)
        assert not (terminal_set & nonterminal_set)
        self._derivs = derivs or [(None, [])] * len(rules)

        idx_to_names = self._names = []
        names_to_idx = self._indices = {}
        for e in terminals:
            names_to_idx[e] = len(idx_to_names)
            idx_to_names.append(e)
        self._num_terminals = len(terminals)
        for e in nonterminals:
            names_to_idx[e] = len(idx_to_names)
            idx_to_names.append(e)
        self._num_nonterminals = len(nonterminals)

        c_rules = []
        for (l, r) in rules:
            assert l in nonterminal_set
            lhs = names_to_idx[l]
            num_rhses = len(r)
            rhses = [names_to_idx[x] for x in r]
            c_rule = nicate_library.rule_create(lhs, num_rhses, rhses)
            c_rules.append(c_rule)
        self._c_grammar = nicate_library.grammar_create(len(terminals), len(nonterminals), len(c_rules), c_rules)

    def __del__(self):
        nicate_library.grammar_destroy(self._c_grammar)

class Automaton:
    __slots__ = ('_py_grammar', '_c_automaton')

    def __init__(self, grammar, states=None):
        self._py_grammar = grammar
        if states is None:
            self._c_automaton = nicate_library.automaton_create_auto(grammar._c_grammar)
            return
        c_states = []
        for (d, t, n) in states:
            default = d
            acts = [default] * grammar._num_terminals
            for (k, v) in t.items():
                k = grammar._indices[k]
                acts[k] = v
            gotos = [ERROR()] * grammar._num_nonterminals
            for (k, v) in n.items():
                k = grammar._indices[k]
                gotos[k - grammar._num_terminals] = v
            c_state = nicate_library.state_create(grammar._c_grammar, default, acts, gotos)
            c_states.append(c_state)
        self._c_automaton = nicate_library.automaton_create(grammar._c_grammar, len(c_states), c_states)

    def __del__(self):
        nicate_library.automaton_destroy(self._c_automaton)

    def clone(self):
        rv = object.__new__(Automaton)
        rv._py_grammar = self._py_grammar
        rv._c_automaton = nicate_library.automaton_clone(self._c_automaton)
        return rv

    def reset(self):
        nicate_library.automaton_reset(self._c_automaton)

    def feed(self, sym, data):
        a = self._c_automaton
        sym = self._py_grammar._indices[sym]
        data = u2b(data)
        rv = nicate_library.automaton_feed_term(a, sym, data, len(data))
        return bool(rv)

    def _get_count(self):
        return nicate_library.automaton_tree_count(self._c_automaton)

    def get(self, classes, *, _index=0):
        trees = nicate_library.automaton_result(self._c_automaton)
        tree_count = self._get_count()
        assert 0 <= _index < tree_count
        return self._get(trees + _index, classes)

    def _get(self, tree, classes):
        while tree.num_children == 1:
            type_name, deriv = self._py_grammar._derivs[tree.rule]
            if deriv:
                # This guarantees that rules of the form:
                #
                # tu:
                #     tu? stmt
                #
                # will always return a `tu`, with `nothing` as the sentinel,
                # rather than a `stmt`. It also handles cases like:
                #
                # expr-list:
                #     expr-list? ','= expr
                break
            tree = tree.children
        if not tree.num_children:
            type_name = self._py_grammar._names[tree.type]
            data = nicate_ffi.buffer(tree.token, tree.token_length)[:]
            return classes[type_name](data)
        else:
            type_name, deriv = self._py_grammar._derivs[tree.rule]
            d = 0
            j = 0
            args = [None] * (tree.num_children + len(deriv))
            for i in range(len(args)):
                if d < len(deriv) and i == deriv[d]:
                    args[i] = classes['nothing']()
                    d += 1
                else:
                    args[i] = self._get(tree.children + j, classes)
                    j += 1
            return classes[type_name](args)


def SHIFT(s):
    return (nicate_library.SHIFT, s)
def GOTO(s):
    return (nicate_library.GOTO, s)
def REDUCE(r):
    return (nicate_library.REDUCE, r)
def ERROR():
    return (nicate_library.ERROR, 0)
def ACCEPT():
    return (nicate_library.ACCEPT, 0)

def lower_grammar(gram):
    '''
    Change a grammar.Grammar into a nicate.Grammar.

    Other than rearranging everything, the important thing here is the
    removal of all `_opt` rules.

    We also inline all `anon-` and `tag-` rules and aliases.
    '''

    terminals = ['$end']
    nonterminals = ['$accept']
    start = gram.start
    while isinstance(start, grammar.Alias):
        start = start.child
    if isinstance(start, grammar.Option):
        # not grammar.Slave
        start = start.child
    rules = [('$accept', [start.tag.dash, '$end'])]
    derivs = [('$accept', [])]

    def add_rule(lhs, rhses, name, deriv):
        assert isinstance(lhs, grammar.IdentifierCase)
        assert all(isinstance(rhs, grammar.Rule) for rhs in rhses)
        if len(rhses) == 1:
            rhs, = rhses
            if isinstance(rhs, grammar.Sequence):
                if rhs.tag.visual.startswith(('anon-', 'tag-')):
                    add_rule(lhs, rhs.bits, name, deriv)
                    return
        for i, rhs in enumerate(rhses):
            while isinstance(rhs, grammar.Alias):
                rhses[i] = rhs = rhs.child
            if isinstance(rhs, grammar.Option):
                # We only need to track the omitted cases.
                add_rule(lhs, rhses[:i] + rhses[i+1:], name, deriv + [i+len(deriv)])
                add_rule(lhs, rhses[:i] + [rhs.child] + rhses[i+1:], name, deriv)
                return
            if isinstance(rhs, grammar.Slave):
                # Slave must be preceded by Option (already split).
                if deriv and deriv[-1] == i+len(deriv) - 1:
                    add_rule(lhs, rhses[:i] + rhses[i+1:], name, deriv + [i+len(deriv)])
                else:
                    add_rule(lhs, rhses[:i] + [rhs.child] + rhses[i+1:], name, deriv)
                return
            if isinstance(rhs, grammar.Sequence):
                assert not rhs.tag.visual.startswith(('anon-', 'tag-'))
        rules.append((lhs.dash, [rhs.tag.dash for rhs in rhses]))
        derivs.append((name, deriv))

    for key, rule in gram.rules.items():
        assert key == rule.tag.visual
        if isinstance(rule, grammar.Term):
            assert len(nonterminals) == 1
            assert len(rules) == 1
            terminals.append(rule.tag.dash)
            continue
        if isinstance(rule, (grammar.Option, grammar.Slave, grammar.Alias)):
            continue
        if key.startswith(('anon-', 'tag-')):
            continue
        nonterminals.append(rule.tag.dash)
        if isinstance(rule, grammar.Sequence):
            add_rule(rule.tag, rule.bits, rule.tag.dash, [])
            continue
        if isinstance(rule, grammar.Alternative):
            for alt in rule.bits:
                add_rule(rule.tag, [alt], alt.tag.dash, [])
            continue
        assert False, '%s has unknown subclass %s' % (rule.tag.visual, type(rule).__name__)
    return Grammar(terminals, nonterminals, rules, derivs=derivs)

def lower_lexicon(gram):
    symbols = [Symbol(term.tag.dash, term.regex) for term in gram.patterns]
    return Lexicon(symbols)

class Error(Exception):
    pass

class LexerError(Error):
    pass

class ParserError(Error):
    pass

class Parser:
    __slots__ = ('_py_tokenizer', '_py_automaton', '_loc', '_classes', 'Tree', 'Nothing', 'Terminal', 'Nonterminal')

    def __init__(self, grammar):
        print('%s: creating tokenizer ...' % grammar.language.dash)
        self._py_tokenizer = Tokenizer(lower_lexicon(grammar))
        print('%s: creating automaton ...' % grammar.language.dash)
        self._py_automaton = Automaton(lower_grammar(grammar))
        self._loc = LocationTracker('<unknown-file>')

        self._classes = self._build_classes(grammar)

    def _build_classes(self, g):
        # TODO figure out what to do e.g. for mixins?
        rv = {}
        class Tree:
            __slots__ = ()
        self.Tree = Tree

        class Nothing(Tree):
            __slots__ = ()
        rv['nothing'] = self.Nothing = Nothing

        class Terminal(Tree):
            __slots__ = ('data',)

            def __init__(self, data):
                assert isinstance(data, bytes)
                self.data = data
        self.Terminal = Terminal

        class Nonterminal(Tree):
            __slots__ = ('children',)

            def __init__(self, children):
                assert isinstance(children, list)
                assert all(isinstance(child, Tree) for child in children)
                self.children = children
        self.Nonterminal = Nonterminal

        for rule in g.rules.values():
            if isinstance(rule, grammar.Term):
                base = Terminal
            elif isinstance(rule, grammar.Sequence):
                base = Nonterminal
            else:
                assert isinstance(rule, (grammar.Option, grammar.Slave, grammar.Alias, grammar.Alternative))
                continue
            class Foo(base):
                __slots__ = ()
            Foo.tag = rule.tag
            Foo.__name__ = rule.tag.camel
            Foo.__qualname__ = '%s.%s' % (g.language.lower, Foo.__name__)
            Foo.__module__ = None
            rv[rule.tag.dash] = Foo
        return rv

    def clone(self):
        rv = object.__new__(Parser)
        rv._py_tokenizer = self._py_tokenizer.clone()
        rv._py_automaton = self._py_automaton.clone()
        for slot in Parser.__slots__[2:]:
            setattr(rv, slot, getattr(self, slot))
        return rv

    def reset(self):
        self._py_tokenizer.reset()
        self._py_automaton.reset()
        self._loc.reset()

    def feed(self, text, at_eof):
        tokenizer = self._py_tokenizer
        automaton = self._py_automaton
        grammar = automaton._py_grammar

        tokenizer.feed(text)
        while True:
            sym = tokenizer.get(at_eof)
            if sym is None:
                break
            sym_type, sym_data = sym
            if sym_type == 'error':
                if at_eof:
                    sym_type = '$end'
                    assert sym_data == ''
                else:
                    raise LexerError(self._loc.error('Unexpected character: %s' % sym_data))
            if sym_type != 'whitespace':
                if not automaton.feed(sym_type, sym_data):
                    if sym_type == '$end':
                        if automaton._get_count() == 0:
                            break
                    raise ParserError(self._loc.error('Unexpected %s: %s' % (sym_type, sym_data)))
            self._loc.track(sym_type, sym_data)
            if sym_data == '':
                break

    def get(self, *, _index=0):
        return self._py_automaton.get(self._classes, _index=_index)

    def parse_file(self, fo):
        self = self.clone()
        self._loc.file = getattr(fo, 'name', '<unknown-file>')
        while True:
            chunk = fo.read(512)
            self.feed(chunk, not chunk)
            if not chunk:
                break
        count = self._py_automaton._get_count()
        if count == 0:
            return self.Nothing()
        assert count == 2
        return self.get()
