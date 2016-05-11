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
import io
import pytest
import sys


@pytest.fixture(scope='session')
def glass():
    from nicate.grammar import Grammar
    from nicate.core import Parser
    fn = 'gram/nicate-glass.gram'

    with open(fn) as fo:
        g = Grammar(fn, fo)
    return Parser(g)

@pytest.mark.parametrize('fn', glob.glob('glass/test/*.glass'))
def test_lowering(glass, fn):
    from nicate.glass import AstCreator, AstLowerer

    with open(fn) as fo:
        glass_tu = glass.parse_file(fo)
    semantic_tu = AstCreator(glass).visit(glass_tu)
    c_tu = AstLowerer().visit(semantic_tu)
    sys.stdout.write(c_tu.emit_to_string())
