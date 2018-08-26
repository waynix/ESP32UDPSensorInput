libkeccak-tiny.o: keccak-tiny.c
	gcc -march=native -std=c11 -Wextra -Wpedantic -Wall  keccak-tiny.c -D'memset_s(A,B,C,D)=memset(A,C,D)' -c  -o libkeccak-tiny.o
udp2: libkeccak-tiny.o udp2.c
	gcc udp2.c libkeccak-tiny.o -o udp2

clean: 
	libkeccak-tiny.o udp2

all: udp2
