

build: example1.exe example2.exe

clean:
	rm -rf *.o
	rm -rf *.exe

all: clean build

example1.exe: example1.o tiny-json.o
	gcc -o example1.exe example1.o tiny-json.o

example2.exe: example2.o tiny-json.o
	gcc -o example2.exe example2.o tiny-json.o

tiny-json.o: tiny-json.c tiny-json.h
	gcc -c tiny-json.c

example1.o: example1.c tiny-json.h
	gcc -c example1.c

example2.o: example2.c tiny-json.h
	gcc -c example2.c
