CC=gcc
CFLAGS= -D_REENTRANT
CPPFLAGS=-I$(INC)
LIBFLAGS=-Llib
LDFLAGS=-lprproj -lpthread
INC=inc

all: lib victim prefetchreload

lib: 
	@cd lib/ && $(MAKE)

victim: src/victim.c $(INC)/test_lib.h 
	$(CC) $(CFLAGS) $(LIBFLAGS) $(CPPFLAGS) -o $@ $< $(LDFLAGS)

prefetchreload: pr_utils.o prefetch.o 
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $@ $^ $(LDFLAGS)

pr_utils.o: src/pr_utils.c $(INC)/pr_utils.h $(INC)/test_lib.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

prefetch.o: src/prefetch.c $(INC)/pr_utils.h $(INC)/test_lib.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

help:
	@echo "usage: "

clean:
	rm -f *.o prefetchreload victim

.PHONY: help clean lib
