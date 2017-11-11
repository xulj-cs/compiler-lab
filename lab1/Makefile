# global settings #

CC := gcc
LD := ld

CFLAGS := -std=c99 -I code/
LDFLAGS := -lfl -ly

all: Parser

# rules for building the project #

FLEX := flex
FlexSrc := code/lexical.l
FlexObj := obj/lex.yy.c

BISON := bison
BisonSrc := code/syntax.y
BisonObj = $(BisonObj_H) $(BisonObj_C) 
BisonObj_H := obj/syntax.tab.h
BisonObj_C := obj/syntax.tab.c
Bison_FLAGS := -d 
Bison_DEBUG := 

Parser_BIN := ./parser


Parser: $(Parser_BIN) 
$(Parser_BIN): obj/syntax.tab.o obj/node.o obj/main.o 
	$(CC) -o $@ $^ $(LDFLAGS)  

obj/syntax.tab.o: $(FlexSrc) $(BisonSrc)
	@mkdir -p obj/
	$(FLEX) -o $(FlexObj) $(FlexSrc) 
	$(BISON) -o $(BisonObj_C) $(Bison_FLAGS) $(Bison_DEBUG)  $(BisonSrc) 
	$(CC) $(CFLAGS)	-c -o $@ $(BisonObj_C)
obj/node.o:	code/node.c
	@mkdir -p obj/
	$(CC) $(CFLAGS)	-c -o $@ $^
obj/main.o:	code/main.c
	@mkdir -p obj/
	$(CC) $(CFLAGS)	-c -o $@ $^

	


# some convinient rules #
.PHONY: clean test test-all clean-temp

TESTFILE := test.txt
test: all
	$(Parser_BIN) $(TESTFILE) 
test-all: all
	for testfile in $(shell find ./pretest/*); \
	do \
		echo $$testfile; \
		$(Parser_BIN) $$testfile; \
	done

clean-temp:
	-rm -rf obj

clean:clean-temp
	-rm -f $(Parser_BIN)
