# global settings #

CC := gcc
LD := ld

CFLAGS := 

all: Scanner

# rules for building the project #

FLEX := flex
FlexSrc := ./lexical.l
FlexObj := ./lex.yy.c

Scanner_LDFLAGS := -lfl
Scanner_BIN := ./scanner

TESTFILE := test.txt

Scanner: $(Scanner_BIN)
$(Scanner_BIN): $(FlexSrc)
	$(FLEX) $(FlexSrc)
	$(CC) -o $@ $(FlexObj) $(Scanner_LDFLAGS)  

.PHONY: clean test

test: all
	$(Scanner_BIN) $(TESTFILE)
clean:
	-rm -f $(FlexObj) $(Scanner_BIN)
