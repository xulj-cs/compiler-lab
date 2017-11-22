#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include "type.h"
#include "node.h"
#define SIZE_OF_TABLE 0x7fff

//extern FieldList table[SIZE_OF_TABLE];
void initSymTable();
void updateSymTable(Node*);
void searchSymTable(Node*);
void printSymTable();
typedef struct symNode{
	char *name;
	enum{Variable,Structure,Function} kind;
	union{
		Type type;		//for variable 
		FieldList field;	//for struct definition
		ParaList_Ret para;	// for function definition
	}info;

	struct symNode* tail;
}symNode;
#endif

