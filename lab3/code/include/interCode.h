#ifndef _INTERCODE_H_
#define _INTERCODE_H_

typedef struct Operand_* Operand;
struct Operand_{
	enum {VARIABLE , CONSTANT , ADDRESS} kind;
	union {
		int var_no;
		int value;
	};
};

typedef struct InterCode {

	enum {ASSIGN , ADD , SUB , MUL} kind;
	union{
		struct {Operand left,right;} assign;
		struct {Operand result,op1,op2;}binop;
	};
}InterCode;

typedef struct InterCodes {
	InterCode code;
	struct InterCodes *prev,*next;
}InterCodes;

void print_ICs();

#endif


