#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "symtable.h"
#include "semanticAnalysis.h"
symNode* table[SIZE_OF_TABLE];

unsigned int hash_pjw(const char *name){
	unsigned int val = 0,i;
	for(; *name; name++){
		val = (val << 2) + *name;
		if( (i = val & ~(SIZE_OF_TABLE)) )
			val = (val ^ (i>>12)) & SIZE_OF_TABLE;
	}
	return val;
}


void initSymTable(){
	memset(table,0,sizeof(symNode*)*SIZE_OF_TABLE);
}
bool searchSymTable(const char *name,int kind, void **info,int flag){
	// flag 1 : if success , return info
	// flag 0 ：if success , return nothing
	unsigned int index = hash_pjw(name);
	symNode *p = table[index];
	while(p){
		if(strcmp(name,p->name)==0&&kind==p->kind){
			if(flag)
				memcpy(info,&p->info,sizeof(p->info));
			return true;
		}
		p = p->tail;
	}	

	return false;
}
int searchSymTable_f(const char *name,ParaList_Ret paraList_ret,int lineno){
	//ret = 0 :neither dec nor def
	//ret = 1 :redefined funcion
	//ret = 2 :def dec mismatched or dec & dec mismatched
	//ret = 3 :def->dec or dec->def or def->def matched
	unsigned int index = hash_pjw(name);
	symNode *p = table[index];
	while(p){
		if(strcmp(name,p->name)==0&&p->kind==Function){
			if(lineno==0){
				//definition
				if(p->lineno==0){
					//have been definition
					return 1;
				}
				else{
					//have been declaration
					if(!fieldEq(paraList_ret,p->info.para_ret))
						return 2;
					p->lineno = 0;
					return 3;
				}
			}
			else{
				//declaration
				if(p->lineno==0){
					//have been definition
					if(!fieldEq(paraList_ret,p->info.para_ret))
						return 2;
					return 3;
				}
				else{
					//have been declaration
					if(!fieldEq(paraList_ret,p->info.para_ret))
						return 2;
					p->lineno = lineno;
					return 3;
				}

			}
		}
		p = p->tail;
	}	
	return 0;	
}
int insertSymTable(const char *name,int kind,void * info,int func_line){
	//return value 0 : sucess
	//1 :	(new)V->(old)V
	//2 :  	(new)V->(old)S
	//3 :   (new)S->(old)V
	//4 :   (new)S->(old)S
	//5 :	redefined function
	//6 :   mismatched
	//printf("insertSymTable:%d--%s \n",kind,name);
	if(kind == Variable){
		if(searchSymTable(name,Variable,NULL,0))
			return 1;
		if(searchSymTable(name,Structure,NULL,0))
			return 2;	
	}
	else if(kind == Structure){
		if(searchSymTable(name,Variable,NULL,0))
			return 3;
		if(searchSymTable(name,Structure,NULL,0))
			return 4;		
	}
	else if (kind == Function){
		int ret = searchSymTable_f(name,info,func_line);
		if(ret == 3)
			return 0;
		else if(ret == 1)
			return 5;
		else if(ret == 2)
			return 6;
	}
	symNode *p = (symNode *)malloc(sizeof(symNode));
	p->name = name;
	p->kind = kind;
	memcpy(&p->info,&info,sizeof(info));
	if(kind==Function)
		p->lineno = func_line;
	p->tail = NULL;
	unsigned int index = hash_pjw(name);
	if(table[index]==NULL){
		table[index] = p;
	}
	else{
		symNode *t = table[index];
		while(t->tail){
			t = t->tail;
		}
		t->tail = p;
	}

	return 0;	
}



void checkSymTable(){

	symNode *p;
	for(int i=0;i<SIZE_OF_TABLE;i++){
		if((p=table[i])){
			if(p->kind == Function && p->lineno!=0)
				SERROR(18);
			while((p = p->tail)){
				if(p->kind == Function && p->lineno!=0)
					SERROR(18);
			}
		}
	}

}



/*		for debugging ...
static void printType(Type type);
static void printField(FieldList field){
	if(field==NULL)
		return ;
	if(field->name){
		printf("%s\t",field->name);
		printType(field->type);
	}
	else 
		printf("(null)\t");
	if(field->tail){
		printf("-->");
		printField(field->tail);
	}
	
}
static void printType(Type type){

		if(!type){
			printf("(null)");
			return;
		}
		switch(type->kind){
			case 0:printf("Basic:");
				   if(type->basic==0)
					   printf("INT");
				   else
					   printf("FLOAT");
				   break;
			case 1:printf("Array:");
				   printf("[%d]",type->array.size);
				   printType(type->array.elem);
				   break;

			case 2:printf("Structure:");
				   printField(type->structure);
				   break;
		}

}
static void printSymNode(symNode *t){
	printf("%s\t",t->name);
	switch(t->kind){
		case 0:printf("Variable\t");printType(t->info.type);break;
		case 1:printf("Structure\t");printField(t->info.field);break;
		case 2:printf("Function\t");printField(t->info.para_ret);break;
	}

	printf("\n");
}
void printSymTable(){
	printf("*************************************************\n");
	for(int i=0;i<SIZE_OF_TABLE;i++){
		symNode *t;
		if((t=table[i])){
			printSymNode(t);
			while((t = t->tail)){
				printSymNode(t);
			}
		}
	}
}
*/
