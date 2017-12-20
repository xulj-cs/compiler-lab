#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include <stdbool.h>
#include "type.h"
#include "node.h"
#define SIZE_OF_TABLE 0x7fff

//extern FieldList table[SIZE_OF_TABLE];

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
unsigned hash_pjw(const char *);
void initSymTable();
bool searchSymTable(const char *,int,void **,int);
int searchSymTable_f(const char *,ParaList_Ret,int);
int insertSymTable(const char *,int ,void *,int);
void checkSymTable();
int num_of_args(char *);
//void printSymTable();


#endif

