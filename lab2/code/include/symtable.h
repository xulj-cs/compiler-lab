#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include "type.h"
#include "node.h"
#define SIZE_OF_TABLE 0x7fff

extern FieldList table[SIZE_OF_TABLE];
void initSymTable();
void insertSymTable(Node*);
void searchSymTable(Node*);

#endif

