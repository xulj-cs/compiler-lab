#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "node.h"
#include "interCode.h"
#include "symtable.h"

InterCodes * ICROOT = NULL;

//for debugging...

static int temp_no = 0;
char *new_temp(){
	// __t[x]
	char *str = (char *)malloc(sizeof(char)*5);
	sprintf(str,"__t%d",temp_no);
	temp_no++;
	return str;
}
void print_ICs(InterCodes *head){
    if(!head){
		printf("NULL\n");
		return;
	}
	printf("begin...\n");
	InterCodes* p = head;
    while(true){
        print_IC(p->code);
		p = p->next;
		if(p==head)
			break;
    }
	printf("over...\n");
}

InterCodes *ic_gen_fun_dec(char *name){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = FUN_DEC;
	code->name = name ;
	return IC_concat(1,code);
}

static InterCode *ic_gen_var(const char *name){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = VAR_DEC;
	code->name = name;
	return code;
}
InterCodes *ic_gen_varlist(FieldList para){
	InterCodes *head = NULL;
	while(para){
		head = ICs_push(head,ic_gen_var(para->name));
		para = para->tail;
	}
	return head;
}

InterCodes *ic_gen_assign(char *place,char *info,int flag){
	// flag = 0 ->int
	// flag = 1 ->id
	if(!place)
		return NULL;
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = ASSIGN;
	code->assign.left = new_operand(place,1);
    code->assign.right = new_operand(info,flag);
	return IC_concat(1,code);	
}

InterCodes *ic_gen_ari(char *place,char *t1,char *t2,const char *op){
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
	code->binop.result = new_operand(place,1);
	code->binop.op1 = new_operand(t1,1);
	code->binop.op2 = new_operand(t2,1);
	return IC_concat(1,code);
}
InterCodes * ic_gen_neg(char *place,char *t){
	if(!place)
		return NULL;
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = SUB;
	code->binop.result = new_operand(place,1);
	code->binop.op1 = new_operand("0",0);
	code->binop.op2 = new_operand(t,1);
	return IC_concat(1,code);	
}

InterCodes *ic_gen_ret(char *t){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = RET;
	code->name = t;
	return IC_concat(1,code);
}
static InterCodes *translate_FunDec(Node *p){
	//FunDec -> ID( VarList )
	//			| ID ()
	char *func_name = p->child[0]->lexeme;
	ParaList_Ret para_ret;
	searchSymTable(func_name,Function,(void **)&para_ret,1);
		
	InterCodes *code1 = ic_gen_fun_dec(func_name);
	//print_ICs(code1);
	InterCodes *code2 = ic_gen_varlist(para_ret->tail);
	//print_ICs(code2);
	return ICs_concat(2,code1,code2);

}


static InterCodes *translate_Exp(Node *p,char *place){
	switch(p->num_of_child){
		case 1:
		{
		   if(strcmp(p->child[0]->symbol,"INT")==0){
				char *value = p->child[0]->lexeme;
				return ic_gen_assign(place,value,0);
		   }
		   else if(strcmp(p->child[0]->symbol,"ID")==0){
				char *name = p->child[0]->lexeme;
				return ic_gen_assign(place,name,1);
		   }
		}
		case 2:
		{
			if(strcmp(p->child[0]->symbol,"MINUS")==0){
				char *t1 = new_temp();
				InterCodes *code1 = translate_Exp(p->child[1],t1);
				InterCodes *code2 = ic_gen_neg(place,t1);
				return ICs_concat(2,code1,code2);
			}
		}
		case 3:
		{
        	if(strcmp(p->child[0]->symbol,"Exp")==0 && strcmp(p->child[2]->symbol,"Exp")==0){
				const char *op = p->child[1]->symbol;
				if(strcmp(op,"ASSIGNOP")==0){
					char *variable = p->child[0]->child[0]->lexeme;
					char *t1 = new_temp();
					InterCodes *code1 = translate_Exp(p->child[2],t1);
					InterCodes *code2 = ic_gen_assign(variable,t1,1);
					InterCodes *code3 = ic_gen_assign(place,variable,1);
					return ICs_concat(3,code1,code2,code3);
				}	   
			    else if(strcmp(op,"PLUS")==0 \
                    || strcmp(op,"MINUS")==0 \
                    || strcmp(op,"STAR")==0 \
                    || strcmp(op,"DIV")==0 ){
				
					char *t1 = new_temp();
					char *t2 = new_temp();
					InterCodes *code1 = translate_Exp(p->child[0],t1);
					InterCodes *code2 = translate_Exp(p->child[2],t2);
					InterCodes *code3 = ic_gen_ari(place,t1,t2,op);
					return ICs_concat(3,code1,code2,code3);
				}

			}
		}
	}
	return NULL;
}

static InterCodes *translate_CompSt(Node *p);
static InterCodes *translate_Stmt(Node *p){
	switch(p->num_of_child){
		case 1:return translate_CompSt(p->child[0]);
		case 2:return translate_Exp(p->child[0],NULL);
		case 3:
		{
			char *t1 = new_temp();
			InterCodes *code1 = translate_Exp(p->child[1],t1);
			InterCodes *code2 = ic_gen_ret(t1);
			return ICs_concat(2,code1,code2);
		}
//		case 3:
	}

}
static InterCodes *translate_StmtList(Node *p){
	// StmtList -> Stmt StmtList 
	//		| epsilon
	if(!p)
		return NULL;
	InterCodes *code1 = translate_Stmt(p->child[0]);
	InterCodes *code2 = translate_StmtList(p->child[1]);
	return ICs_concat(2,code1,code2);
}
static InterCodes *translate_CompSt(Node *p){
	// CompSt -> { DefList StmtList}
	return translate_StmtList(p->child[2]);
}
void IR_Gen(Node *p){
	// ExtDef -> Specifier FunDec CompSt

	if(strcmp(p->child[1]->symbol,"FunDec")==0){
		InterCodes* code1 = translate_FunDec(p->child[1]);
		InterCodes* code2 = translate_CompSt(p->child[2]);
		InterCodes* code = ICs_concat(2,code1,code2);
		ICROOT = ICs_concat(2,ICROOT,code);
	}
}


void print_ICROOT(){
	printf("*********************************\n");
   	InterCodes* p = ICROOT;
    while(true){
        print_IC(p->code);
		p = p->next;
		if(p==ICROOT)
			break;
    }
}

