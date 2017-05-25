build:
	gcc -g -o test -mavx -O3 main.c

all: build
	./test
