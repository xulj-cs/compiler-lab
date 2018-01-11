#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "reg_alloc.h"

#define NUM_OF_REGS (18)
const char* regs[NUM_OF_REGS];
int isDirty[NUM_OF_REGS];
//t0--t9 s0--s7

VarDesc *vars = NULL;
int st_top = 0;

void new_var_addr(const char *name,int off){
	if(vars==NULL){
		vars = malloc(sizeof(VarDesc*));
		vars->name = name;
		vars->offset = off;
		vars->next = NULL;
	}
	else{
		VarDesc *p = vars;
		while(p->next)
			p = p->next;
		p->next = malloc(sizeof(VarDesc*));
		p->name = name ;
		p->offset = off;
		p->next = NULL;
	}
}
int get_var_addr(const char *v){
	VarDesc *p = vars;
	while(p){
		if(strcmp(p->name,v)==0)
			return vars->offset;
		p = p->next;
	}
	
	return -1;
}

void initRegs(){
	memset(regs,0,sizeof(char*)*NUM_OF_REGS);
	memset(isDirty,0,sizeof(int)*NUM_OF_REGS);
}
void initStack(){
	st_top = 0;
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
		printf("addi $sp, $sp, -4\n");
		st_top = st_top - 4;
		new_var_addr(regs[r],st_top);	
	}
	printf("sw %s,%d($fp)",reg_name(r),offset);
	
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
const char* ensure(const char *v){
	for(int i=0;i<NUM_OF_REGS;i++){
		if(regs[i]==NULL)
			continue;
		if(strcmp(v,regs[i])==0){
			return reg_name(i);
		}
	}
	return reg_name(allocate(v));
}



void storeDirtyVar(){
	for(int i=0;i<NUM_OF_REGS;i++){
		if(isDirty[i]){
			spill(i);
		}
	
	}

}




