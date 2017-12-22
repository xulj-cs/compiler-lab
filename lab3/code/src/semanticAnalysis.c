#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "symtable.h"
#include "semanticAnalysis.h"

extern int SERROR ;
void semanticError(int kind,int lineno){
	SERROR ++;
	printf("Error type %d at Line %d: ",kind,lineno);
	switch(kind){
			case 1 :printf("Undefined variable");break;
			case 2 :printf("Undefined function");break;
			case 3 :printf("Redefined variable OR duplicate variable name ");break;
			case 4 :printf("Redefined function");break;
			case 5 :printf("Type mismatched for assignment");break;
			case 6 :printf("The left-hand side of an assignment must be a variable");break;
			case 7 :printf("Type mismathced for operands OR mismatched type of expression");break;
			case 8 :printf("Type mismathced for return");break;
			case 9 :printf("Actual args mismathced for function call");break;
			case 10:printf("Illegal usage for \"[ ]\"");break;
			case 11:printf("Illegal usage for \"( )\"");break;
			case 12:printf("Non-integer Exp in \"[ ]\"");break;
			case 13:printf("Illegal usage for \".\"");break;
			case 14:printf("Undefined field in struct");break;
			case 15:printf("Redefined field in struct OR initialization in struct field");break;
			case 16:printf("Redefined struct OR duplicate struct name");break;
			case 17:printf("Undefined struct");break;
			case 18:printf("Undefined function but declare");break;
			case 19:printf("Inconsistent declaration/definition of function");break;
	}
	
	printf(".\n");
}


