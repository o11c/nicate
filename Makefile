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

CC = gcc -fsanitize=address
CFLAGS = -g -O2 ${WARNINGS}
CPPFLAGS =
LDFLAGS =
LDLIBS =

WARNINGS = -Werror -Wall -Wextra -Wformat -Wunused -Wc++-compat -Wmissing-declarations -Wredundant-decls

LIB_OBJECTS = builder.o bridge.o c89.gen.o pool.o hashmap.o PMurHash.o

############################################################
#        No user servicable parts beyond this point.       #
############################################################

override CFLAGS += -std=c89 -D_POSIX_C_SOURCE=200809L
override CFLAGS += -MMD -MP
override CFLAGS += -fPIC

export LD_LIBRARY_PATH:=.:${LD_LIBRARY_PATH}

.SUFFIXES:
.SECONDARY:
.DELETE_ON_ERROR:

default: hello.gen.run hello2.gen.run

hello.x: hello.o libnicate.so
libnicate.so: ${LIB_OBJECTS}
# force order
bridge.o builder.o: c89.gen.h
hello2.gen.c: libnicate.so

lib%.so: %.o
	${CC} -shared ${LDFLAGS} $^ ${LDLIBS} -o $@
%.x: %.o
	${CC} ${LDFLAGS} $^ ${LDLIBS} -o $@
%.o: %.c
	${CC} ${CPPFLAGS} ${CFLAGS} -c -o $@ $<
%.gen.c %.gen.h: %.py
	./$<
%.gen.c: %.x
	./$< > $@
%.run: %.x
	./$<

clean:
	rm -f *.x *.o *.d *.so
distclean: clean
	rm -f *.gen.*

-include *.d
