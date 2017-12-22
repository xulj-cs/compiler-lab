#ifndef _INTERCODE_H_
#define _INTERCODE_H_

typedef struct Operand_* Operand;
struct Operand_{
	enum {VARIABLE , CONSTANT , ADDRESS  } kind;
	union {
		
		const char *info;
		
	};
};

typedef struct InterCode {

	enum {ASSIGN , ASSIGN_ADDR,ASSIGN_STAR,STAR_ASSIGN,ADD , SUB , MUL , DIV ,FUNC_DEC, PARAM, ARG , RET, IF, GOTO , LABEL ,FUNC_CALL, READ ,WRITE , DEC} kind;
	union{
		struct {Operand left,right;} assign;
		struct {Operand op;}unop;
		struct {Operand result,op1,op2;}binop;
		struct {Operand left,right;const char *op;}cond;
		struct {Operand place;const char *func_name;}func;
		struct {Operand op;int size;}dec;
		const char *name; //func_name var_name
		Operand op;
	};
}InterCode;

typedef struct InterCodes {
	InterCode *code;
	struct InterCodes *prev,*next;
}InterCodes;

Operand new_operand(const char *,int);
void print_IC(InterCode*);
InterCodes *IC_2_ICs(InterCode *);
InterCodes* ICs_concat(int,...);
InterCodes *ICs_pop_back(InterCodes*);
Operand search_operand(const char *);
#endif


