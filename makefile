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
	
tiny-json.o: tiny-json.c tiny-json.h
	gcc $(CFLAGS) -c tiny-json.c

example-01.o: example-01.c tiny-json.h
	gcc $(CFLAGS) -c example-01.c

example-02.o: example-02.c tiny-json.h
	gcc $(CFLAGS) -c example-02.c
	
example-03.o: example-03.c tiny-json.h
	gcc $(CFLAGS) -c example-03.c

tests.o: tests.c tiny-json.h
	gcc $(CFLAGS) -c tests.c	
