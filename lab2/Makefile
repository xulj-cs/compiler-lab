# global settings #

CC := gcc
LD := ld
FLEX := flex
BISON := bison

SRC := code/src
INC := code/include
OBJ := obj

CFLAGS := -std=c99 -I $(INC)/
LDFLAGS := -lfl -ly


all: Parser

# rules for building the project #


FlexSrc := $(SRC)/lexical.l
FlexObj := $(OBJ)/lex.yy.c

BisonSrc := $(SRC)/syntax.y
BisonObj = $(BisonObj_H) $(BisonObj_C) 
BisonObj_H := $(OBJ)/syntax.tab.h
BisonObj_C := $(OBJ)/syntax.tab.c

Bison_FLAGS := -d 
Bison_DEBUG := 

Parser_BIN := ./parser


Parser: $(Parser_BIN) 
$(Parser_BIN): $(OBJ)/syntax.tab.o $(OBJ)/node.o $(OBJ)/symtable.o $(OBJ)/main.o 
	$(CC) -o $@ $^ $(LDFLAGS)  

$(OBJ)/syntax.tab.o: $(FlexSrc) $(BisonSrc)
	@mkdir -p $(OBJ)/
	$(FLEX) -o $(FlexObj) $(FlexSrc) 
	$(BISON) -o $(BisonObj_C) $(Bison_FLAGS) $(Bison_DEBUG)  $(BisonSrc) 
	$(CC) $(CFLAGS)	-c -o $@ $(BisonObj_C)
$(OBJ)/node.o:	$(SRC)/node.c
	@mkdir -p $(OBJ)/
	$(CC) $(CFLAGS)	-c -o $@ $^
$(OBJ)/symtable.o: $(SRC)/symtable.c
	@mkdir -p $(OBJ)/
	$(CC) $(CFLAGS)	-c -o $@ $^
$(OBJ)/main.o:	$(SRC)/main.c
	@mkdir -p $(OBJ)/
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
	-rm -rf $(OBJ)

clean:clean-temp
	-rm -f $(Parser_BIN)
