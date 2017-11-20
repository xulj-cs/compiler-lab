#ifndef _TYPE_H_
#define _TYPE_H_

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
	char *name;
	Type type;
	FieldList tail;
};

#endif

