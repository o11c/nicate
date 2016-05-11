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

C_SOURCES := $(addprefix cache/,$(wildcard src/*.c example/*.c))
C_HEADERS := $(addprefix cache/,$(wildcard src/*.h))
PYTHON_SOURCES := $(addprefix cache/,$(wildcard python/*.py))

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
override CPPFLAGS += -I cache/src/ -I cache/gen/

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
# Python is not valgrind-clean. This never worked.
#TEST_WRAPPER = valgrind
override LDFLAGS += -Wl,--no-undefined
endif

.SUFFIXES:
.SECONDARY:
.DELETE_ON_ERROR:

all: default
default: bin/hello.x bin/hello.gen.x bin/hello2.gen.x bin/hello3.gen.gen.x
default: lib/libnicate.so
default: obj/gnu-c.gen.o obj/nicate-glass.gen.o
test: hello.gen.run hello2.gen.run hello3.gen.gen.run
test: test-py
test-py: lib/libnicate.so
	@echo Note: assert rewriting may be slow the first time.
	${TEST_WRAPPER} ${PYTHON} -m pytest nicate/ ${PYTEST_ARGS}

bin/hello.x: cache/lib/libnicate.so
bin/hello3.gen.x: cache/lib/libnicate.so

lib/libnicate.so: $(addprefix obj/,${LIB_OBJECTS})
# force order
obj/bridge.o obj/builder.o obj/gnu-c.gen.o: cache/gen/gnu-c.gen.h
obj/nicate-glass.gen.o: cache/gen/nicate-glass.gen.h
${C_SOURCES}: ${C_HEADERS}

lib/lib%.so: cache/obj/%.o
	$(MKDIR_FIRST)
	${CC} -shared ${LDFLAGS} $^ ${LDLIBS} -o $@
bin/%.x: cache/obj/%.o
	$(MKDIR_FIRST)
	${CC} ${LDFLAGS} $^ ${LDLIBS} -o $@
obj/%.o: cache/src/%.c
	$(MKDIR_FIRST)
	${CC} ${CPPFLAGS} ${CFLAGS} -c -o $@ $<
obj/%.o: cache/gen/%.c
	$(MKDIR_FIRST)
	${CC} ${CPPFLAGS} ${CFLAGS} -c -o $@ $<
obj/%.o: cache/example/%.c
	$(MKDIR_FIRST)
	${CC} ${CPPFLAGS} ${CFLAGS} -c -o $@ $<
gen/%.gen.c gen/%.gen.h: cache/gram/%.gram ${PYTHON_SOURCES}
	$(MKDIR_FIRST)
	${PYTHON} -m nicate.grammar $< gen/$*.gen.c gen/$*.gen.h
gen/%.gen.c: cache/example/%.py cache/lib/libnicate.so ${PYTHON_SOURCES}
	$(MKDIR_FIRST)
	${PYTHON} $< > $@
gen/%.gen.c: cache/bin/%.x
	$(MKDIR_FIRST)
	./$< > $@
gen/%.gen.c: cache/example/%.glass cache/lib/libnicate.so ${PYTHON_SOURCES}
	${PYTHON} -m nicate.glass < $< > $@
%.run: bin/%.x
	./$<

EXTS :=
EXTS += .c
EXTS += .h
EXTS += .py
EXTS += .glass
EXTS += .gram
EXTS += .o
EXTS += .x
EXTS += .so
$(foreach EXT,${EXTS},$(eval EXTS += .gen${EXT}))
$(foreach EXT,${EXTS},$(eval cache/%${EXT}: %${EXT}; $${MKDIR_FIRST}; cmp -s $$< $$@ || cp $$< $$@))

# For debugging - enter the ASAN environment appropriately.
exec-%:
	-+$*

clean:
	rm -rf bin/ obj/ lib/
	rm -rf cache/
distclean: clean
	rm -rf gen/

-include obj/*.d
