
CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

build: example-01.exe example-02.exe example-03.exe

clean:
	rm -rf *.o
	rm -rf *.exe

all: clean build

test: test.exe
	./test.exe
	
example-01.exe: example-01.o tiny-json.o
	gcc $(CFLAGS) -o $@ $^

example-02.exe: example-02.o tiny-json.o
	gcc $(CFLAGS) -o $@ $^

example-03.exe: example-03.o tiny-json.o
	gcc $(CFLAGS) -o $@ $^

test.exe: tests.o tiny-json.o
	gcc $(CFLAGS) -o $@ $^	
