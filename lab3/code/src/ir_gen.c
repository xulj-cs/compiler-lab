#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
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
	sprintf(str,"t%d",temp_no);
	temp_no++;
	return str;
}
char *new_label(){
	// __l[x]
	char *str = (char *)malloc(sizeof(char)*7);
	sprintf(str,"l%d",label_no);
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
	//TODO
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = ARG;
	code->name = name;
	RET_IC;
	
}
static InterCodes *ic_gen_var(FieldList field){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = PARAM;
	if(isStruct(field->type)||(int)isArray(field->type))
		code->op = new_operand(field->name,ADDRESS);
	else 
		code->op = new_operand(field->name,VARIABLE);
	RET_IC;
}
InterCodes *ic_gen_varlist(FieldList para){
	InterCodes *head = NULL;
//	InterCodes *head2 = NULL;
	while(para){
//		if(isStruct(para->type)){
//			head2 = ICs_concat(2,head2,ic_gen_assign_star(para->name,para->name));
//		}
		head = ICs_concat( 2,head,ic_gen_var(para) );
		para = para->tail;
	}
	return head;
}
static InterCodes *ic_gen_assign_star(char *left,char *right){
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
static InterCodes *ic_gen_assign_addr(char *left,char *right){
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

InterCodes *ic_gen_assign(char *left,char *right){
	// flag = 0 ->int
	// flag = 1 ->id
	if(!left)
		return NULL;
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = ASSIGN;
	if(isdigit(right[0])){
		code->assign.left = new_operand(left,VARIABLE);
		code->assign.right = new_operand(right,CONSTANT);
	}
	else{
		code->assign.right = search_operand(right);
		code->assign.left = new_operand(left,code->assign.right->kind);
	}
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
	else
		assert(0);
	if(isdigit(t1[0]))
		code->binop.op1 = new_operand(t1,CONSTANT);
	else
		code->binop.op1 = search_operand(t1);
	if(isdigit(t1[0]))
		code->binop.op2 = new_operand(t2,CONSTANT);
	else
		code->binop.op2 = search_operand(t2);
	if(code->binop.op1->kind == ADDRESS || code->binop.op2->kind == ADDRESS)
		code->binop.result = new_operand(place,ADDRESS);
	else
		code->binop.result = new_operand(place,VARIABLE);
		
	RET_IC;
}
InterCodes * ic_gen_neg(char *place,char *t){
	if(!place)
		return NULL;
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = SUB;
	code->binop.op1 = new_operand("0",CONSTANT);
	code->binop.op2 = search_operand(t);
	code->binop.result = new_operand(place,VARIABLE);
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
	code->cond.left = search_operand(t1);
	code->cond.op = op;
	code->cond.right = search_operand(t2);
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
	code->op = new_operand(place,VARIABLE);
	RET_IC;
}
InterCodes *ic_gen_write(char *place){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = WRITE;
	code->op = search_operand(place);
	RET_IC;
}
InterCodes *ic_gen_func_call(char *place,char *function){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = FUNC_CALL;
	code->func.place = search_operand(place);
	code->func.func_name = function;
	RET_IC;
}
InterCodes *ic_gen_dec(char *place,int size){
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = DEC;
	code->dec.op = new_operand(place,VARIABLE);
	code->dec.size = size;
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
	InterCodes *code2 = ic_gen_varlist(para_ret->tail);
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
char *int2string(int n){
	int digit = 0;
	int t = n;
	while(n){
		digit++;
		n = n/10;
	}
	char *ret = (char *)malloc(sizeof(char)*(digit+1));
	sprintf(ret,"%d",t);
	return ret;
}

static InterCodes *translate_Exp(Node *p,char *place){
	switch(p->num_of_child){
		case 1:
		{
		   if(!place)
			   return NULL;
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
			if(!place)
			   return NULL;
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
					//Exp = Exp 
					char *t1 = new_temp();
					char *t2 = new_temp();
					InterCodes *code1 = translate_Exp(p->child[0],t1);
					InterCodes *code2 = translate_Exp(p->child[2],t2);
					InterCodes *code3 = code1->prev;
					code1 = ICs_pop_back(code1);
					code3->prev = code3->next = code3;
					if(code3->code->kind == ASSIGN_STAR){
						code3->code->kind = STAR_ASSIGN;
					}
					code3->code->assign.left = code3->code->assign.right;
					code3->code->assign.right = search_operand(t2);
					return ICs_concat(3,code1,code2,code3);	
				}	   
			    if(strcmp(op,"PLUS")==0 \
                    || strcmp(op,"MINUS")==0 \
                    || strcmp(op,"STAR")==0 \
                    || strcmp(op,"DIV")==0 ){
				
					if(!place)
						return NULL;
					char *t1 = new_temp();
					char *t2 = new_temp();
					InterCodes *code1 = translate_Exp(p->child[0],t1);
					InterCodes *code2 = translate_Exp(p->child[2],t2);
					InterCodes *code3 = ic_gen_ari(place,t1,t2,op);
					return ICs_concat(3,code1,code2,code3);
				}

			}
			else if(strcmp(p->child[1]->symbol,"Exp")==0){
				return translate_Exp(p->child[1],place);
			}
			else if(strcmp(p->child[0]->symbol,"ID")==0){
				if(!place)
					return NULL;
				char *function = p->child[0]->lexeme;
				if(strcmp(function,"read")==0){
					return ic_gen_read(place);
				}
				else{
					return ic_gen_func_call(place,function);
				}
			}
			else if(strcmp(p->child[1]->symbol,"DOT")==0){
				//TODO	Exp . ID

				char *t1 = new_temp();
				char *t2 = new_temp();
				char *t3 = new_temp();
				InterCodes *code1 = translate_Exp(p->child[0],t1);
				//t1 = ...(v)
				InterCodes *code2 = ic_gen_assign_addr(t2,t1);
				//t2 = &t1
				char *field_name = p->child[2]->lexeme;
				Type type = typeofNode(p->child[0]);
				int size = 0;
				FieldList fields = type->structure;
				while(strcmp(fields->name,field_name)!=0){
					size += sizeofType(fields->type);
					fields = fields->tail;
				}
				InterCodes *code3 = ic_gen_ari(t3,t2,int2string(size),"PLUS");
				//t3 = t2 + size
				InterCodes *code4 = ic_gen_assign_star(place,t3);
				//place = *t3
				return ICs_concat(4,code1,code2,code3,code4);
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
			else if(strcmp(p->child[0]->symbol,"Exp")==0){
				//Exp [ Exp ]
				if(!place)
					return NULL;
				char *t1 = new_temp();
				char *t2 = new_temp();
				char *t3 = new_temp();
				char *t4 = new_temp();
				char *t5 = new_temp();
				//char *t6 = new_temp();
				InterCodes *code1 = translate_Exp(p->child[0],t1);
				//t1 = ...(v)
				InterCodes *code2 = ic_gen_assign_addr(t2,t1);
				//t2 = &t1
				InterCodes *code3 = translate_Exp(p->child[2],t3);
				//t3 = int
				int size = sizeofType(typeofNode(p));
				InterCodes *code4 = ic_gen_ari(t4,t3,int2string(size),"STAR");
				//t4 = t3 * size
				InterCodes *code5 = ic_gen_ari(t5,t2,t4,"PLUS");
				//t5 = t2 + t4
				//InterCodes *code6 = ic_gen_assign_star(t6,t5);
				//t6 = *t5
				//InterCodes *code7  = ic_gen_assign(place,t6);
				//place = t6
				InterCodes *code6 = ic_gen_assign_star(place,t5);
				//place = *t5
				//return ICs_concat(7,code1,code2,code3,code4,code5,code6,code7);
				return ICs_concat(6,code1,code2,code3,code4,code5,code6);
			}
		}
	}
	// rest of case 
	{
		if(!p)
			return NULL;
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

static InterCodes *translate_VarDec(Node *p){
	while(strcmp(p->child[0]->symbol,"ID")!=0){
		p = p->child[0];
	}
	char *v = p->child[0]->lexeme;
	Type type = NULL;
	searchSymTable(v,Variable,(void **)&type,1);
	assert(type);
	if(type->kind != BASIC){
		return ic_gen_dec(v,sizeofType(type));
	}
	else{ 
		new_operand(v,Variable);
		return NULL;
	}
}
static InterCodes *translate_Dec(Node *p){
	if(p->num_of_child==1){
		return translate_VarDec(p->child[0]);
	}
	else{
		//InterCodes *code1 = translate_VarDec(p->child[0]);
		//only variable can init when definition
		//char *variable = code1->code->dec.op->info;
		//char *t1 = new_temp();
		Node *t = p->child[0];
		while(strcmp(t->child[0]->symbol,"ID")!=0){
			t = t->child[0];
		}
		char *place = t->child[0]->lexeme;
		InterCodes *code = translate_Exp(p->child[2],place);
		return code;
		//InterCodes *code2 = translate_Exp(p->child[2],t1);
		//InterCodes *code3 = ic_gen_assign(variable,t1);
		//return ICs_concat(3,code1,code2,code3);
	}
}
static InterCodes *translate_DecList(Node *p){
	if(p->num_of_child==1){
		return translate_Dec(p->child[0]);
	}
	else {
		InterCodes *code1 = translate_Dec(p->child[0]);
		InterCodes *code2 = translate_DecList(p->child[2]);
		return ICs_concat(2,code1,code2);
	}

}
static InterCodes *translate_Def(Node *p){
	return translate_DecList(p->child[1]);
}
static InterCodes *translate_DefList(Node *p){
	if(!p)
		return NULL;
	InterCodes *code1 = translate_Def(p->child[0]);
	InterCodes *code2 = translate_DefList(p->child[1]);
	return ICs_concat(2,code1,code2);
}
static InterCodes *translate_CompSt(Node *p){
	// CompSt -> { DefList StmtList}
	InterCodes *code1 = translate_DefList(p->child[1]);
	InterCodes *code2 = translate_StmtList(p->child[2]);
	return ICs_concat(2,code1,code2);
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

