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

import cffi
from collections import namedtuple


def u2b(u):
    return u.encode('utf-8', errors='surrogateescape')

def b2u(b):
    return b.decode('utf-8', errors='surrogateescape')

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
            x = [l for l in f if not l.startswith('#')]
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
        if ty.name == 'long double':
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
    assert False, '%s of %s' % (ty, type(ty))

def make_fun(sk, lk, v):
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
    b = u2b(u)
    return nicate_ffi.new('char[]', b)

Symbol = namedtuple('Symbol', ('name', 'regex'))

class Lexicon:
    __slots__ = ('_c_lexicon',)

    def __init__(self, symbols):
        syms = [(new_string(s.name), new_string(s.regex)) for s in symbols]
        self._c_lexicon = nicate_library.lexicon_create(len(syms), nicate_ffi.new('Symbol[]', syms))

    def __del__(self):
        nicate_library.lexicon_destroy(self._c_lexicon)

    def name(self, idx):
        c = nicate_library.lexicon_name(self._c_lexicon, idx)
        b = nicate_ffi.string(c)
        return b2u(b)

class Tokenizer:
    __slots__ = ('_c_tokenizer', '_py_lexicon')

    def __init__(self, lexicon):
        self._c_tokenizer = nicate_library.tokenizer_create(lexicon._c_lexicon)
        self._py_lexicon = lexicon

    def __del__(self):
        nicate_library.tokenizer_destroy(self._c_tokenizer)

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
    __slots__ = ('_names', '_indices', '_num_terminals', '_num_nonterminals', '_c_grammar')

    def __init__(self, terminals, nonterminals, rules):
        terminals = list(terminals)
        nonterminals = list(nonterminals)
        rules = [(l, [i for i in r]) for (l, r) in rules]
        terminal_set = set(terminals)
        nonterminal_set = set(nonterminals)
        assert len(terminals) == len(terminal_set)
        assert len(nonterminals) == len(nonterminal_set)
        assert not (terminal_set & nonterminal_set)

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

class RawAutomaton:
    __slots__ = ('_py_grammar', '_c_automaton')

    def __init__(self, grammar, states):
        self._py_grammar = grammar
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

    def reset(self):
        nicate_library.automaton_reset(self._c_automaton)

    def feed(self, sym, data):
        a = self._c_automaton
        sym = self._py_grammar._indices[sym]
        data = u2b(data)
        rv = nicate_library.automaton_feed_term(a, sym, data, len(data))
        return bool(rv)

    def dump_trees(self):
        trees = nicate_library.automaton_result(self._c_automaton)
        tree_count = nicate_library.automaton_tree_count(self._c_automaton)
        _dump_trees(0, self._py_grammar, trees, tree_count)

def _dump_line(level, *args):
    print('  ' * level, *args, sep='')

def _dump_tree(level, grammar, ptr):
    type_name = grammar._names[ptr.type]
    while ptr.num_children == 1:
        ptr = ptr.children
        type_name += ' ' + grammar._names[ptr.type]
    _dump_line(level, 'type: ', type_name)
    if ptr.num_children:
        _dump_line(level, 'children:')
        _dump_trees(level + 1, grammar, ptr.children, ptr.num_children)
    else:
        _dump_line(level, 'token: ', repr(nicate_ffi.buffer(ptr.token, ptr.token_length)[:]))

def _dump_trees(level, grammar, base, size):
    _dump_line(level, '[')
    for i in range(size):
        if i:
            _dump_line(level, ',')
        _dump_tree(level + 1, grammar, base + i)
    _dump_line(level, ']')

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
