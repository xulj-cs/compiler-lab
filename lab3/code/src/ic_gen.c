#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "interCode.h"
#include "type.h"

#define RET_IC return IC_2_ICs(code)

int isDigit(char c){
	if('0'<=c&&c<='9')
		return 1;
	if(c=='+'||c=='-')
		return 1;
	return 0;
}

InterCodes *ic_gen_func_dec(const char *name){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = FUNC_DEC;
	code->name = name ;
	RET_IC;
}

InterCodes *ic_gen_arg(const char *name){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = ARG;
	code->op = search_operand(name);
	RET_IC;
	
}
static InterCodes *ic_gen_var(FieldList field){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = PARAM;
	if(isStruct(field->type)||isArray(field->type))
		code->op = new_operand(field->name,ADDRESS);
	else 
		code->op = new_operand(field->name,VARIABLE);
	RET_IC;
}
InterCodes *ic_gen_varlist(FieldList para){
	InterCodes *head = NULL;
	while(para){
		head = ICs_concat( 2,head,ic_gen_var(para) );
		para = para->tail;
	}
	return head;
}
InterCodes *ic_gen_assign_star(const char *left,const char *right){
	// l = *r
	if(!left)
		return NULL;
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = ASSIGN_STAR;
	
	code->assign.right = search_operand(right);
	assert(code->assign.right->kind == ADDRESS);
	code->assign.left = new_operand(left,VARIABLE);
	RET_IC;
}
InterCodes *ic_gen_assign_addr(const char *left,const char *right){
	// l = &r
	if(!left)
		return NULL;
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = ASSIGN_ADDR;
	
	code->assign.right = search_operand(right);
	assert(code->assign.right->kind == VARIABLE);
	code->assign.left = new_operand(left,ADDRESS);
	RET_IC;
}

InterCodes *ic_gen_assign(const char *left,const char *right){
	if(!left)
		return NULL;
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = ASSIGN;
	if(isDigit(right[0])){
		code->assign.left = new_operand(left,VARIABLE);
		code->assign.right = new_operand(right,CONSTANT);
	}
	else{
		code->assign.right = search_operand(right);
		code->assign.left = new_operand(left,code->assign.right->kind);
	}
	RET_IC;	
}

InterCodes *ic_gen_ari(const char *place,const char *t1,const char *t2,const const char *op){
	if(!place)
		return NULL;
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	if(strcmp(op,"PLUS")==0){
		code->kind = ADD;
	}
	else if(strcmp(op,"MINUS")==0){
		code->kind = SUB;
	}
	else if(strcmp(op,"STAR")==0){
		code->kind = MUL;
	}
	else if(strcmp(op,"DIV")==0){
		code->kind = DIV;
	}
	else
		assert(0);
	if(isDigit(t1[0]))
		code->binop.op1 = new_operand(t1,CONSTANT);
	else
		code->binop.op1 = search_operand(t1);
	if(isDigit(t2[0]))
		code->binop.op2 = new_operand(t2,CONSTANT);
	else
		code->binop.op2 = search_operand(t2);
	if(code->binop.op1->kind == ADDRESS || code->binop.op2->kind == ADDRESS)
		code->binop.result = new_operand(place,ADDRESS);
	else
		code->binop.result = new_operand(place,VARIABLE);
		
	RET_IC;
}
InterCodes * ic_gen_neg(const char *place,const char *t){
	if(!place)
		return NULL;
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = SUB;
	code->binop.op1 = new_operand("0",CONSTANT);
	code->binop.op2 = search_operand(t);
	code->binop.result = new_operand(place,VARIABLE);
	RET_IC;	
}

InterCodes *ic_gen_ret(const char *t){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = RET;
	code->name = t;
	RET_IC;
}

InterCodes *ic_gen_goto(const char *label){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = GOTO;
	code->name = label;
	RET_IC;
}
InterCodes *ic_gen_if(const char *t1,const char *t2,const char *op){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = IF;
	code->cond.left = search_operand(t1);
	code->cond.op = op;
	code->cond.right = search_operand(t2);
	RET_IC;
	
}
InterCodes *ic_gen_label(const char *label){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = LABEL;
	code->name = label;
	RET_IC;

}

InterCodes *ic_gen_read(const char *place){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = READ;
	code->op = new_operand(place,VARIABLE);
	RET_IC;
}
InterCodes *ic_gen_write(const char *place){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = WRITE;
	code->op = search_operand(place);
	RET_IC;
}
InterCodes *ic_gen_func_call(const char *place,const char *function){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = FUNC_CALL;
	code->func.place = new_operand(place,VARIABLE);
	code->func.func_name = function;
	RET_IC;
}
InterCodes *ic_gen_dec(const char *place,int size){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = DEC;
	code->dec.op = new_operand(place,VARIABLE);
	code->dec.size = size;
	RET_IC;
}
