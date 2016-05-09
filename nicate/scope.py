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


class Scope:
    __slots__ = ('name', 'parent', 'vars')

    def __init__(self, parent, name):
        self.name = name
        self.parent = parent
        self.vars = {}

    def get(self, key):
        scope = self
        while scope is not None:
            try:
                return scope.vars[key]
            except KeyError:
                pass
            scope = scope.parent
        raise KeyError((key, self.name))

    def has(self, key):
        scope = self
        while scope is not None:
            if key in scope.vars:
                return True
            scope = scope.parent
        return False

    def put(self, key, value, *, shadow=False, replace=False):
        if not shadow and self.parent and self.parent.has(key):
            raise KeyError((key, self.name))
        if not replace and key in self.vars:
            raise KeyError((key, self.name))
        self.vars[key] = value
