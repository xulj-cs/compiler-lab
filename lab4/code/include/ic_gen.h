#ifndef _IC_GEN_H_
#define _IC_GEN_H_

#include "interCode.h"
#include "type.h"

InterCodes *ic_gen_func_dec(const char *);
InterCodes *ic_gen_arg(const char *);
InterCodes *ic_gen_varlist(FieldList para);
InterCodes *ic_gen_assign_star(const char *,const char*);
InterCodes *ic_gen_assign_addr(const char *,const char*);
InterCodes *ic_gen_assign(const char *,const char*);
InterCodes *ic_gen_ari(const char *,const char*,const char *,const char *);
InterCodes *ic_gen_neg(const char*,const char *);
InterCodes *ic_gen_ret(const char *);
InterCodes *ic_gen_goto(const char *);
InterCodes *ic_gen_if(const char *,const char *,const char*,const char *);
InterCodes *ic_gen_label(const char*);
InterCodes *ic_gen_read(const char*);
InterCodes *ic_gen_write(const char*);
InterCodes *ic_gen_func_call(const char*,const char*);
InterCodes *ic_gen_dec(const char *,int);

#endif
