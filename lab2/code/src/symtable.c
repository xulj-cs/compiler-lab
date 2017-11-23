#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "symtable.h"

symNode* table[SIZE_OF_TABLE];

static unsigned int hash_pjw(char *name){
	unsigned int val = 0,i;
	for(; *name; name++){
		val = (val << 2) + *name;
		if( i = val & ~(SIZE_OF_TABLE))
			val = (val ^ (i>>12)) & SIZE_OF_TABLE;
	}
	return val;
}

static void semanticError(int kind,int lineno){
	printf("Error type %d at Line %d: ",kind,lineno);
	switch(kind){
		case 17:printf("Undefined structure.\n");break;
	}
}
void initSymTable(){
	memset(table,0,sizeof(symNode*)*SIZE_OF_TABLE);
}
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

		assert(type);
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
		case 2:printf("Funciton\t");printField(t->info.para);break;
	}

	printf("\n");
}
void printSymTable(){
	for(int i=0;i<SIZE_OF_TABLE;i++){
		symNode *t;
		if(t=table[i]){
			printSymNode(t);
			while(t = t->tail){
				printSymNode(t);
			}
		}
	}
}
static void insertSymTable(char *name,int kind,void * info){
	printf("insertSymTable:%d--%s \n",kind,name);
	symNode *p = (symNode *)malloc(sizeof(symNode));
	p->name = name;
	p->kind = kind;
	memcpy(&p->info,&info,sizeof(info));
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
}

static bool searchSymTable(char *name,int kind, void **info){
	unsigned int index = hash_pjw(name);
	symNode *p = table[index];
	while(p){
		if(strcmp(name,p->name)==0&&kind==p->kind){

			if(kind==Structure){
				*info = p->info.field;
			}
			return true;
		}
		p = p->tail;
	}	

	return false;
}

static void VarDec(Node *p,Type type,FieldList field,int flag){	
	//flag = 01 : the field in structure ; 
	//flag = 10 : variable definition ;
	//flag = 11 : both
	if(p->num_of_child==1){
		// VarDec -> id
		if(flag&2)
			insertSymTable(p->child[0]->lexeme,Variable,type);
		if (flag&1) {
			field->name = p->child[0]->lexeme;
			field->type = type;
		}
	}
	else if(p->num_of_child==4){
		// VarDec -> VarDec[int]
		Type arrayType = (Type)malloc(sizeof(struct Type_));
		arrayType->kind = ARRAY;
		arrayType->array.elem = type;
		{
        	int base;
			char *lexeme = p->child[2]->lexeme;	
			if( /*lexeme[0]=='0'&&*/ lexeme[1]=='x'||lexeme[1]=='X')
				base = 16;
            else if(lexeme[0]=='0')
				base = 8;
            else
				base = 10;
            arrayType->array.size = strtol(lexeme,NULL,base);
		}
		VarDec(p->child[0],arrayType,field,flag);
	}
	else 
		assert(0);
}
static void Dec(Node *p,Type type,FieldList field){
	if(p->num_of_child==1){
		//Dec -> VarDec
		VarDec(p->child[0],type,field,1);
	}
	else{
	
	}
}
static void DecList(Node *p,Type type,FieldList field){
	if(p->num_of_child==1){
		//DecList -> Dec
		Dec(p->child[0],type,field);
		field->tail = NULL;
	}
	else if(p->num_of_child==3){
		//DecList -> Dec,DecList
		Dec(p->child[0],type,field);
		field->tail = (FieldList)malloc(sizeof(struct FieldList_));
		DecList(p->child[2],type,field->tail);	
	}
}

