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

def test_grammar_is_lr1(gram):
    gram = nicate.lower_grammar(gram)
    automaton = nicate.Automaton(gram)
