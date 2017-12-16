#ifndef _INTERCODE_H_
#define _INTERCODE_H_

typedef struct Operand_* Operand;
struct Operand_{
	enum {VARIABLE , CONSTANT , ADDRESS , } kind;
	union {
		//int var_no;
		//int value;
		char *info;
	};
};

typedef struct InterCode {

	enum {ASSIGN , ADD , SUB , MUL , DIV ,FUN_DEC, VAR_DEC , RET} kind;
	union{
		struct {Operand left,right;} assign;
		struct {Operand op;}unop;
		struct {Operand result,op1,op2;}binop;
		const char *name; //func_name var_name
	};
}InterCode;

typedef struct InterCodes {
	InterCode *code;
	struct InterCodes *prev,*next;
}InterCodes;
Operand new_operand(char *,int);
void print_IC(InterCode*);
InterCodes* ICs_push(InterCodes *,InterCode*);
InterCodes* ICs_concat(int,...);
InterCodes* IC_concat(int,...);
#endif


