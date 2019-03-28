
CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

src = $(wildcard *.c)
src += $(wildcard ../*.c)
obj = $(src:.c=.o)
dep = $(obj:.o=.d) 

.PHONY: build all clean

build: example-01.exe example-02.exe example-03.exe

all: clean build

clean::
	rm -rf $(dep)
	rm -rf $(obj)
	rm -rf *.exe

	
example-01.exe: example-01.o ../tiny-json.o
	gcc $(CFLAGS) -o $@ $^

example-02.exe: example-02.o ../tiny-json.o
	gcc $(CFLAGS) -o $@ $^

example-03.exe: example-03.o ../tiny-json.o
	gcc $(CFLAGS) -o $@ $^	

-include $(dep)

%.d: %.c
	$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@