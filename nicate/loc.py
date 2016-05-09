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


class LocationTracker:
    __slots__ = ('file', 'line', 'col')

    def __init__(self, file):
        self.reset()

    def reset(self):
        self.line = 1
        self.col = 1

    def track(self, t, s):
        for c in s:
            if c == '\n':
                self.line += 1
                self.col = 1
            else:
                self.col += 1

    def msg(self, lvl, msg):
        return '%s:%d:%d: %s: %s' % (self.file, self.line, self.col, lvl, msg)

    def error(self, msg):
        return self.msg('error', msg)

    def warning(self, msg):
        return self.msg('error', msg)
