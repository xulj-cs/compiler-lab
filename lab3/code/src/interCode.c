#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include "interCode.h"


//void init_IC(){
//	ICs = NULL;
//}

InterCodes* ICs_push(InterCodes *head,InterCode *code){
	if(head == NULL){
		head  = (InterCodes*)malloc(sizeof(InterCodes));
		head->code = code;
		head->prev = head->next = head;	
	}
	else {

		InterCodes* p = (InterCodes*)malloc(sizeof(InterCodes));
		p->code = code;
		head->prev->next = p;
		p->next = head;
		p->prev = head->prev;
		head->prev = p;

	}
	return head;
}
InterCodes *IC_concat(int num_of_code,...){
	InterCodes *head = NULL;
	va_list codeList;
	va_start(codeList,num_of_code);
	for(int i=0;i<num_of_code;i++){
		head = ICs_push(head,va_arg(codeList,InterCode*));
	}	
	va_end(codeList);
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
Operand new_operand(char *info,int flag){
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = flag==0?CONSTANT:VARIABLE;
	op->info = info;
	return op;

}
void print_operand(Operand op){
	if(op->kind==VARIABLE)
		printf("%s",op->info);
	else if(op->kind==CONSTANT)
		printf("#%s",op->info);
}
void print_IC(InterCode* ic){
	assert(ic);
	switch(ic->kind){
		case FUN_DEC:printf("FUNCTION %s :\n",ic->name);break;
		case VAR_DEC:printf("PARAM %s\n",ic->name);break;			 
		case ASSIGN:
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
		default : printf("TBD");
	}
	
}

