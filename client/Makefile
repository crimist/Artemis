# Artemis Makefile

# LTO (Link time optimization?) It's really annoying to set up compiling for it so I'll leave it off
#CFLAGS=-std=gnu99 -O2 -march=native -flto -fuse-ld=gold
#CC=clang-3.9

# Wireshark filter for scanner: tcp.port == 23 && tcp.flags.syn == 1 && ! tcp.analysis.retransmission && ! icmp


# Non LTO
CFLAGS=-std=gnu99 -O2 -march=native
DFLAGS=-DDEBUG -g -ggdb -Werror -Wshadow -Wall -Wextra -Wpedantic
RFLAGS=-s -static
CC=clang
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

