# global settings #

CC := gcc
LD := ld

CFLAGS := -std=c99 
LDFLAGS := -lfl -ly

all: Parser

# rules for building the project #

FLEX := flex
FlexSrc := ./lexical.l
FlexObj := ./lex.yy.c

BISON := bison
BisonSrc := ./syntax.y
BisonObj = $(BisonObj_H) $(BisonObj_C) 
BisonObj_H := ./syntax.tab.h
BisonObj_C := ./syntax.tab.c
Bison_FLAGS := -d 
Bison_DEBUG := -v

Parser_BIN := ./parser


Parser: $(Parser_BIN) 
$(Parser_BIN): syntax.tab.o node.o main.o 
	$(CC) -o $@ $^ $(LDFLAGS)  

syntax.tab.o: $(FlexSrc) $(BisonSrc)
	$(FLEX) -o $(FlexObj) $(FlexSrc) 
	$(BISON) -o $(BisonObj_C) $(Bison_FLAGS) $(Bison_DEBUG)  $(BisonSrc) 
	$(CC) $(CFLAGS)	-c -o $@ $(BisonObj_C)


# some convinient rules #
.PHONY: clean test

TESTFILE := ./pretest/11.txt
test: all
	$(Parser_BIN) $(TESTFILE) 
test-all: all
	for testfile in $(shell find ./pretest/*); \
	do \
		echo $$testfile; \
		$(Parser_BIN) $$testfile; \
	done
clean:
	-rm -f $(FlexObj) $(BisonObj)
	-rm -f *.o
	-rm -f $(Parser_BIN)
