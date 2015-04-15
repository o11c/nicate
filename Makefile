CC = gcc -fsanitize=address
CFLAGS = -g -O2 ${WARNINGS}
CPPFLAGS =
LDFLAGS =
LDLIBS =

WARNINGS = -Werror -Wall -Wextra -Wformat -Wunused -Wc++-compat -Wmissing-declarations -Wredundant-decls

############################################################
#        No user servicable parts beyond this point.       #
############################################################

override CFLAGS += -std=c89 -D_POSIX_C_SOURCE=200809L
override CFLAGS += -MMD

.SUFFIXES:
.SECONDARY:
.DELETE_ON_ERROR:

hello.gen.run:
hello.x: hello.o builder.o bridge.o c89.o pool.o hashmap.o PMurHash.o

%.x: %.o
	${CC} ${LDFLAGS} $^ ${LDLIBS} -o $@
%.o: %.c
	${CC} ${CPPFLAGS} ${CFLAGS} -c -o $@ $<
%.c %.h: %.py
	./$<
%.gen.c: %.x
	./$< > $@
%.run: %.x
	./$<

clean:
	rm -f *.x *.o *.d
distclean: clean
	rm -f c89.h c89.c

-include *.d
