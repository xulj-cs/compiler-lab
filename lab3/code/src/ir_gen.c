#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "node.h"
#include "interCode.h"
#include "symtable.h"

InterCodes * ICROOT = NULL;

//for debugging...
#define RET_IC return IC_2_ICs(code)

static int temp_no = 0;
static int label_no = 0;
char *new_temp(){
	// __t[x]
	char *str = (char *)malloc(sizeof(char)*7);
	sprintf(str,"__t%d",temp_no);
	temp_no++;
	return str;
}
char *new_label(){
	// __l[x]
	char *str = (char *)malloc(sizeof(char)*7);
	sprintf(str,"__l%d",label_no);
	label_no++;
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

InterCodes *ic_gen_func_dec(char *name){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = FUNC_DEC;
	code->name = name ;
	RET_IC;
}

static InterCodes *ic_gen_arg(const char *name){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = ARG;
	code->name = name;
	RET_IC;
	
}
static InterCodes *ic_gen_var(const char *name){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = PARAM;
	code->name = name;
	RET_IC;
}
InterCodes *ic_gen_varlist(FieldList para){
	InterCodes *head = NULL;
	while(para){
		//head = ICs_push(head,ic_gen_var(para->name));
		head = ICs_concat( 2,head,ic_gen_var(para->name) );
		para = para->tail;
	}
	return head;
}

InterCodes *ic_gen_assign(char *place,char *info){
	// flag = 0 ->int
	// flag = 1 ->id
	if(!place)
		return NULL;
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = ASSIGN;
	code->assign.left = new_operand(place);
    code->assign.right = new_operand(info);
	RET_IC;	
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
	code->binop.result = new_operand(place);
	code->binop.op1 = new_operand(t1);
	code->binop.op2 = new_operand(t2);
	RET_IC;
}
InterCodes * ic_gen_neg(char *place,char *t){
	if(!place)
		return NULL;
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = SUB;
	code->binop.result = new_operand(place);
	code->binop.op1 = new_operand("0");
	code->binop.op2 = new_operand(t);
	RET_IC;	
}

InterCodes *ic_gen_ret(char *t){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = RET;
	code->name = t;
	RET_IC;
}

InterCodes *ic_gen_goto(char *label){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = GOTO;
	code->name = label;
	RET_IC;
}
InterCodes *ic_gen_if(char *t1,char *t2,char *op){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = IF;
	code->cond.left = new_operand(t1);
	code->cond.op = op;
	code->cond.right = new_operand(t2);
	RET_IC;
	
}
InterCodes *ic_gen_label(char *label){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = LABEL;
	code->name = label;
	RET_IC;

}

InterCodes *ic_gen_read(char *place){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = READ;
	code->name = place;
	RET_IC;
}
InterCodes *ic_gen_write(char *place){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = WRITE;
	code->name = place;
	RET_IC;
}
InterCodes *ic_gen_func_call(char *place,char *function){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = FUNC_CALL;
	code->func.place = place;
	code->func.func_name = function;
	RET_IC;
}
static InterCodes *translate_Exp(Node *p,char *place);

static InterCodes *translate_Cond(Node *p,char *label_true,char *label_false){
	if(p->num_of_child == 3)
	{
		if(strcmp(p->child[1]->symbol,"RELOP")==0){
			char *t1 = new_temp();
			char *t2 = new_temp();
			InterCodes *code1 = translate_Exp(p->child[0],t1);
			InterCodes *code2 = translate_Exp(p->child[2],t2);
			char *op = p->child[1]->lexeme;
			InterCodes *code3 = ic_gen_if(t1,t2,op);
			InterCodes *code4 = ic_gen_goto(label_true);
			InterCodes *code5 = ic_gen_goto(label_false);
			return ICs_concat(5,code1,code2,code3,code4,code5);
		}
		else if(strcmp(p->child[1]->symbol,"AND")==0){
			char *label = new_label();
			InterCodes *code1 = translate_Cond(p->child[0],label,label_false);
			InterCodes *code2 = ic_gen_label(label);
			InterCodes *code3 = translate_Cond(p->child[2],label_true,label_false);
			return ICs_concat(3,code1,code2,code3);
		}
		else if(strcmp(p->child[1]->symbol,"OR")==0){
			char *label = new_label();
			InterCodes *code1 = translate_Cond(p->child[0],label_true,label);
			InterCodes *code2 = ic_gen_label(label);
			InterCodes *code3 = translate_Cond(p->child[2],label_true,label_false);
			return ICs_concat(3,code1,code2,code3);
		}
	}
	if(p->num_of_child == 2){
		if(strcmp(p->child[0]->symbol,"NOT")==0){
			return translate_Cond(p->child[1],label_false,label_true);
		}	
	}
	
	// rest of cases
	{
		char *t1 = new_temp();
		InterCodes *code1 = translate_Exp(p,t1);
		InterCodes *code2 = ic_gen_if(t1,"0","!=");
		InterCodes *code3 = ic_gen_goto(label_true);
		InterCodes *code4 = ic_gen_goto(label_false);
		return ICs_concat(4,code1,code2,code3,code4);
	
	}

}
static InterCodes *translate_FunDec(Node *p){
	//FunDec -> ID( VarList )
	//			| ID ()
	char *func_name = p->child[0]->lexeme;
	ParaList_Ret para_ret;
	searchSymTable(func_name,Function,(void **)&para_ret,1);
		
	InterCodes *code1 = ic_gen_func_dec(func_name);
	//print_ICs(code1);
	InterCodes *code2 = ic_gen_varlist(para_ret->tail);
	//print_ICs(code2);
	return ICs_concat(2,code1,code2);

}

static InterCodes *translate_Args(Node *p,char **arg_list){
	if(p->num_of_child==1){
		char *t = new_temp();
		InterCodes *code1 = translate_Exp(p->child[0],t);
		*arg_list = t;
		return code1;
	}
	else/* if(p->num_of_child==3)*/{
		char *t = new_temp();
		InterCodes *code1 = translate_Exp(p->child[0],t);
		*arg_list = t;
		InterCodes *code2 = translate_Args(p->child[2],arg_list+1);
		return ICs_concat(2,code1,code2);
	}

}

static InterCodes *translate_Exp(Node *p,char *place){
	switch(p->num_of_child){
		case 1:
		{
		   if(strcmp(p->child[0]->symbol,"INT")==0){
				char *value = p->child[0]->lexeme;
				return ic_gen_assign(place,value);
		   }
		   if(strcmp(p->child[0]->symbol,"ID")==0){
				char *name = p->child[0]->lexeme;
				return ic_gen_assign(place,name);
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
					InterCodes *code2 = ic_gen_assign(variable,t1);
					InterCodes *code3 = ic_gen_assign(place,variable);
					return ICs_concat(3,code1,code2,code3);
				}	   
			    if(strcmp(op,"PLUS")==0 \
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
			else if(strcmp(p->child[0]->symbol,"ID")==0){
				char *function = p->child[0]->lexeme;
				if(strcmp(function,"read")==0){
					return ic_gen_read(place);
				}
				else{
					return ic_gen_func_call(place,function);
				}
			}
		}
		case 4:
		{
			if(strcmp(p->child[0]->symbol,"ID")==0){
				char *function = p->child[0]->lexeme;
				int num = num_of_args(function);
				assert(num>=1);
				char **arg_list = (char **)malloc(sizeof(char*)*num);
				InterCodes *code1 = translate_Args(p->child[2],arg_list);
				InterCodes *code2 = NULL;
				if(strcmp(function,"write")==0){
					code2 = ic_gen_write(arg_list[0]);
					return ICs_concat(2,code1,code2);
				}
				else{
					for (int i=num-1;i>=0;i--){
						code2 = ICs_concat(2,code2,ic_gen_arg(arg_list[i]));	
					}
					InterCodes *code3 = ic_gen_func_call(place,function);
					return	ICs_concat(3,code1,code2,code3);
				}
			}
		}
	}
	// rest of case 
	{
		char *label1 = new_label();
		char *label2 = new_label();
		InterCodes *code1 = ic_gen_assign(place,"0");
		InterCodes *code2 = translate_Cond(p,label1,label2);
		InterCodes *code3 = ic_gen_label(label1);
		InterCodes *code4 = ic_gen_assign(place,"1");
		InterCodes *code5 = ic_gen_label(label2);
		return ICs_concat(5,code1,code2,code3,code4,code5);	
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
		case 5:
		{
			if(strcmp(p->child[0]->symbol,"IF")==0){
				char *label1 = new_label();
				char *label2 = new_label();
				InterCodes *code1 = translate_Cond(p->child[2],label1,label2);
				InterCodes *code2 = ic_gen_label(label1);
				InterCodes *code3 = translate_Stmt(p->child[4]);
				InterCodes *code4 = ic_gen_label(label2);
				return ICs_concat(4,code1,code2,code3,code4);

			}
			else if(strcmp(p->child[0]->symbol,"WHILE")==0){
				char *label1 = new_label();
				char *label2 = new_label();
				char *label3 = new_label();
				InterCodes *code1 = ic_gen_label(label1);
				InterCodes *code2 = translate_Cond(p->child[2],label2,label3);
				InterCodes *code3 = ic_gen_label(label2);
				InterCodes *code4 = translate_Stmt(p->child[4]);
				InterCodes *code5 = ic_gen_goto(label1);
				InterCodes *code6 = ic_gen_label(label3);
				return ICs_concat(6,code1,code2,code3,code4,code5,code6);
			}
		}
		case 7:
		{
			char *label1 = new_label();
			char *label2 = new_label();
			char *label3 = new_label();
			InterCodes *code1 = translate_Cond(p->child[2],label1,label2);
			InterCodes *code2 = ic_gen_label(label1);
			InterCodes *code3 = translate_Stmt(p->child[4]);
			InterCodes *code4 = ic_gen_goto(label3);
			InterCodes *code5 = ic_gen_label(label2);
			InterCodes *code6 = translate_Stmt(p->child[6]);
			InterCodes *code7 = ic_gen_label(label3);
			return ICs_concat(7,code1,code2,code3,code4,code5,code6,code7);			
		}
		default : return NULL;
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

