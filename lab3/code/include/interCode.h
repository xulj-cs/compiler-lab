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

	enum {ASSIGN , ADD , SUB , MUL , DIV ,FUNC_DEC, PARAM, ARG , RET, IF, GOTO , LABEL ,FUNC_CALL, READ ,WRITE} kind;
	union{
		struct {Operand left,right;} assign;
		struct {Operand op;}unop;
		struct {Operand result,op1,op2;}binop;
		struct {Operand left,right;char *op;}cond;
		struct {char *place,*func_name;}func;
		const char *name; //func_name var_name
	};
}InterCode;

typedef struct InterCodes {
	InterCode *code;
	struct InterCodes *prev,*next;
}InterCodes;
Operand new_operand(char *);
void print_IC(InterCode*);
InterCodes *IC_2_ICs(InterCode *);
InterCodes* ICs_concat(int,...);

#define CODE1 code1
#define CODE2 CODE1,code2
#define CODE3 CODE2,code3
#define CODE4 CODE2,code4
#define CODE5 CODE2,code5
#define CODE6 CODE2,code6


#define ICs_concat2 ICs_concat(2,code1,code2)
#define ICs_concat3 ICs_concat(3,code1,code2,code3)
#define ICs_concat4 ICs_concat(4,code1,code2,code3,code4)
#define ICs_concat5 ICs_concat(5,code1,code2,code3,code4,code5)
#define ICs_concat6 ICs_concat(6,code1,code2,code3,code4,code5,code6)
#define ICs_concat7 ICs_concat(7,code1,code2,code3,code4,code5,code6,code7)

#endif