static void VarDec(Node *p,Type type,FieldList head,int flag){	
	//flag = 01 : the field in structure ; 
	//flag = 10 : variable definition ;
	//flag = 11 : both
	if(p->num_of_child==1){
		// VarDec -> id
		if(flag&2){
			int ret = insertSymTable(p->child[0]->lexeme,Variable,type,0);
			if(ret == 1 || ret == 2){
				SERROR(3)	;
			}
		}
		if (flag&1) {
			FieldList t = head;
			while(t->tail){
				if(strcmp(t->tail->name,p->child[0]->lexeme)==0){
					SERROR(15);
					return;
				}
				t = t->tail;
			}
			t->tail = (FieldList)malloc(sizeof(struct FieldList_));
		
			t->tail->name = p->child[0]->lexeme;
			t->tail->type = type;
			t->tail->tail = NULL;
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
		VarDec(p->child[0],arrayType,head,flag);
	}
	else 
		assert(0);
}

static Type Exp(Node *p);
static void Dec(Node *p,Type type,FieldList head,int flag){
	if(p->num_of_child==1){
		//Dec -> VarDec
		VarDec(p->child[0],type,head,flag);
	}
	else if(p->num_of_child==3){ 
		//Dec -> VarDec = Exp
		if(flag==1){
			VarDec(p->child[0],type,head,flag);
			SERROR(15);
		}
		else if (flag==2){
			
			VarDec(p->child[0],type,head,flag);
			if(!(strcmp(p->child[0]->child[0]->symbol,"ID")==0 && typeEq(type,Exp(p->child[2])))){
				SERROR(5);
			}
		}

	}
}
static void DecList(Node *p,Type type,FieldList head,int flag){
	if(p->num_of_child==1){
		//DecList -> Dec
		Dec(p->child[0],type,head,flag);
	}
	else if(p->num_of_child==3){
		//DecList -> Dec,DecList
		Dec(p->child[0],type,head,flag);
		DecList(p->child[2],type,head,flag);
			
	}
}

static Type Specifier(Node *p);
static void Def(Node *p,FieldList head,int flag){
	Type type = Specifier(p->child[0]);
	if(type==NULL){
		SERROR(17);
	}
	else
		DecList(p->child[1],type,head,flag);//name type
	
}
static void DefList(Node *p,FieldList head,int flag){
	if(!p)
		return ;
	if(p->child[1]){
		Def(p->child[0],head,flag);
		DefList(p->child[1],head,flag);
	}
	else{
		Def(p->child[0],head,flag);
	}
}
static Type StructSpecifier(Node *p){
	Type type = NULL;
	if(p->num_of_child==5){
		// structSpecifier -> struct OptTag { DefList  }  
		type = (Type) malloc(sizeof(struct Type_));
		type->kind = STRUCTURE;
		type->structure = NULL;
		if(p->child[3]){
		//	type->structure = DefList(p->child[3],1);
			FieldList head = (FieldList)malloc(sizeof(struct FieldList_));
			head->tail = NULL;
			DefList(p->child[3],head,1);
			type->structure = head->tail;
		}
		if(p->child[1]){
			int ret = insertSymTable(p->child[1]->child[0]->lexeme,Structure,type->structure,0);
			if(ret == 3|| ret ==4){
				SERROR(16);
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
static void ParamDec(Node *p,FieldList head,int flag){
	//ParamDec -> Specifier VarDec
	Type type = Specifier(p->child[0]);
	if(type==NULL){
		SERROR(17);
	}
	else{
		if(flag)	
			VarDec(p->child[1],type,head,3);
		else
			VarDec(p->child[1],type,head,1);
			
	}
}
static void VarList(Node *p,FieldList head,int flag){
//	FieldList field = (FieldList)malloc(sizeof(struct FieldList_));
	ParamDec(p->child[0],head,flag);//name type
	if (p->num_of_child==3){
		// VarList -> ParamDec , VarList
		VarList(p->child[2],head,flag);
	}
	else if (p->num_of_child==1){
		// VarList -> ParamDec
	}
	else
		assert(0);
}

static void FunDec(Node *p,Type ret_type,int flag){
	//flag = 1 :definition
	//flag = 0 :declaration
	ParaList_Ret para = (ParaList_Ret)malloc(sizeof(struct FieldList_));
	para->name = "return";
	para->type = ret_type;
	para->tail = NULL;
	if(p->num_of_child==4){
		// FunDec -> ID ( VarList )
		VarList(p->child[2],para,flag);
	}
	int lineno ;
	if(flag==1)
		lineno=0;
	else 
		lineno = p->lineno;
	int ret = insertSymTable(p->child[0]->lexeme,Function,para,lineno);	
	if(ret == 5){
		SERROR(4);
	}
	else if(ret == 6){
		SERROR(19);
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
				//ID | Exp.Exp | Exp[Exp]
				if(!typeEq(a,b))
					SERROR(5);
				else if((p->child[0]->num_of_child==1&&strcmp(p->child[0]->child[0]->symbol,"ID")==0)\
						||(p->child[0]->num_of_child==3&&strcmp(p->child[0]->child[1]->symbol,"DOT")==0)\
						||(p->child[0]->num_of_child==4&&strcmp(p->child[0]->child[1]->symbol,"LB")==0))
					return a;
				else
					SERROR(6);
			}
			else if(strcmp(op,"AND")==0 || strcmp(op,"OR")==0){
				if(!isInt(a)||!isInt(b))
					SERROR(7);	
				else
					return a;	
			}
			else if(strcmp(op,"RELOP")==0){
				if(!typeEq(a,b)){
					SERROR(7);
				}
				else if(!isInt(a)&&!isFloat(b))
					SERROR(7);
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
					SERROR(7);
				}
				else if(!isInt(a)&&!isFloat(b))
					SERROR(7);
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
				SERROR(13);
			else {
				Type b;
				if(!(b=isField(a->structure,p->child[2]->lexeme)))
					SERROR(14);
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
				SERROR(7);
			else
				return a;
		}
		else if (strcmp(op,"NOT")==0){
			if(!isInt(a))
				SERROR(7);
			return a;
		}
	}	
	else if(p->num_of_child==1)	{
		// Exp -> ID | INT | FLOAT	
		const char *sym = p->child[0]->symbol;
		if(strcmp(sym,"ID")==0){
			Type type ;
			if(!searchSymTable(p->child[0]->lexeme,Variable,(void **)&type,1))
				SERROR(1);
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
				SERROR(12);
			else{
				Type elem;
				if(!(elem=isArray(a)))
					SERROR(10);
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
					SERROR(11);
				else
					SERROR(2);
			}	
			else{
				if(p->num_of_child==3){
					if(para_ret->tail)
						SERROR(9);
				}
				else if(p->num_of_child==4){
					if(!argsEq(p->child[2],para_ret->tail))
						SERROR(9);
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
				SERROR(8);
			break;
		case 5:
			//Stmt -> IF (Exp) Stmt
			//  |  WHILE (Exp) Stmt
			if(!(t=Exp(p->child[2])))
				break;
			if(!isInt(t))
				SERROR(7);
			Stmt(p->child[4],type);
			break;
		case 7:
			//Stmt -> IF (Exp) Stmt ELSE Stmt
			if(!(t=Exp(p->child[2])))
				break;
			if(!isInt(t))
				SERROR(7);
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
	DefList(p->child[1],NULL,2);
	StmtList(p->child[2],type);
}
static void ExtDef(Node *p){
	Type type = Specifier(p->child[0]);
	if (strcmp(p->child[1]->symbol,"ExtDecList")==0){
		// ExtDef -> Specifier ExtDecList SEMI
		if(type==NULL){
			SERROR(17);
		}
		else
			ExtDecList(p->child[1],type);

	}
	else if(strcmp(p->child[1]->symbol,"SEMI")==0){
		// ExtDef -> Specifier SEMI
		//do nothing with definition(except structure)
	}
	else if(strcmp(p->child[1]->symbol,"FunDec")==0){
		// ExtDef -> Specifier FunDec CompSt/SEMI
		
		// new type may not be defined in a return type
		if(type==NULL){
			SERROR(17);
		}
		/*else*/{
			if(strcmp(p->child[2]->symbol,"CompSt")==0){
				FunDec(p->child[1],type,1);
				CompSt(p->child[2],type);	
			}
			else
				FunDec(p->child[1],type,0);
		}	
	}
}

static void semanticAnalysis(Node *p){
	ExtDef(p);
}

void semanticCheck(){
	initSymTable();
	traverseTree(semanticAnalysis);
	checkSymTable();
}
