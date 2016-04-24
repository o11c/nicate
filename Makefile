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
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.

PYTHON = python3

CC = gcc -fsanitize=address,undefined
# Oldest locally tested version.
# CC = gcc-4.2
CFLAGS = -g -O2 ${WARNINGS}
CPPFLAGS =
LDFLAGS =
LDLIBS =
TEST_WRAPPER =
PYTEST_ARGS =

WARNINGS = -Werror -Wall -Wextra -Wformat -Wunused -Wc++-compat -Wmissing-declarations -Wredundant-decls

LIB_OBJECTS = \
    builder.o \
    bridge.o \
    gnu-c.gen.o \
    pool.o \
    hashmap.o \
    bitset.o \
    mre_nfa.o \
    mre_run.o \
    mre_re.o \
    lexer.o \
    automaton.o \
    automaton_auto.o \
    util.o \
    PMurHash.o

MKDIR_FIRST = @mkdir -p ${@D}

############################################################
#        No user servicable parts beyond this point.       #
############################################################

ifneq '$(findstring -fsanitize=address,${CC} ${CFLAGS})' ''
$(info Special support for AddressSanitizer enabled.)
USING_ASAN = yes
else
$(info Special support for AddressSanitizer disabled.)
USING_ASAN = no
endif

ifneq '$(findstring clang,${CC} ${CFLAGS})' ''
$(info Special support for Clang enabled.)
USING_CLANG = yes
else
$(info Special support for Clang disabled.)
USING_CLANG = no
endif

override CFLAGS += -std=c89 -D_POSIX_C_SOURCE=200809L
override CFLAGS += -MMD -MP
override CFLAGS += -fPIC
override CPPFLAGS += -I src/ -I gen/

override PWD := $(shell pwd)

export LD_LIBRARY_PATH:=${PWD}:${LD_LIBRARY_PATH}
export PYTHONPATH:=${PWD}:${PYTHONPATH}
ifeq '${USING_ASAN}' 'yes'
ifeq '${USING_CLANG}' 'yes'
override CC += -shared-libasan
$(warning Clang and shared ASAN do not work together very well.)
endif
$(shell echo 0 > /proc/$$PPID/coredump_filter 2>/dev/null)
export LD_PRELOAD:=$(shell ${CC} -print-file-name=libasan.so):${LD_PRELOAD}
export ASAN_OPTIONS=malloc_context_size=4:abort_on_error=1
export LSAN_OPTIONS=suppressions=leak-suppressions.txt:print_suppressions=0
else
TEST_WRAPPER = valgrind
override LDFLAGS += -Wl,--no-undefined
endif

.SUFFIXES:
.SECONDARY:
.DELETE_ON_ERROR:

default: hello.gen.run hello2.gen.run obj/gnu-c.gen.o obj/nicate-glass.gen.o
test: test-py
test-py: lib/libnicate.so
	@echo Note: assert rewriting may be slow the first time.
	${TEST_WRAPPER} ${PYTHON} -m pytest nicate/ ${PYTEST_ARGS}

bin/hello.x: obj/hello.o lib/libnicate.so
lib/libnicate.so: $(addprefix obj/,${LIB_OBJECTS})
# force order
obj/bridge.o obj/builder.o: gen/gnu-c.gen.h
gen/hello2.gen.c: lib/libnicate.so nicate/*.py

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
obj/%.o: example/%.c
	$(MKDIR_FIRST)
	${CC} ${CPPFLAGS} ${CFLAGS} -c -o $@ $<
gen/%.gen.c gen/%.gen.h: gram/%.gram nicate/*.py
	$(MKDIR_FIRST)
	${PYTHON} -m nicate.grammar $< gen/$*.gen.c gen/$*.gen.h
gen/%.gen.c: example/%.py
	$(MKDIR_FIRST)
	./$< > $@
gen/%.gen.c: bin/%.x
	$(MKDIR_FIRST)
	./$< > $@
%.run: bin/%.x
	./$<

# For debugging - enter the ASAN environment appropriately.
exec-%:
	-+$*

clean:
	rm -rf bin/ obj/ lib/
distclean: clean
	rm -rf gen/

-include obj/*.d
