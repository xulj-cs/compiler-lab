#ifndef _TYPE_H_
#define _TYPE_H_
#include <stdbool.h>
#include <node.h>
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;

struct Type_{
	enum {BASIC,ARRAY,STRUCTURE} kind;
	union{
		enum {INT,FLOAT} basic;
		struct {
			Type elem;
			int size;
		}array;
		FieldList structure;
	};
};

struct FieldList_{
	const char *name;
	Type type;
	FieldList tail;
};
typedef FieldList ParaList_Ret;
bool typeEq(Type,Type);
bool fieldEq(FieldList,FieldList);
bool isInt(Type);
bool isFloat(Type);
bool isStruct(Type);
Type isField(FieldList,const char *);
Type isArray(Type);
Type typeofNode(Node *);
int sizeofType(Type);
#endif

