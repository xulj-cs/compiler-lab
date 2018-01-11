#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "interCode.h"

extern InterCodes *ICROOT;

const char *ensure(const char *);
static void print_reg(const char *s){
	if(s[0]=='$')
		printf("%s",s);
	else{
		//查找x对应的寄存器--寄存器分配
		printf("%s",ensure(s));
	}
}

static void print_reg_i(const char *x,const char *n){
	print_reg(x);
	printf(",");
	printf("%s\n",n);
}
static void print_reg_2(const char *x,const char *y){
	print_reg(x);
	printf(",");
	print_reg(y);
	printf("\n");
}
static void print_reg_2_i(const char *x,const char *y,const char *n){
	print_reg(x);
	printf(",");
	print_reg(y);
	printf(",");
	printf("%s\n",n);
}
static void print_reg_addr(const char *x,const char *y){
	print_reg(x);
	printf(",");
	printf("0(");
	print_reg(y);
	printf(")");
	printf("\n");
}
static void print_reg_3(const char *x,const char *y,const char *z){
	print_reg(x);
	printf(",");
	print_reg(y);
	printf(",");
	print_reg(z);
	printf("\n");
}



void print_ASM(InterCode* ic){
	assert(ic);
	switch(ic->kind){
		case FUNC_DEC:case LABEL:	printf("%s:\n",ic->name);break;
		case ASSIGN:
		{
			Operand op1 = ic->assign.left;
			Operand op2 = ic->assign.right;
			if(op2->kind == CONSTANT){
				printf("li ");
				print_reg_i(op1->info,op2->info);
			}
			else{
				printf("move ");
				print_reg_2(op1->info,op2->info);
			}
			break;
		}
		case ADD:case SUB:
		{
			Operand op1 = ic->binop.op1;
			Operand op2 = ic->binop.op2;
			Operand op3 = ic->binop.result;
			if(op2->kind == CONSTANT){
				assert(0);
				if(ic->kind == SUB){
					char *t = (char *)malloc(sizeof(char)*(2+strlen(op2->info)));
					strcpy(t+1,op2->info);
					t[0]='-';
					print_reg_2_i(op3->info,op1->info,t);
				}
				else
					print_reg_2_i(op3->info,op1->info,op2->info);
			}							
			else{
				if(ic->kind == ADD)
					printf("add ");
				else
					printf("sub ");
				print_reg_3(op3->info,op1->info,op2->info);
			}		
			break;		
		}
		case MUL:case DIV:
		{
			Operand op1 = ic->binop.op1;
			Operand op2 = ic->binop.op2;
			Operand op3 = ic->binop.result;
			const char *reg = op2->info;
			if(op2->kind == CONSTANT){
				printf("li ");
			//	reg = new_reg();
			//	print_reg_i(reg,op2->info);
			}
			if(ic->kind == MUL){
				printf("mul ");
				print_reg_3(op3->info,op1->info,reg);	
			}
			else{
				printf("div ");
				print_reg_2(op1->info,reg);
				printf("mflo ");
				print_reg(op3->info);
				printf("\n");
			}
			break;
	   	}
		case ASSIGN_STAR:	printf("lw ");
							print_reg_addr(ic->assign.left->info,ic->assign.right->info);
							break;
		case STAR_ASSIGN:	printf("sw ");
							print_reg_addr(ic->assign.right->info,ic->assign.left->info);
							break;
		case GOTO:	printf("j %s\n",ic->name);break;
		case FUNC_CALL:	printf("jar %s\n",ic->func.func_name);
						printf("move ");
						print_reg(ic->func.place->info);
						printf(",$v0\n");
						break;
		case RET:	printf("move ");
					printf("$v0,");
					print_reg(ic->name);
					printf("\n");
					printf("jr $ra\n");
					break;
		case IF:
		{
			const char *op = ic->cond.op;
			if(strcmp(op,"==")==0)
				printf("beq");
			else if(strcmp(op,"!=")==0)
				printf("bne");
			else if(strcmp(op,">") ==0)
				printf("bgt");
			else if(strcmp(op,"<") ==0)
				printf("blt");
			else if(strcmp(op,">=")==0)
				printf("bge");
			else if(strcmp(op,"<=")==0)
				printf("ble");
			printf(" ");
			print_reg_2_i(ic->cond.left,ic->cond.right,ic->cond.label);
			break;
		}
		default:printf("TBD\n");
	}
	printf("***************\n");
}

void print_ASMs(){
	InterCodes* p = ICROOT;
	while(true){
		if(p->code->kind == FUNC_DEC){
			initRegs();
			initStack();
		}
		else if(p->code->kind == LABEL){
			initRegs();
		}
		print_ASM(p->code);
		p = p->next;
		if(p==ICROOT)
			break;
	}
}
