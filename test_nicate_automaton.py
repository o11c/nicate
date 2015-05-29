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
#   but WITHOUT ANY WARRANTY without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.

import nicate
from nicate import SHIFT, GOTO, REDUCE, ERROR, ACCEPT


def test_terminals():
    inputs = [
            '0 ;',
            '0 ; 1 ; 2 ;',
            'x ;',
            '2 + 2 ;',
            '2 - 2 ;',
            '2 * 2 ;',
            '2 / 2 ;',
            '2 % 2 ;',
            '2 < 2 ;',
            '2 > 2 ;',
            '2 <= 2 ;',
            '2 >= 2 ;',
            '2 == 2 ;',
            '2 != 2 ;',
            '( 2 + 2 ) ;',
            'x = 2 ;',
            '1 + 2 + 3 ;',
            '1 - 2 - 3 ;',
            '1 * 2 * 3 ;',
            '1 / 2 / 3 ;',
            '1 % 2 % 3 ;',
            'y = x = 2 ;',
            'x = ( a == b ) == ( c == d ) ;',
    ]

    terminals = [
            '$end',
            'error',
            '$unused',
            '*',
            '/',
            '%',
            '+',
            '-',
            '=',
            '<',
            '>',
            '<=',
            '>=',
            '==',
            '!=',
            '(',
            ')',
            ';',
            'ID',
            'LIT',
    ]
    nonterminals = [
            '$accept',
            'all',
            'top',
            'assign',
            'cmp',
            'add',
            'mul',
            'prim',
    ]
    rules = [
            ('$accept', ['all', '$end']),
            ('all', ['top']),
            ('all', ['all', 'top']),
            ('top', ['assign', ';']),
            ('assign', ['cmp']),
            ('assign', ['ID', '=', 'assign']),
            ('cmp', ['add']),
            ('cmp', ['add', '<', 'add']),
            ('cmp', ['add', '>', 'add']),
            ('cmp', ['add', '<=', 'add']),
            ('cmp', ['add', '>=', 'add']),
            ('cmp', ['add', '==', 'add']),
            ('cmp', ['add', '!=', 'add']),
            ('add', ['mul']),
            ('add', ['add', '+', 'mul']),
            ('add', ['add', '-', 'mul']),
            ('mul', ['prim']),
            ('mul', ['mul', '*', 'prim']),
            ('mul', ['mul', '/', 'prim']),
            ('mul', ['mul', '%', 'prim']),
            ('prim', ['ID']),
            ('prim', ['LIT']),
            ('prim', ['(', 'cmp', ')']),
    ]
    states = [
            # 0
            (
                ERROR(), {
                    '(': SHIFT(1),
                    'ID': SHIFT(2),
                    'LIT': SHIFT(3),
                }, {
                    'all': GOTO(4),
                    'top': GOTO(5),
                    'assign': GOTO(6),
                    'cmp': GOTO(7),
                    'add': GOTO(8),
                    'mul': GOTO(9),
                    'prim': GOTO(10),
                }
            ),
            # 1
            (
                ERROR(), {
                    '(': SHIFT(1),
                    'ID': SHIFT(11),
                    'LIT': SHIFT(3),
                }, {
                    'cmp': GOTO(12),
                    'add': GOTO(8),
                    'mul': GOTO(9),
                    'prim': GOTO(10),
                }
            ),
            # 2
            (
                REDUCE(20), {
                    '=': SHIFT(13),
                }, {
                }
            ),
            # 3
            (
                REDUCE(21), {
                }, {
                }
            ),
            # 4
            (
                ERROR(), {
                    '$end': SHIFT(14),
                    '(': SHIFT(1),
                    'ID': SHIFT(2),
                    'LIT': SHIFT(3),
                }, {
                    'top': GOTO(15),
                    'assign': GOTO(6),
                    'cmp': GOTO(7),
                    'add': GOTO(8),
                    'mul': GOTO(9),
                    'prim': GOTO(10),
                }
            ),
            # 5
            (
                REDUCE(1), {
                }, {
                }
            ),
            # 6
            (
                ERROR(), {
                    ';': SHIFT(16),
                }, {
                }
            ),
            # 7
            (
                REDUCE(4), {
                }, {
                }
            ),
            # 8
            (
                REDUCE(6), {
                    '+': SHIFT(17),
                    '-': SHIFT(18),
                    '<': SHIFT(19),
                    '>': SHIFT(20),
                    '<=': SHIFT(21),
                    '>=': SHIFT(22),
                    '==': SHIFT(23),
                    '!=': SHIFT(24),
                }, {
                }
            ),
            # 9
            (
                REDUCE(13), {
                    '*': SHIFT(25),
                    '/': SHIFT(26),
                    '%': SHIFT(27),
                }, {
                }
            ),
            # 10
            (
                REDUCE(16), {
                }, {
                }
            ),
            # 11
            (
                REDUCE(20), {
                }, {
                }
            ),
            # 12
            (
                ERROR(), {
                    ')': SHIFT(28),
                }, {
                }
            ),
            # 13
            (
                ERROR(), {
                    '(': SHIFT(1),
                    'ID': SHIFT(2),
                    'LIT': SHIFT(3),
                }, {
                    'assign': GOTO(29),
                    'cmp': GOTO(7),
                    'add': GOTO(8),
                    'mul': GOTO(9),
                    'prim': GOTO(10),
                }
            ),
            # 14
            (
                ACCEPT(), {
                }, {
                }
            ),
            # 15
            (
                REDUCE(2), {
                }, {
                }
            ),
            # 16
            (
                REDUCE(3), {
                }, {
                }
            ),
            # 17
            (
                ERROR(), {
                    '(': SHIFT(1),
                    'ID': SHIFT(11),
                    'LIT': SHIFT(3),
                }, {
                    'mul': GOTO(30),
                    'prim': GOTO(10),
                }
            ),
            # 18
            (
                ERROR(), {
                    '(': SHIFT(1),
                    'ID': SHIFT(11),
                    'LIT': SHIFT(3),
                }, {
                    'mul': GOTO(31),
                    'prim': GOTO(10),
                }
            ),
            # 19
            (
                ERROR(), {
                    '(': SHIFT(1),
                    'ID': SHIFT(11),
                    'LIT': SHIFT(3),
                }, {
                    'add': GOTO(32),
                    'mul': GOTO(9),
                    'prim': GOTO(10),
                }
            ),
            # 20
            (
                ERROR(), {
                    '(': SHIFT(1),
                    'ID': SHIFT(11),
                    'LIT': SHIFT(3),
                }, {
                    'add': GOTO(33),
                    'mul': GOTO(9),
                    'prim': GOTO(10),
                }
            ),
            # 21
            (
                ERROR(), {
                    '(': SHIFT(1),
                    'ID': SHIFT(11),
                    'LIT': SHIFT(3),
                }, {
                    'add': GOTO(34),
                    'mul': GOTO(9),
                    'prim': GOTO(10),
                }
            ),
            # 22
            (
                ERROR(), {
                    '(': SHIFT(1),
                    'ID': SHIFT(11),
                    'LIT': SHIFT(3),
                }, {
                    'add': GOTO(35),
                    'mul': GOTO(9),
                    'prim': GOTO(10),
                }
            ),
            # 23
            (
                ERROR(), {
                    '(': SHIFT(1),
                    'ID': SHIFT(11),
                    'LIT': SHIFT(3),
                }, {
                    'add': GOTO(36),
                    'mul': GOTO(9),
                    'prim': GOTO(10),
                }
            ),
            # 24
            (
                ERROR(), {
                    '(': SHIFT(1),
                    'ID': SHIFT(11),
                    'LIT': SHIFT(3),
                }, {
                    'add': GOTO(37),
                    'mul': GOTO(9),
                    'prim': GOTO(10),
                }
            ),
            # 25
            (
                ERROR(), {
                    '(': SHIFT(1),
                    'ID': SHIFT(11),
                    'LIT': SHIFT(3),
                }, {
                    'prim': GOTO(38),
                }
            ),
            # 26
            (
                ERROR(), {
                    '(': SHIFT(1),
                    'ID': SHIFT(11),
                    'LIT': SHIFT(3),
                }, {
                    'prim': GOTO(39),
                }
            ),
            # 27
            (
                ERROR(), {
                    '(': SHIFT(1),
                    'ID': SHIFT(11),
                    'LIT': SHIFT(3),
                }, {
                    'prim': GOTO(40),
                }
            ),
            # 28
            (
                REDUCE(22), {
                }, {
                }
            ),
            # 29
            (
                REDUCE(5), {
                }, {
                }
            ),
            # 30
            (
                REDUCE(14), {
                    '*': SHIFT(25),
                    '/': SHIFT(26),
                    '%': SHIFT(27),
                }, {
                }
            ),
            # 31
            (
                REDUCE(15), {
                    '*': SHIFT(25),
                    '/': SHIFT(26),
                    '%': SHIFT(27),
                }, {
                }
            ),
            # 32
            (
                REDUCE(7), {
                    '+': SHIFT(17),
                    '-': SHIFT(18),
                }, {
                }
            ),
            # 33
            (
                REDUCE(8), {
                    '+': SHIFT(17),
                    '-': SHIFT(18),
                }, {
                }
            ),
            # 34
            (
                REDUCE(9), {
                    '+': SHIFT(17),
                    '-': SHIFT(18),
                }, {
                }
            ),
            # 35
            (
                REDUCE(10), {
                    '+': SHIFT(17),
                    '-': SHIFT(18),
                }, {
                }
            ),
            # 36
            (
                REDUCE(11), {
                    '+': SHIFT(17),
                    '-': SHIFT(18),
                }, {
                }
            ),
            # 37
            (
                REDUCE(12), {
                    '+': SHIFT(17),
                    '-': SHIFT(18),
                }, {
                }
            ),
            # 38
            (
                REDUCE(17), {
                }, {
                }
            ),
            # 39
            (
                REDUCE(18), {
                }, {
                }
            ),
            # 40
            (
                REDUCE(19), {
                }, {
                }
            ),
    ]
    grammar = nicate.Grammar(terminals, nonterminals, rules)
    automaton = nicate.RawAutomaton(grammar, states)

    for i in inputs:
        for x in i.split():
            sym, val = pair(x)
            b = automaton.feed(sym, val)
            assert b
        if True:
            sym, val = ('$end', '')
            b = automaton.feed(sym, val)
            assert b
        automaton.reset()

    b = automaton.feed('$end', '')
    assert not b
    automaton.reset()

    b = automaton.feed('LIT', '0')
    assert b
    b = automaton.feed('$end', '')
    assert not b
    automaton.reset()


def pair(v):
    if v.isdigit():
        return ('LIT', v)
    if v.isalpha():
        return ('ID', v)
    return (v, v)
