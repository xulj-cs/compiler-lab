#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include "interCode.h"

#define SIZE_OF_TABLE (0x7fff)
Operand opTable[SIZE_OF_TABLE];

FILE *fout;

void init_opTable(){
	memset(opTable,0,sizeof(Operand)*SIZE_OF_TABLE);
}
Operand new_operand(const char *info,int kind){
	Operand op = (Operand)malloc(sizeof(struct Operand_));
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
		fprintf(fout,"%s",op->info);
	else if(op->kind==CONSTANT)
		fprintf(fout,"#%s",op->info);
}
void print_IC(InterCode* ic){
	assert(ic);
	switch(ic->kind){
		case FUNC_DEC:fprintf(fout,"FUNCTION %s :\n",ic->name);break;
		case PARAM:	fprintf(fout,"PARAM %s\n",ic->op->info);break;			 
		case ASSIGN:
					print_operand(ic->assign.left);
					fprintf(fout," := ");
					print_operand(ic->assign.right);
					fprintf(fout,"\n");
					break;
		case ASSIGN_ADDR:
					print_operand(ic->assign.left);
					fprintf(fout," := &");
					print_operand(ic->assign.right);
					fprintf(fout,"\n");
					break;
		case ASSIGN_STAR:
					print_operand(ic->assign.left);
					fprintf(fout," := *");
					print_operand(ic->assign.right);
					fprintf(fout,"\n");
					break;
		case STAR_ASSIGN:
					fprintf(fout,"*");
					print_operand(ic->assign.left);
					fprintf(fout," := ");
					print_operand(ic->assign.right);
					fprintf(fout,"\n");
					break;
		case ADD:case SUB:case MUL:case DIV:{
				    print_operand(ic->binop.result);
					fprintf(fout," := ");
					print_operand(ic->binop.op1);
					switch(ic->kind){
						case ADD:
							fprintf(fout," + ");break;
						case SUB:
							fprintf(fout," - ");break;
						case MUL:
							fprintf(fout," * ");break;
						case DIV:
							fprintf(fout," / ");break;
						default:assert(0);
					}
					print_operand(ic->binop.op2);
					fprintf(fout,"\n");
					break;
		}
		case RET:	fprintf(fout,"RETURN ");
					fprintf(fout,"%s\n",ic->name);
					break;
		case IF :	fprintf(fout,"IF ");
					print_operand(ic->cond.left);
					fprintf(fout," %s ",ic->cond.op);
					print_operand(ic->cond.right);
					fprintf(fout," ");
					break;
		case GOTO:  fprintf(fout,"GOTO %s\n",ic->name);
					break;
		case LABEL: fprintf(fout,"LABEL %s :\n",ic->name);
					break;
		case READ:	fprintf(fout,"READ %s\n",ic->op->info);
					break;
		case WRITE:	fprintf(fout,"WRITE %s\n",ic->op->info);
					break;
		case FUNC_CALL: fprintf(fout,"%s := CALL %s\n",ic->func.place->info,ic->func.func_name);
					   break;	
		case ARG :	fprintf(fout,"ARG %s\n",ic->op->info);
					break;
		case DEC :	fprintf(fout,"DEC %s %d\n",ic->dec.op->info,ic->dec.size);
					break;
		default : fprintf(fout,"TBD");
	}
	
}

