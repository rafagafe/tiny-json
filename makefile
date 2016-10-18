

build: example-01.exe example-02.exe

clean:
	rm -rf *.o
	rm -rf *.exe

all: clean build

example-01.exe: example-01.o tiny-json.o
	gcc -o example-01.exe example-01.o tiny-json.o

example-02.exe: example-02.o tiny-json.o
	gcc -o example-02.exe example-02.o tiny-json.o

tiny-json.o: tiny-json.c tiny-json.h
	gcc -c tiny-json.c

example-01.o: example-01.c tiny-json.h
	gcc -c example-01.c

example-02.o: example-02.c tiny-json.h
	gcc -c example-02.c
