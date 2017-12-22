#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "interCode.h"

#define SIZE_OF_TABLE (0x7fff)
Operand opTable[SIZE_OF_TABLE];

void init_opTable(){
	memset(opTable,0,sizeof(Operand)*SIZE_OF_TABLE);
}
Operand new_operand(const char *info,int kind){
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	//op->kind = isdigit(info[0])?CONSTANT:VARIABLE;
	op->kind = kind;
	op->info = info;
	int i=0;
	while(opTable[i]){
		i++;
	}
	opTable[i] = op;
	return op;
}
Operand search_operand(const char *info){
	for(int i=0;i<SIZE_OF_TABLE;i++){
		if(opTable[i]==NULL)
			break;
		if(strcmp(opTable[i]->info,info)==0)
			return opTable[i];
	}

	printf("35%s\n",info);
	assert(0);
	return NULL;
}

InterCodes* IC_2_ICs(InterCode *code){
	InterCodes *head  = (InterCodes*)malloc(sizeof(InterCodes));
	head->code = code;
	head->prev = head->next = head;	
	return head;
}
static void ICs_concat_2(InterCodes *head1,InterCodes* head2){
	head1->prev->next = head2;
	head2->prev->next = head1;
	
	InterCodes *p = head1->prev;
	head1->prev = head2->prev;
	head2->prev = p;
}
InterCodes *ICs_concat(int num,...){
	va_list codesList;
	va_start(codesList,num);
	InterCodes* head,*p;
	int i=1;
	while(NULL == (head = va_arg(codesList,InterCodes*))){
		i++;
		if(i>num)
			return NULL;
	}
	for(;i<num;i++){
		if((p = va_arg(codesList,InterCodes*)))
			ICs_concat_2(head,p);
	}
	va_end(codesList);
	return head;
}

InterCodes *ICs_pop_back(InterCodes *head){
	assert(head);
	if(head->next == head ){
		return NULL;
	}
	head->prev->prev->next = head;
	head->prev = head->prev->prev;
	return head;
}
void print_operand(Operand op){
	if(op->kind==VARIABLE||op->kind==ADDRESS)
		printf("%s",op->info);
	else if(op->kind==CONSTANT)
		printf("#%s",op->info);
}
void print_IC(InterCode* ic){
	assert(ic);
	switch(ic->kind){
		case FUNC_DEC:printf("FUNCTION %s :\n",ic->name);break;
		case PARAM:	printf("PARAM %s\n",ic->op->info);break;			 
		case ASSIGN:
					print_operand(ic->assign.left);
					printf(" := ");
					print_operand(ic->assign.right);
					printf("\n");
					break;
		case ASSIGN_ADDR:
					print_operand(ic->assign.left);
					printf(" := &");
					print_operand(ic->assign.right);
					printf("\n");
					break;
		case ASSIGN_STAR:
					print_operand(ic->assign.left);
					printf(" := *");
					print_operand(ic->assign.right);
					printf("\n");
					break;
		case STAR_ASSIGN:
					printf("*");
					print_operand(ic->assign.left);
					printf(" := ");
					print_operand(ic->assign.right);
					printf("\n");
					break;
		case ADD:case SUB:case MUL:case DIV:{
				    print_operand(ic->binop.result);
					printf(" := ");
					print_operand(ic->binop.op1);
					switch(ic->kind){
						case ADD:
							printf(" + ");break;
						case SUB:
							printf(" - ");break;
						case MUL:
							printf(" * ");break;
						case DIV:
							printf(" / ");break;
						default:assert(0);
					}
					print_operand(ic->binop.op2);
					printf("\n");
					break;
		}
		case RET:	printf("RETURN ");
					printf("%s\n",ic->name);
					break;
		case IF :	printf("IF ");
					print_operand(ic->cond.left);
					printf(" %s ",ic->cond.op);
					print_operand(ic->cond.right);
					printf(" ");
					break;
		case GOTO:  printf("GOTO %s\n",ic->name);
					break;
		case LABEL: printf("LABEL %s :\n",ic->name);
					break;
		case READ:	printf("READ %s\n",ic->op->info);
					break;
		case WRITE:	printf("WRITE %s\n",ic->op->info);
					break;
		case FUNC_CALL: printf("%s := CALL %s\n",ic->func.place->info,ic->func.func_name);
					   break;	
		case ARG :	printf("ARG %s\n",ic->op->info);
					break;
		case DEC :	printf("DEC %s %d\n",ic->dec.op->info,ic->dec.size);
					break;
		default : printf("TBD");
	}
	
}

