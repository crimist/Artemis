# Artemis Makefile
# Wireshark filter for scanner: tcp.port == 23 && tcp.flags.syn == 1 && ! tcp.analysis.retransmission && ! icmp

CFLAGS=-std=gnu99
DFLAGS=-DDEBUG -g -ggdb -Weverything
RFLAGS=-s -w -static -Os
CC=clang
FILENAME=artemis

debug: src/*.c
	${CC} ${CFLAGS} ${DFLAGS} -o ${FILENAME}.out src/*.c

release: src/*.c
	mkdir -p build
	${CC} ${CFLAGS} ${RFLAGS} -o build/${FILENAME}.x86_64 src/*.c
	${CC} ${CFLAGS} ${RFLAGS} -m32 -o build/${FILENAME}.x86 src/*.c

compress: build/*
	upx -9 --ultra-brute build/*
	du -sh build/*

installcc:
	# Bunch of wget stuff

clean:
	rm -f ${FILENAME}
	rm -rf build/
