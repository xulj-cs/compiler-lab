#include <string.h>
#include "symtable.h"

FieldList table[SIZE_OF_TABLE];

static unsigned int hash_pjw(char *name){
	unsigned int val = 0,i;
	for(; *name; name++){
		val = (val << 2) + *name;
		if( i = val & ~(SIZE_OF_TABLE))
			val = (val ^ (i>>12)) & SIZE_OF_TABLE;
	}
	return val;
}
void initSymTable(){
	memset(table,0,sizeof(FieldList)*SIZE_OF_TABLE);
}

void insertSymTable(Node *p){	//p->symbol = ExtDef OR Def
	/*	
	ExtDef:	Specifier ExtDecList SEMI
		|	Specifier SEMI
		|	Specifier FunDec CompSt
	Def	:	Specifier DecList SEMi
	*/	
}


void searchSymTable(Node *p){	//p->symbol = Exp
	/*
	Exp:...
	*/

}



