# Artemis Makefile
# Wireshark filter for scanner: tcp.port == 23 && tcp.flags.syn == 1 && ! tcp.analysis.retransmission && ! icmp

# LTO (Link time optimization?) It's really annoying to set up compiling for it so I'll leave it off
#CFLAGS=-std=gnu99 -O2 -march=native -flto -fuse-ld=gold
#CC=clang-3.9

# Non LTO
CFLAGS=-std=gnu99 -O3
DFLAGS=-DDEBUG -g -ggdb -Weverything -ferror-limit=0 -ftemplate-backtrace-limit=0
RFLAGS=-s -static
CC=clang
FILENAME=artemis

debug: src/*.c
	${CC} ${CFLAGS} ${DFLAGS} -o ${FILENAME} src/*.c

release: src/*.c
	mkdir -p build
	${CC} ${CFLAGS} ${RFLAGS} -o build/${FILENAME}.x86_64 src/*.c
	${CC} ${CFLAGS} ${RFLAGS} -m32 -o build/${FILENAME}.x86 src/*.c

installcc:
	# Bunch of wget stuff

clean:
	rm -f ${FILENAME}
