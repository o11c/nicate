#   Copyright © 2016 Ben Longbons
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

import glob
import pytest

import grammar
import nicate

@pytest.fixture(params=glob.glob('gram/*.gram'))
def gram(request):
    with open(request.param) as f:
        return grammar.Grammar(request.param, f)

def lower_grammar(gram):
    '''
    Change a grammar.Grammar into a nicate.Grammar.

    Other than rearranging everything, the important thing here is the
    removal of all `_opt` rules.

    We also inline all `anon-` rules and aliases.
    '''

    terminals = ['$end']
    nonterminals = ['$accept']
    start = gram.start
    while isinstance(start, grammar.Alias):
        start = start.child
    if isinstance(start, grammar.Option):
        start = start.child
    rules = [('$accept', [start.tag.dash, '$end'])]

    def add_rule(lhs, rhses):
        assert isinstance(lhs, grammar.IdentifierCase)
        assert all(isinstance(rhs, grammar.Rule) for rhs in rhses)
        if len(rhses) == 1:
            rhs, = rhses
            if isinstance(rhs, grammar.Sequence):
                if rhs.tag.visual.startswith('anon-'):
                    add_rule(lhs, rhs.bits)
                    return
        for i, rhs in enumerate(rhses):
            while isinstance(rhs, grammar.Alias):
                rhses[i] = rhs = rhs.child
            if isinstance(rhs, grammar.Option):
                add_rule(lhs, rhses[:i] + rhses[i+1:])
                add_rule(lhs, rhses[:i] + [rhs.child] + rhses[i+1:])
                return
            if isinstance(rhs, grammar.Sequence):
                assert not rhs.tag.visual.startswith('anon-')
        rules.append((lhs.dash, [rhs.tag.dash for rhs in rhses]))

    for key, rule in gram.rules.items():
        assert key == rule.tag.visual
        if isinstance(rule, grammar.Term):
            assert len(nonterminals) == 1
            assert len(rules) == 1
            terminals.append(rule.tag.dash)
            continue
        if isinstance(rule, (grammar.Option, grammar.Alias)):
            continue
        if key.startswith('anon-'):
            continue
        nonterminals.append(rule.tag.dash)
        if isinstance(rule, grammar.Sequence):
            add_rule(rule.tag, rule.bits)
            continue
        if isinstance(rule, grammar.Alternative):
            for alt in rule.bits:
                add_rule(rule.tag, [alt])
            continue
        assert False, '%s has unknown subclass %s' % (rule.tag.visual, type(rule).__name__)
    return nicate.Grammar(terminals, nonterminals, rules)

def test_grammar_is_lr1(gram):
    gram = lower_grammar(gram)
    automaton = nicate.Automaton(gram)
