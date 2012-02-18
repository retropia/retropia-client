.PHONY : all releast debug test clean 
.SUFFIXES:

all: release

release:
	rake build:release

debug:
	rake build:debug

test:
	rake test:all
	rake gtest:all

%.c: FORCE
	rake test:$@

%.cc: FORCE
	rake gtest:path[$@]

clean:
	-rm -rf build/*

Makefile: ;

FORCE: