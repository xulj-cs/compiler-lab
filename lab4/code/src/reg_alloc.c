#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "reg_alloc.h"

#define NUM_OF_REGS (18)
const char* regs[NUM_OF_REGS];
int dirty[NUM_OF_REGS];
//t0--t9 s0--s7

extern FILE* fout;
VarDesc *vars = NULL;

int st_top = 0 ;

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
	st_top = st_top -4;
	new_var_addr(v,st_top);	
	return st_top;
}

void initRegs(){
	memset(regs,0,sizeof(char*)*NUM_OF_REGS);
	memset(dirty,0,sizeof(int)*NUM_OF_REGS);
}

static const char *reg_name(int n){
	char *s = malloc(sizeof(char)*4);
	if(n<10)
		sprintf(s,"$t%d",n);
	else
		sprintf(s,"$s%d",n-10);
	return s;
}
static void spill(int r){
	int offset = get_var_addr(regs[r]);
	fprintf(fout,"sw %s,%d($fp)\n",reg_name(r),offset);
	
}
static int allocate(const char *v){
	for(int i=0;i<NUM_OF_REGS;i++){
		if(regs[i]==NULL){
			regs[i]= v;
			return i;
		}
	}
	srand(time(0));
	int r = (int)rand()%18; 
	spill(r);
	regs[r] = v;
	return r;
}
static int allocate_v(const char *v,int avoid){
	for(int i=0;i<NUM_OF_REGS;i++){
		if(regs[i]==NULL){
			regs[i]= v;
			return i;
		}
	}
	int r ;
	srand(time(0));
	while((r=(int)rand()%18)==avoid);
	spill(r);
	regs[r] = v;
	return r;
}
const char* ensure(const char *v,int flag){
	if(strcmp(v,"0")==0){
		return "$zero";
	}
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


	if(r == -1){
		r = allocate(v);
		if(flag){
			dirty[r] = 1;
		}
		else{
			int off = get_var_addr(v);
			fprintf(fout,"lw %s, %d($fp)\n",reg_name(r),off);
		}
	}
	return reg_name(r);
}
const char* ensure_v(const char *v,const char *v2){
	if(strcmp(v,"0")==0){
		return "$zero";
	}
	int r = -1;
	int avoid = -1;
	for(int i=0;i<NUM_OF_REGS;i++){
		if(regs[i]==NULL)
			continue;
		if(strcmp(v,regs[i])==0)
			r = i;
		if(strcmp(v2,regs[i])==0)
			avoid = i;
		
	}

	if(r == -1){
		r = allocate_v(v,avoid);
		int off = get_var_addr(v);
		fprintf(fout,"lw %s, %d($fp)\n",reg_name(r),off);
	}
	return reg_name(r);
}




void storeDirtyVar(){
	for(int i=0;i<NUM_OF_REGS;i++){
		if(dirty[i]){
			spill(i);
			dirty[i]=0;
		}
	
	}
}




