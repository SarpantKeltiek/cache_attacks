CC=gcc
CFLAGS=
CPPFLAGS=-I$(INC)
LIBFLAGS=-Llib
LDFLAGS=-lfrproj
INC=inc

all: lib victim flushreload perf

lib:
	cd lib/ && $(MAKE)
	cd ..
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib/

perf: performance.c
	$(CC) $(CFLAGS) -o $@ $<

script: script.c
	$(CC) $(CFLAGS) -o $@ $<

victim: fr_utils.o victim.o
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $@ $^ $(LDFLAGS)

flushreload: fr_utils.o main.o 
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $@ $^ $(LDFLAGS)

fr_utils.o: src/fr_utils.c $(INC)/fr_utils.h $(INC)/test_lib.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

main.o: src/main.c $(INC)/fr_utils.h $(INC)/test_lib.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

victim.o: src/victim.c $(INC)/fr_utils.h $(INC)/test_lib.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

help:
	@echo "usage: "

clean:
	rm -f *.o flushreload victim perf

.PHONY: help clean lib
