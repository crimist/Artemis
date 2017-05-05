# Serpent Makefile

CFLAGS=-std=gnu99 -O2 -march=native -flto -fuse-ld=gold
DFLAGS=-DDEBUG -g -ggdb -Werror -Wshadow -Wall -Wextra -Wpedantic
RFLAGS=-s -static
# clang 3.9 actually has -flto working easy
CC=clang-3.9
FILENAME=artemis

debug: src/*.c
	${CC} ${CFLAGS} ${DFLAGS} -o ${FILENAME} src/*.c

release: src/*.c
	${CC} ${CFLAGS} ${RFLAGS} -o ${FILENAME} src/*.c

test: src/*.c
	cd test && \
		gcc ${CFLAGS} ${DFLAGS} ../src/*.c -fdump-tree-all && \
		rm a.out

clean:
	rm -f ${FILENAME}

