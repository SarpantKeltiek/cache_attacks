CFLAGS=
INC=-I../inc/

all: lib

lib: test_lib.c ../inc/test_lib.h
	$(CC) $(CFLAGS) $(INC) -c -fpic test_lib.c
	$(CC) --shared test_lib.o -o libfrproj.so

help:
	@echo "Usage: make or make lib to compile shared library test"
	@echo "  when testing executables you may need to do:"
	@echo "  LD_LIBRARY_PATH=`pwd`:$LD_LIBRARY_PATH"
	@echo "  OR export LD_LIBRARY_PATH=`pwd`:$LD_LIBRARY_PATH"

.PHONY: help