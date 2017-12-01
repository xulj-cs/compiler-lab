#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include "type.h"
#include "node.h"
#define SIZE_OF_TABLE 0x7fff

//extern FieldList table[SIZE_OF_TABLE];
void initSymTable();
void updateSymTable(Node*);
void checkSymTable();
void printSymTable();

typedef struct symNode{
	const char *name;
	enum{Variable,Structure,Function} kind;
	union{
		Type type;		//for variable 
		FieldList field;	//for struct definition
		ParaList_Ret para_ret;	// for function definition
	}info;
	int lineno ;	// for function : if defined then lines=0 ; else declartion
	struct symNode* tail;
}symNode;
#endif

