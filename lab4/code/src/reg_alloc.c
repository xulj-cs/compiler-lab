#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "reg_alloc.h"

#define NUM_OF_REGS (18)
const char* regs[NUM_OF_REGS];
int dirty[NUM_OF_REGS];
//t0--t9 s0--s7

extern FILE* fout;
VarDesc *vars = NULL;
int st_top = 0;
void print_var(){
	VarDesc *p = vars;
	while(p){
		printf("%s:%d\n",p->name,p->offset);
		p = p->next;
	}
}


void new_var_addr(const char *name,int off){
	if(vars==NULL){
		vars = malloc(sizeof(VarDesc));
		vars->name = name;
		vars->offset = off;
		vars->next = NULL;
	}
	else{
		VarDesc *p = vars;
		while(p->next)
			p = p->next;
		p->next = malloc(sizeof(VarDesc));
		p->next->name = name ;
		p->next->offset = off;
		p->next->next = NULL;
	}
}
int get_var_addr(const char *v){
	VarDesc *p = vars;
	while(p){
		if(strcmp(p->name,v)==0)
			return p->offset;
		p = p->next;
	}
	//printf("//%s is not in stack\n",v);	
	return -1;
}

void initRegs(){
//	printf("************INIT REGS*********\n");
	memset(regs,0,sizeof(char*)*NUM_OF_REGS);
	memset(dirty,0,sizeof(int)*NUM_OF_REGS);
}
void initStack(){
	st_top = 0;
//	printf("************INIT ST*********\n");
}

const char *reg_name(int n){
	char *s = malloc(sizeof(char)*4);
	if(n<10)
		sprintf(s,"$t%d",n);
	else
		sprintf(s,"$s%d",n-10);
	return s;
}
void spill(int r){
	int offset = get_var_addr(regs[r]);
	if(offset == -1){
//		printf("addi $sp, $sp, -4\n");
		st_top = st_top - 4;
		offset = st_top;
		new_var_addr(regs[r],st_top);	
	}
	fprintf(fout,"sw %s,%d($fp)\n",reg_name(r),offset);
	//print_var();
	
}
int FarthestUse(){
	return 0;
}
int allocate(const char *v){
	for(int i=0;i<NUM_OF_REGS;i++){
		if(regs[i]==NULL){
			regs[i]= v;
			return i;
		}
	}
	int r = FarthestUse();
	spill(r);
	regs[r] = v;
	return r;
}
void print_regs(){
	for(int i=0;i<NUM_OF_REGS;i++){
		if(regs[i]){
			printf("%s:%s\n",reg_name(i),regs[i]);
		}
	}
}
const char* ensure(const char *v,int flag){
//	print_regs();
	int r = -1;
	for(int i=0;i<NUM_OF_REGS;i++){
		if(regs[i]==NULL)
			continue;
		if(strcmp(v,regs[i])==0){
			r = i;
			if(flag)
				dirty[r] = 1;
			break;
		}
	}
	//r != -1 v的值保存在第r个寄存器中


	if(r == -1){
		r = allocate(v);
		if(flag){
		//	printf("***%s:%s is dirty****\n",v,reg_name(r));
			dirty[r] = 1;
		}
		else{
			int off = get_var_addr(v);
			fprintf(fout,"lw %s, %d($fp)\n",reg_name(r),off);
		}
	}
	return reg_name(r);
}



void storeDirtyVar(){
//	print_var();
	for(int i=0;i<NUM_OF_REGS;i++){
		if(dirty[i]){
			spill(i);
			dirty[i]=0;
		}
	
	}
//	print_var();
//	printf("************STORE REGS*********\n");
}




