CC = gcc
CFLAGS = -O3 -std=c99 -Wall -pedantic

src = $(wildcard *.c)
src += $(wildcard ../*.c)
obj = $(src:.c=.o)
dep = $(obj:.o=.d) 

.PHONY: build all clean

build: test.exe

all: clean build

clean::
	rm -rf $(dep)
	rm -rf $(obj)
	rm -rf *.exe

test: test.exe
	./test.exe	

test.exe: $(obj)
	gcc $(CFLAGS) -o $@ $^	

-include $(dep)

%.d: %.c
	$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@
