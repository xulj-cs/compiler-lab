# global settings #

CC := gcc
LD := ld
FLEX := flex
BISON := bison

SRC := code/src/
INC := code/include/
OBJ := obj/

CFLAGS := -c -std=c99 -I $(INC)  -Wall #-Werror 
LDFLAGS := -lfl -ly

target :=  ./parser

all : $(target)

# rules for building the project #


FlexSrc := $(SRC)lexical.l
FlexObj := $(OBJ)lex.yy.c

BisonSrc := $(SRC)syntax.y
BisonObj := $(OBJ)syntax.tab
BisonObj_H := $(BisonObj).h
BisonObj_C := $(BisonObj).c
BisonObj_O := $(BisonObj).o

Bison_FLAGS := -d 
Bison_DEBUG := #-v 

SRCS := $(shell find $(SRC) -name "*.c")
OBJS := $(BisonObj_O) $(patsubst $(SRC)%.c,$(OBJ)%.o,$(SRCS)) 

$(target): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)  

$(BisonObj_O): $(FlexSrc) $(BisonSrc)
	@mkdir -p $(OBJ)
	$(FLEX) -o $(FlexObj) $(FlexSrc) 
	$(BISON) -o $(BisonObj_C) $(BisonSrc) $(Bison_FLAGS) $(Bison_DEBUG)  
	$(CC) -o $@ $(BisonObj_C) -c -I $(INC)
$(OBJ)%.o:$(SRC)%.c
	@mkdir -p $(@D)
	$(CC) -o $@ $< $(CFLAGS)

# some convinient rules #
.PHONY: clean test test-all clean-temp

TESTFILE := test.c
test: all
	$(target) $(TESTFILE)
test-all: all
	for testfile in $(shell find ./pretest/*.txt); \
	do \
		echo $$testfile; \
		$(target) $$testfile; \
	done

clean-temp:
	-rm -rf $(OBJ)

clean:clean-temp
	-rm -f $(target)
