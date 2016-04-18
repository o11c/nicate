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
#   but WITHOUT ANY WARRANTY without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.

import nicate


def do_match(pat, txt):
    if isinstance(txt, bytes):
        txt = nicate.b2u(txt)
    l = nicate.Lexicon([nicate.Symbol('ANON', pat)])
    t = nicate.Tokenizer(l)
    t.feed(txt)
    m = t.get(True)
    assert m is not None
    assert isinstance(m, tuple)
    assert len(m) == 2
    assert m[0] == 'ANON'
    assert m[1] == txt

def test_regex_atom():
    do_match('a', 'a')
    do_match('"a"', 'a')
    do_match('\'a\'', 'a')
    do_match('[a]', 'a')
    do_match('(a)', 'a')
    do_match('!', '!')
    do_match('%', '%')
    do_match('&', '&')
    do_match(',', ',')
    do_match('-', '-')
    do_match(':', ':')
    do_match(';', ';')
    do_match('<', '<')
    do_match('=', '=')
    do_match('>', '>')
    do_match('@', '@')
    do_match('_', '_')
    do_match('`', '`')
    do_match('~', '~')
    do_match('\\!', '!')
    do_match('\\%', '%')
    do_match('\\&', '&')
    do_match('\\,', ',')
    do_match('\\-', '-')
    do_match('\\:', ':')
    do_match('\\;', ';')
    do_match('\\<', '<')
    do_match('\\=', '=')
    do_match('\\>', '>')
    do_match('\\@', '@')
    do_match('\\_', '_')
    do_match('\\`', '`')
    do_match('\\~', '~')
    do_match('\\ ', ' ')
    do_match('\\"', '"')
    do_match('\\#', '#')
    do_match('\\$', '$')
    do_match('\\\'', '\'')
    do_match('\\(', '(')
    do_match('\\)', ')')
    do_match('\\*', '*')
    do_match('\\+', '+')
    do_match('\\.', '.')
    do_match('\\/', '/')
    do_match('\\?', '?')
    do_match('\\[', '[')
    do_match('\\\\', '\\')
    do_match('\\]', ']')
    do_match('\\^', '^')
    do_match('\\{', '{')
    do_match('\\|', '|')
    do_match('\\}', '}')

def test_regex_atom2():
    do_match('\\0', '\0')
    do_match('\\7', '\7')
    do_match('\\00', '\0')
    do_match('\\70', '\70')
    do_match('\\77', '\77')
    do_match('\\000', '\0')
    do_match('\\300', b'\300')
    do_match('\\377', b'\377')
    do_match('\\x00', '\x00')
    do_match('\\x0f', '\x0f')
    do_match('\\x1f', '\x1f')
    do_match('\\xf0', b'\xf0')
    do_match('\\xf1', b'\xf1')
    do_match('\\xff', b'\xff')

def test_regex_repeat():
    do_match('ab?c', 'ac')
    do_match('ab?c', 'abc')
    do_match('ab*c', 'ac')
    do_match('ab*c', 'abc')
    do_match('ab*c', 'abbc')
    do_match('ab*c', 'abbbc')
    do_match('ab+c', 'abc')
    do_match('ab+c', 'abbc')
    do_match('ab+c', 'abbbc')
    do_match('ab{,1}c', 'ac')
    do_match('ab{,1}c', 'abc')
    do_match('ab{,2}c', 'ac')
    do_match('ab{,2}c', 'abc')
    do_match('ab{,2}c', 'abbc')
    do_match('ab{,3}c', 'ac')
    do_match('ab{,3}c', 'abc')
    do_match('ab{,3}c', 'abbc')
    do_match('ab{,3}c', 'abbbc')
    do_match('ab{0,1}c', 'ac')
    do_match('ab{0,1}c', 'abc')
    do_match('ab{0,2}c', 'ac')
    do_match('ab{0,2}c', 'abc')
    do_match('ab{0,2}c', 'abbc')
    do_match('ab{0,3}c', 'ac')
    do_match('ab{0,3}c', 'abc')
    do_match('ab{0,3}c', 'abbc')
    do_match('ab{0,3}c', 'abbbc')
    do_match('ab{0,}c', 'ac')
    do_match('ab{0,}c', 'abc')
    do_match('ab{0,}c', 'abbc')
    do_match('ab{0,}c', 'abbbc')
    do_match('ab{1}c', 'abc')
    do_match('ab{1,1}c', 'abc')
    do_match('ab{1,2}c', 'abc')
    do_match('ab{1,2}c', 'abbc')
    do_match('ab{1,3}c', 'abc')
    do_match('ab{1,3}c', 'abbc')
    do_match('ab{1,3}c', 'abbbc')
    do_match('ab{1,}c', 'abc')
    do_match('ab{1,}c', 'abbc')
    do_match('ab{1,}c', 'abbbc')
    do_match('ab{2}c', 'abbc')
    do_match('ab{2,2}c', 'abbc')
    do_match('ab{2,3}c', 'abbc')
    do_match('ab{2,3}c', 'abbbc')
    do_match('ab{2,}c', 'abbc')
    do_match('ab{2,}c', 'abbbc')
    do_match('ab{3}c', 'abbbc')
    do_match('ab{3,3}c', 'abbbc')
    do_match('ab{3,}c', 'abbbc')

def test_regex_cat():
    do_match('ab?c?d', 'ad')
    do_match('ab?c?d', 'acd')
    do_match('ab?c?d', 'abd')
    do_match('ab?c?d', 'abcd')

def test_regex_alt():
    do_match('a|aa', 'a')
    do_match('a|aa', 'aa')
    do_match('ab|ac', 'ab')
    do_match('ab|ac', 'ac')
    do_match('bd|cd', 'bd')
    do_match('bd|cd', 'cd')

def test_regex_class():
    do_match('[-]', '-')
    do_match('[\\\\]', '\\')
    do_match('[]]', ']')
    do_match('[a^]', '^')
    do_match('[a^]', 'a')
    do_match('[a-z]', 'a')
    do_match('[a-z]', 'b')
    do_match('[a-z]', 'y')
    do_match('[a-z]', 'z')

def test_regex_seq():
    l = nicate.Lexicon([
        nicate.Symbol('whitespace', '\\ *[ \\n]'),
        nicate.Symbol('A', 'a'),
        nicate.Symbol('AA', 'aa'),
        nicate.Symbol('Z', '[a-z]'),
    ])
    t = nicate.Tokenizer(l)
    t.feed('a')
    assert t.get(False) is None
    assert t.get(True) == ('A', 'a')
    t.feed('ab')
    assert t.get(False) == ('A', 'a')
    assert t.get(False) == ('Z', 'b')
    t.feed('aaa')
    assert t.get(False) == ('AA', 'aa')
    assert t.get(True) == ('A', 'a')
