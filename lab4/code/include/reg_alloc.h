#ifndef _REG_ALLOC_H_
#define _REG_ALLOC_H_

typedef struct VarDesc{
	const char *name;
	int offset;
	struct VarDesc *next;
}VarDesc;




#endif
