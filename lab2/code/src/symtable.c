#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "symtable.h"

symNode* table[SIZE_OF_TABLE];

static unsigned int hash_pjw(const char *name){
	unsigned int val = 0,i;
	for(; *name; name++){
		val = (val << 2) + *name;
		if( (i = val & ~(SIZE_OF_TABLE)) )
			val = (val ^ (i>>12)) & SIZE_OF_TABLE;
	}
	return val;
}

static void semanticError(int kind,int lineno,const char *info){
	printf("Error type %d at Line %d: ",kind,lineno);
	if(info)
		printf("%s.\n",info);
	else{
		switch(kind){
			case 1 :printf("Undefined variable");break;
			case 2 :printf("Undefined function");break;
			case 3 :printf("Redefined variable.OR ...");break;
			case 4 :printf("Redefined function");break;
			case 5 :printf("Type mismatched for assignment");break;
			case 6 :printf("The left-hand side of an assignment must be a variable");break;
			case 7 :printf("Type mismathced for operands");break;
			case 8 :printf("Type mismathced for return");break;
			case 9 :printf("Actual args mismathced for function call");break;
			case 10:printf("Illegal usage for \"[ ]\"");break;
			case 11:printf("Illegal usage for \"( )\"");break;
			case 12:printf("Non-integer Exp in [ ]");break;
			case 13:printf("Illegal usage for \".\"");break;
			case 14:printf("Undefined field in struct");break;
			case 15:printf("Redefined field in struct");break;
			case 16:printf("Redefined struct OR ...");break;
			case 17:printf("Undefined struct");break;

		}
	}
	printf(".\n");
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
		case 2:printf("Funciton\t");printField(t->info.para_ret);break;
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
static bool searchSymTable(const char *name,int kind, void **info,int flag){
	// flag 1 : if success , return info
	// flag 0 ：if success , return nothing
	unsigned int index = hash_pjw(name);
	symNode *p = table[index];
	while(p){
		if(strcmp(name,p->name)==0&&kind==p->kind){

/*			if(kind==Variable && flag){
				*info = p->info.type;
			}
			if(kind==Structure && flag){
				*info = p->info.field;
			}
			if(kind==Function && flag){
				*info = p->info.para_ret;
			}
*/
			if(flag)
				memcpy(info,&p->info,sizeof(p->info));
			return true;
		}
		p = p->tail;
	}	

	return false;
}
static int insertSymTable(const char *name,int kind,void * info){
	//return value 0 : sucess
	//1 :	(new)V->(old)V
	//2 :  	(new)V->(old)S
	//3 :   (new)S->(old)V
	//4 :   (new)S->(old)S
	//5 :   (new)F->(old)F
	printf("insertSymTable:%d--%s \n",kind,name);
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
		if(searchSymTable(name,Function,NULL,0))
			return 5;
	}
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

	return 0;	
}


