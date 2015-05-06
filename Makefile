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

LIB_OBJECTS = \
    builder.o \
    bridge.o \
    c89.gen.o \
    pool.o \
    hashmap.o \
    bitset.o \
    mre_nfa.o \
    mre_run.o \
    mre_re.o \
    PMurHash.o

MKDIR_FIRST = @mkdir -p ${@D}

############################################################
#        No user servicable parts beyond this point.       #
############################################################

override CFLAGS += -std=c89 -D_POSIX_C_SOURCE=200809L
override CFLAGS += -MMD -MP
override CFLAGS += -fPIC
override CPPFLAGS += -I src/ -I gen/

export LD_LIBRARY_PATH:=.:${LD_LIBRARY_PATH}

.SUFFIXES:
.SECONDARY:
.DELETE_ON_ERROR:

default: hello.gen.run hello2.gen.run obj/gnu-c.gen.o bin/lex.x
test: default
	./test_nicate.py

bin/lex.x: obj/lex.o lib/libnicate.so
bin/hello.x: obj/hello.o lib/libnicate.so
lib/libnicate.so: $(addprefix obj/,${LIB_OBJECTS})
# force order
obj/bridge.o obj/builder.o: gen/c89.gen.h
gen/hello2.gen.c: lib/libnicate.so nicate.py

lib/lib%.so: obj/%.o
	$(MKDIR_FIRST)
	${CC} -shared ${LDFLAGS} $^ ${LDLIBS} -o $@
bin/%.x: obj/%.o
	$(MKDIR_FIRST)
	${CC} ${LDFLAGS} $^ ${LDLIBS} -o $@
obj/%.o: src/%.c
	$(MKDIR_FIRST)
	${CC} ${CPPFLAGS} ${CFLAGS} -c -o $@ $<
obj/%.o: gen/%.c
	$(MKDIR_FIRST)
	${CC} ${CPPFLAGS} ${CFLAGS} -c -o $@ $<
gen/%.gen.c gen/%.gen.h: gram/%.gram grammar.py
	$(MKDIR_FIRST)
	./grammar.py $< gen/$*.gen.c gen/$*.gen.h
gen/%.gen.c: %.py
	$(MKDIR_FIRST)
	./$< > $@
gen/%.gen.c: bin/%.x
	$(MKDIR_FIRST)
	./$< > $@
%.run: bin/%.x
	./$<

clean:
	rm -rf bin/ obj/ lib/
distclean: clean
	rm -rf gen/

-include obj/*.d
