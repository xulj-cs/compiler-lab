#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "node.h"
#include "interCode.h"
#include "symtable.h"
#include "ic_gen.h"
InterCodes * ICROOT = NULL;


static int temp_no = 0;
static int label_no = 0;
static char *new_temp(){
	// __t[x]
	char *str = (char *)malloc(sizeof(char)*10);
	sprintf(str,"t%d",temp_no);
	temp_no++;
	return str;
}
static char *new_label(){
	// __l[x]
	char *str = (char *)malloc(sizeof(char)*10);
	sprintf(str,"l%d",label_no);
	label_no++;
	return str;
}
//for debugging...
/*
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
*/
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
			InterCodes *code3 = ic_gen_if(t1,t2,op,label_true);
			//InterCodes *code4 = ic_gen_goto(label_true);
			InterCodes *code5 = ic_gen_goto(label_false);
			return ICs_concat(4,code1,code2,code3,/*code4,*/code5);
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
		InterCodes *code2 = ic_gen_if(t1,"0","!=",label_true);
		//InterCodes *code3 = ic_gen_goto(label_true);
		InterCodes *code4 = ic_gen_goto(label_false);
		return ICs_concat(3,code1,code2,/*code3,*/code4);
	
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

static InterCodes *translate_Args(Node *p,char **arg_list,int *flag){
	if(p->num_of_child==1){
		char *t = new_temp();
		InterCodes *code1 = translate_Exp(p->child[0],t);
		Operand op = search_operand(t);
		// t := ...
		if(flag[0]&&op->kind==VARIABLE){
			InterCode *last_code = code1->prev->code;
			last_code->kind = ASSIGN_ADDR;
			last_code->assign.left->kind = ADDRESS;
		}
		*arg_list = t;
		return code1;
	}
	else/* if(p->num_of_child==3)*/{
		char *t = new_temp();
		InterCodes *code1 = translate_Exp(p->child[0],t);
		Operand op = search_operand(t);
		// t := ...
		if(flag[0]&&op->kind==VARIABLE){
			InterCode *last_code = code1->prev->code;
			last_code->kind = ASSIGN_ADDR;
			last_code->assign.left->kind = ADDRESS;
		}
		*arg_list = t;
		InterCodes *code2 = translate_Args(p->child[2],arg_list+1,flag+1);
		return ICs_concat(2,code1,code2);
	}

}
static char *int2string(int n){
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
static InterCodes *translate_Addr(Node *p,char *place);
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
					InterCodes *code4 = ic_gen_assign(place,t2);
					return ICs_concat(4,code1,code2,code3,code4);	
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
					if(place)
						return ic_gen_func_call(place,function);
					else{
						char *t = new_temp();
						return ic_gen_func_call(t,function);
					}
				}
			}
			else if(strcmp(p->child[1]->symbol,"DOT")==0){
				//	Exp . ID

				if(!place)
					return NULL;
				char *t1 = new_temp();
				InterCodes *code1 = translate_Addr(p,t1);
				//t1 = address
				InterCodes *code2 = ic_gen_assign_star(place,t1);
				//place = *t1
				return ICs_concat(2,code1,code2);
			}
		}
		case 4:
		{
			if(strcmp(p->child[0]->symbol,"ID")==0){
				//if(!place)
				//	return NULL;
				char *function = p->child[0]->lexeme;
				int num = num_of_args(function);
				assert(num>=1);
				char **arg_list = (char **)malloc(sizeof(char*)*num);
				int *flag = (int *)malloc(sizeof(int)*num);
				ParaList_Ret para_ret;
				searchSymTable(function,Function,(void**)&para_ret,1);
				FieldList para = para_ret->tail;
				int i=0;
				while(para){
					if(isStruct(para->type) || isArray(para->type)){
						flag[i] = 1;
					}
					else
						flag[i] = 0;
					i++;
					para = para->tail;
				}
				assert(i==num);

				InterCodes *code1 = translate_Args(p->child[2],arg_list,flag);
				InterCodes *code2 = NULL;
				if(strcmp(function,"write")==0){
					code2 = ic_gen_write(arg_list[0]);
					return ICs_concat(2,code1,code2);
				}
				else{
					for (int i=num-1;i>=0;i--){
						code2 = ICs_concat(2,code2,ic_gen_arg(arg_list[i]));	
					}
					InterCodes *code3 = NULL;
					if(place)
						code3 = ic_gen_func_call(place,function);
					else {
						char *t = new_temp();
						code3 = ic_gen_func_call(t,function);

					}
					return	ICs_concat(3,code1,code2,code3);
				}
			}
			else if(strcmp(p->child[0]->symbol,"Exp")==0){
				//Exp [ Exp ]
				if(!place)
					return NULL;
				char *t1 = new_temp();
				InterCodes *code1 = translate_Addr(p,t1);
				//t1 = address
				InterCodes *code2 = ic_gen_assign_star(place,t1);
				//place = *t1
				return ICs_concat(2,code1,code2);
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
static InterCodes *translate_Addr(Node *p,char *place){
	// Exp -> Exp[Exp]
	//	| ID
	if(p->num_of_child==1){

		char *v = p->child[0]->lexeme;
		Operand op = search_operand(v);
		if(op->kind==VARIABLE)
			return ic_gen_assign_addr(place,v);
		else if(op->kind == ADDRESS)
			return ic_gen_assign(place,v);
	}
	else if(p->num_of_child==3){
		char *t1 = new_temp();
		InterCodes *code1 = translate_Addr(p->child[0],t1);
		//t1 = address
		
		char *field_name = p->child[2]->lexeme;
		Type type = typeofNode(p->child[0]);
		int size = 0;
		FieldList fields = type->structure;
		while(strcmp(fields->name,field_name)!=0){
			size += sizeofType(fields->type);
			fields = fields->tail;
		}

		InterCodes *code2 = ic_gen_ari(place,t1,int2string(size),"PLUS");
		//t2 = t1 + size
		return ICs_concat(2,code1,code2);
	}
	else if(p->num_of_child==4){
		char *t1 = new_temp();
		char *t2 = new_temp();
		char *t3 = new_temp();
		InterCodes *code1 = translate_Addr(p->child[0],t1);
		//t1 = ...(v)
		InterCodes *code2 = translate_Exp(p->child[2],t2);
		//t2 = int
		int size = sizeofType(typeofNode(p));
		InterCodes *code3 = ic_gen_ari(t3,t2,int2string(size),"STAR");
		//t3 = t2 * size
		InterCodes *code4 = ic_gen_ari(place,t1,t3,"PLUS");
		//place = t1+t3
		return ICs_concat(4,code1,code2,code3,code4);
	}
	assert(0);
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
		Node *t = p->child[0];
		while(strcmp(t->child[0]->symbol,"ID")!=0){
			t = t->child[0];
		}
		char *place = t->child[0]->lexeme;
		InterCodes *code = translate_Exp(p->child[2],place);
		return code;
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
static void IR_Gen(Node *p){
	// ExtDef -> Specifier FunDec CompSt

	if(strcmp(p->child[1]->symbol,"FunDec")==0){
		InterCodes* code1 = translate_FunDec(p->child[1]);
		InterCodes* code2 = translate_CompSt(p->child[2]);
		InterCodes* code = ICs_concat(2,code1,code2);
		ICROOT = ICs_concat(2,ICROOT,code);
	}
}


void print_ICROOT(){
   	InterCodes* p = ICROOT;
    while(true){
        print_IC(p->code);
		p = p->next;
		if(p==ICROOT)
			break;
    }
}

void InterCodeGen(){
	traverseTree(IR_Gen);
}
