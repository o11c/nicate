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
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.


import builtins
import sys


def u2b(u):
    return u.encode('utf-8', errors='surrogateescape')

def b2u(b):
    return b.decode('utf-8', errors='surrogateescape')

def isoctb(b):
    return b'0' <= b <= b'7'

def ishexb(b):
    return b'0' <= b <= b'9' or b'A' <= b <= b'F' or b'a' <= b <= b'f'

def eprint(*args, **kwargs):
    kwargs.setdefault('file', sys.stderr)
    return builtins.print(*args, **kwargs)