static Type Specifier(Node *p);
static void Def(Node *p,FieldList field){
	//Def -> Specifier DecList SEMI
	Type type = Specifier(p->child[0]);
	if(type==NULL){
		semanticError(17,p->lineno);
		field->tail = NULL;
	}
	else
		DecList(p->child[1],type,field);//name type

}
static FieldList DefList(Node *p){
	FieldList field;
	if(p->child[1]){
		//DefList -> Def DefList
		field = (FieldList) malloc(sizeof(struct FieldList_));
		Def(p->child[0],field);//name type
		FieldList t = field;
		while(t->tail){
			t = t->tail;
		}
		t->tail = DefList(p->child[1]);
	}
	else{
		//DefList -> Def
		field = (FieldList) malloc(sizeof(struct FieldList_));
		Def(p->child[0],field);//name type	
	}

	return field;
}
static Type StructSpecifier(Node *p){
	Type type = NULL;
	if(p->num_of_child==5){
		// structSpecifier -> struct OptTag { DefList  }  
		type = (Type) malloc(sizeof(struct Type_));
		type->kind = STRUCTURE;
		type->structure = NULL;
		if(p->child[3]){
			type->structure = DefList(p->child[3]);
		}
		if(p->child[1]){
			insertSymTable(p->child[1]->child[0]->lexeme,Structure,type->structure);
		}
	}
	else if (p->num_of_child==2){		
		// structSpecifier -> struct Tag
		FieldList field ;
		if(searchSymTable(p->child[1]->child[0]->lexeme,Structure,(void **)&field)){
			type = (Type) malloc(sizeof(struct Type_));
			type->kind = STRUCTURE;
			type->structure = field;
		}
	}
	else 
		assert(0);
	return type;
}
static Type Specifier(Node *p){
	Type type;
	if(strcmp(p->child[0]->symbol,"TYPE")==0){
		type = (Type)malloc(sizeof(struct Type_));
		type->kind = BASIC;
		if(strcmp(p->child[0]->lexeme,"int")==0)
			type->basic = INT;
		else if(strcmp(p->child[0]->lexeme,"float")==0)
			type->basic = FLOAT;
		else
			assert(0);
	}
	else if (strcmp(p->child[0]->symbol,"StructSpecifier")==0){
		//type->kind = STRUCTURE;
		//type->structure = StructSpecifier(p->child[0]);
		type = StructSpecifier(p->child[0]);
	}
	else 
		assert(0);
	return type;
}

static void ExtDecList(Node *p,Type type){
	if(p->num_of_child==1){
		//ExtDecList -> VarDec
		VarDec(p->child[0],type,NULL,2);
	}
	else if(p->num_of_child==3){
		//ExtDecList -> VarDec , ExtDecList
		VarDec(p->child[0],type,NULL,2);
		ExtDecList(p->child[2],type);	
	}
	else
		assert(0);
}
static void ParamDec(Node *p,FieldList field){
	//ParamDec -> Specifier VarDec
	Type type = Specifier(p->child[0]);
	if(type==NULL){
		semanticError(17,p->lineno);
	}
	else
		VarDec(p->child[1],type,field,3);
}
static FieldList VarList(Node *p){
	FieldList field = (FieldList)malloc(sizeof(struct FieldList_));
	ParamDec(p->child[0],field);//name type
	if (p->num_of_child==3){
		// VarList -> ParamDec , VarList
		field->tail = VarList(p->child[2]);
	}
	else if (p->num_of_child==1){
		// VarList -> ParamDec
		field->tail = NULL;
	}
	else
		assert(0);
	return field;
}

static void FunDec(Node *p,Type ret_type){
	ParaList_Ret para = (ParaList_Ret)malloc(sizeof(struct FieldList_));
	para->name = "return";
	para->type = ret_type;
	
	if(p->num_of_child==4){
		// FunDec -> ID ( VarList )
		para->tail = VarList(p->child[2]);	
	}
	else if(p->num_of_child==3){
		// FunDec -> ID ( )
		para->tail = NULL;
	}
	else 
		assert(0);
	insertSymTable(p->child[0]->lexeme,Function,para);	

}
static void ExtDef(Node *p){
	Type type = Specifier(p->child[0]);
	if (strcmp(p->child[1]->symbol,"ExtDecList")==0){
		// ExtDef -> Specifier ExtDecList SEMI
		if(type==NULL){
			semanticError(17,p->lineno);
		}
		else
			ExtDecList(p->child[1],type);

	}
	else if(strcmp(p->child[1]->symbol,"SEMI")==0){
		// ExtDef -> Specifier SEMI
		//do nothing with definition(except structure)
	}
	else if(strcmp(p->child[1]->symbol,"FunDec")==0){
		// ExtDef -> Specifier FunDec CompSt
		
		// new type may not be defined in a return type
		if(type==NULL){
			semanticError(17,p->lineno);
		}
		else
			FunDec(p->child[1],type);
	}
}

/*static void Def(Node *p){


}*/
void updateSymTable(Node *p){	//p->symbol = ExtDef OR Def
	/*	
	ExtDef:	Specifier ExtDecList SEMI
		|	Specifier SEMI
		|	Specifier FunDec CompSt
	Def	:	Specifier DecList SEMi
	*/
	if(strcmp(p->symbol,"ExtDef")==0){
		ExtDef(p);
	}	
}


void checkSymTable(Node *p){	//p->symbol = Exp
	/*
	Exp:...
	*/

}



