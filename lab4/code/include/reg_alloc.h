#ifndef _REG_ALLOC_H_
#define _REG_ALLOC_H_

typedef struct VarDesc{
	const char *name;
	int offset;
	struct VarDesc *next;
}VarDesc;

extern int st_top;
const char *ensure(const char*,int);
const char *ensure_v(const char *,const char*);
void storeDirtyVar();
void initRegs();
int get_var_addr(const char *);
void new_var_addr(const char *,int);

#endif