static void VarDec(Node *p,Type type,FieldList field,int flag){	
	//flag = 01 : the field in structure ; 
	//flag = 10 : variable definition ;
	//flag = 11 : both
	if(p->num_of_child==1){
		// VarDec -> id
		if(flag&2){
			int ret = insertSymTable(p->child[0]->lexeme,Variable,type);
			if(ret == 1 || ret == 2){
				semanticError(3,p->lineno,"Redefined Variable")	;
			}
		}
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
			const char *lexeme = p->child[2]->lexeme;	
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
static void Dec(Node *p,Type type,FieldList field,int flag){
	if(p->num_of_child==1){
		//Dec -> VarDec
		VarDec(p->child[0],type,field,flag);
	}
	else if(p->num_of_child==3){ 
		//Dec -> VarDec = Exp
		if(flag==1)
			semanticError(15,p->lineno,"Illegal initialization in structure");
		else if (flag==2){
			//...先检查类型？？？
			VarDec(p->child[0],type,field,flag);
		}

	}
}
static void DecList(Node *p,Type type,FieldList field,int flag){
	if(p->num_of_child==1){
		//DecList -> Dec
		Dec(p->child[0],type,field,flag);
		if(flag == 1)
			field->tail = NULL;
	}
	else if(p->num_of_child==3){
		//DecList -> Dec,DecList
		Dec(p->child[0],type,field,flag);
		if(flag==1){
			field->tail = (FieldList)malloc(sizeof(struct FieldList_));
			DecList(p->child[2],type,field->tail,flag);
		}
		else if (flag==2){
			DecList(p->child[2],type,field,flag);
		}	
	}
}

static Type Specifier(Node *p);
static void Def(Node *p,FieldList field,int flag){
	//Def -> Specifier DecList SEMI
	//flag = 01 : the field in structure ; 
	//flag = 10 : variable definition ;
	Type type = Specifier(p->child[0]);
	if(type==NULL){
		semanticError(17,p->lineno,NULL);
		if(flag == 1)
			field->tail = NULL;
	}
	else
		DecList(p->child[1],type,field,flag);//name type

}
static FieldList DefList(Node *p,int flag){
	//flag = 01 : struct OptTag{ *** }
	//flag = 10 : CompSt -> { *** }
	if(!p)
		return NULL;
	FieldList field;
	if(p->child[1]){
		//DefList -> Def DefList
		if(flag==1)
			field = (FieldList) malloc(sizeof(struct FieldList_));
		else if(flag==2)
			field = NULL;
		Def(p->child[0],field,flag);//name type
		if(flag==1){
			FieldList t = field;
			while(t->tail){
				t = t->tail;
			}
			t->tail = DefList(p->child[1],flag);
		}
		else if(flag==2){
			DefList(p->child[1],flag);
		}
	}
	else{
		//DefList -> Def
		if(flag==1)
			field = (FieldList) malloc(sizeof(struct FieldList_));
		else if (flag==2)
			field = NULL;
		Def(p->child[0],field,flag);//name type	
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
			type->structure = DefList(p->child[3],1);
		}
		if(p->child[1]){
			int ret = insertSymTable(p->child[1]->child[0]->lexeme,Structure,type->structure);
			if(ret == 3|| ret ==4){
				semanticError(16,p->lineno,"Redefined structure");
			}
		}
	}
	else if (p->num_of_child==2){		
		// structSpecifier -> struct Tag
		FieldList field ;
		if(searchSymTable(p->child[1]->child[0]->lexeme,Structure,(void **)&field,1)){
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
		{
			printf("%s\n",p->child[0]->lexeme);
			assert(0);
		}
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
		semanticError(17,p->lineno,NULL);
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
	int ret = insertSymTable(p->child[0]->lexeme,Function,para);	
	if(ret == 5){
		semanticError(4,p->lineno,"Redefined funcion");
	}
}

static Type Exp(Node *p);
static bool argsEq(Node *p,FieldList para){
	if(!para)
		return false;
	if(p->num_of_child==3){
		return typeEq(para->type,Exp(p->child[0])) && argsEq(p->child[2],para->tail); 
	}
	else/* if(p->num_of_child==1) */{
		return typeEq(para->type,Exp(p->child[0])) && para->tail == NULL;
	}

}
static Type Exp(Node *p){

	if(p->num_of_child==3){
	   	if(strcmp(p->child[0]->symbol,"Exp")==0 && strcmp(p->child[2]->symbol,"Exp")==0){
			Type a = Exp(p->child[0]);
			Type b = Exp(p->child[2]);
			if(!a||!b)
				return NULL;
			const char *op = p->child[1]->symbol;
			if(strcmp(op,"ASSIGNOP")==0){
				if(!typeEq(a,b))
					semanticError(5,p->lineno,NULL);
				else
					return a;
			}
			else if(strcmp(op,"AND")==0 || strcmp(op,"OR")==0){
				if(!isInt(a)||!isInt(b))
					semanticError(7,p->lineno,NULL);	
				else
					return a;	
			}
			else if(strcmp(op,"RELOP")==0){
				if(!typeEq(a,b)){
					semanticError(7,p->lineno,NULL);
				}
				else if(!isInt(a)&&!isFloat(b))
					semanticError(7,p->lineno,NULL);
				else{
					Type i = (Type)malloc(sizeof(struct Type_));
					i->kind = BASIC;
					i->basic = INT;
					return i;
				}
				
			}
			else if(strcmp(op,"PLUS")==0 \
					|| strcmp(op,"MINUS")==0 \
					|| strcmp(op,"STAR")==0 \
					|| strcmp(op,"DIV")==0 ){
				if(!typeEq(a,b)){
					semanticError(7,p->lineno,NULL);
				}
				else if(!isInt(a)&&!isFloat(b))
					semanticError(7,p->lineno,NULL);
				else
					return a;
			
			}
		}
		else if(strcmp(p->child[1]->symbol,"Exp")==0){
			//Exp -> ( Exp )
			return Exp(p->child[1]);
		}
		else if(strcmp(p->child[1]->symbol,"DOT")==0){
			//Exp -> Exp . ID
			Type a = Exp(p->child[0]);
			if(!isStruct(a))
				semanticError(13,p->lineno,NULL);
			else {
				Type b;
				if(!(b=isField(a->structure,p->child[2]->lexeme)))
					semanticError(14,p->lineno,NULL);
				else
					return b;
			}
		}
	}
	else if (p->num_of_child==2){
		Type a = Exp(p->child[1]);
		const char *op = p->child[0]->symbol;
		if(strcmp(op,"MINUS")==0){
			if(!isInt(a)&&!isFloat(a))
				semanticError(7,p->lineno,NULL);
			else
				return a;
		}
		else if (strcmp(op,"NOT")==0){
			if(!isInt(a))
				semanticError(7,p->lineno,NULL);
			return a;
		}
	}	
	else if(p->num_of_child==1)	{
		// Exp -> ID | INT | FLOAT	
		const char *sym = p->child[0]->symbol;
		if(strcmp(sym,"ID")==0){
			Type type ;
			if(!searchSymTable(p->child[0]->lexeme,Variable,(void **)&type,1))
				semanticError(1,p->lineno,NULL);
			else
				return type;		
		}
		else if(strcmp(sym,"INT")==0){
			Type i = (Type) malloc(sizeof(struct Type_));
			i->kind = BASIC;
			i->basic = INT;
			return i;
		}
		else if(strcmp(sym,"FLOAT")==0){
			Type f = (Type) malloc(sizeof(struct Type_));
			f->kind = BASIC;
			f->basic = FLOAT;
			return f;
		}
	}
	else if (p->num_of_child==4){
		// Exp -> Exp [ Exp ]	
		if(strcmp(p->child[0]->symbol,"ID")!=0){
			Type a = Exp(p->child[0]);
			Type b = Exp(p->child[2]);
			if (!isInt(b))
				semanticError(12,p->lineno,NULL);
			else{
				Type elem;
				if(!(elem=isArray(a)))
					semanticError(10,p->lineno,NULL);
				else
					return elem;
			}
		}
	}

	if(strcmp(p->child[0]->symbol,"ID") == 0 && p->num_of_child >=3){
		// Exp -> ID ( ) || ID ( ARGS )
			ParaList_Ret para_ret ;
			bool isFunc = searchSymTable(p->child[0]->lexeme,Function,(void **)&para_ret,1);
			bool isVar = searchSymTable(p->child[0]->lexeme,Variable,NULL,0);
			if(!isFunc){
				if(isVar)
					semanticError(11,p->lineno,NULL);
				else
					semanticError(2,p->lineno,NULL);
			}	
			else{
				if(p->num_of_child==3){
					if(para_ret->tail)
						semanticError(9,p->lineno,NULL);
				}
				else if(p->num_of_child==4){
					if(!argsEq(p->child[2],para_ret->tail))
						semanticError(9,p->lineno,NULL);
				}
				 
				return para_ret->type;
			}
	}
	return NULL;
}

static void CompSt(Node *p,Type type);
static void Stmt(Node *p,Type type){
	Type t;
	switch(p->num_of_child){
		case 1:
			//Stmt -> CompSt
			CompSt(p->child[0],type);
			break;
		case 2:
			//Stmt -> Exp ;
			Exp(p->child[0]);
			break;
		case 3:
			//Stmt -> RETURN Exp ;
			if(!(t=Exp(p->child[1])))
				break;
			if(!typeEq(type,t))
				semanticError(8,p->lineno,NULL);
			break;
		case 5:
			//Stmt -> IF (Exp) Stmt
			//  |  WHILE (Exp) Stmt
			if(!(t=Exp(p->child[2])))
				break;
			if(!isInt(t))
				semanticError(7,p->lineno,NULL);
			Stmt(p->child[4],type);
			break;
		case 7:
			//Stmt -> IF (Exp) Stmt ELSE Stmt
			if(!(t=Exp(p->child[2])))
				break;
			if(!isInt(t))
				semanticError(7,p->lineno,NULL);
			Stmt(p->child[4],type);
			Stmt(p->child[6],type);
			break;
	}
}
static void StmtList(Node *p,Type type){
	// StmtList -> Stmt StmtList
	if(!p)
		return ;
	assert(p->num_of_child==2);
	Stmt(p->child[0],type);
	StmtList(p->child[1],type);

}
static void CompSt(Node *p,Type type){
	// CompSt -> { DefList StmtList }
	DefList(p->child[1],2);
	StmtList(p->child[2],type);
}
static void ExtDef(Node *p){
	Type type = Specifier(p->child[0]);
	if (strcmp(p->child[1]->symbol,"ExtDecList")==0){
		// ExtDef -> Specifier ExtDecList SEMI
		if(type==NULL){
			semanticError(17,p->lineno,NULL);
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
			semanticError(17,p->lineno,NULL);
		}
		else
			FunDec(p->child[1],type);
		
		CompSt(p->child[2],type);
	}
}

/*static void Def(Node *p){


}*/
void updateSymTable(Node *p){	//p->symbol = ExtDef // OR Def
	/*	
	ExtDef:	Specifier ExtDecList SEMI
		|	Specifier SEMI
		|	Specifier FunDec CompSt
	//Def	:	Specifier DecList SEMi
	*/
//	if(strcmp(p->symbol,"ExtDef")==0){
		ExtDef(p);
//	}
//	else if (strcmp(p->symbol,"Def")==0){
//		Def(p,NULL,2);
//	}	
}



void checkSymTable(Node *p){	//p->symbol = Exp
	/*
	Exp:...
	*/
	Exp(p);
}



