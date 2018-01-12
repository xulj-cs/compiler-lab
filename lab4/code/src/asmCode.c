#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "interCode.h"

int sp_upd = 0;
int old_sp = 0;

extern InterCodes *ICROOT;
extern int st_top;
const char *ensure(const char *,int);
const char *ensure_v(const char *,const char *);
void storeDirtyVar();
void initRegs();
int get_var_addr(const char *);
void new_var_addr(const char *,int);
extern FILE* fout;
static void print_reg_i(const char *op,const char *x,const char *n){
	fprintf(fout,"%s %s, %s\n",op,ensure(x,1),n);
}
static void print_reg_2(const char *op,const char *x,const char *y){
	fprintf(fout,"%s %s, %s\n",op, ensure(x,1),ensure(y,0));
}
static void print_reg_2_i(const char *op,const char *x,const char *y,const char *n){
	fprintf(fout,"%s %s, %s, %s\n",op,ensure(x,1),ensure(y,0),n);
}
static void print_reg_3(const char *op,const char *x,const char *y,const char *z){
	fprintf(fout,"%s %s, %s, %s\n",op,ensure(x,1),ensure_v(y,z),ensure(z,0));
}

static void print_func(char *name){
	fprintf(fout,"\
jal %s\n\
",name);
}

void print_ASM(InterCode* ic){
	assert(ic);
	switch(ic->kind){
		case FUNC_DEC:
			initRegs();
			
			fprintf(fout,"\n%s:\n",ic->name);
			//保存返回地址（如果此函数中没有调用函数，则可以不需要保存）
			fprintf(fout,"addi $sp, $sp, -4\n");
			fprintf(fout,"sw $ra, 0($sp)\n");

			//保存旧的ebp
			fprintf(fout,"addi $sp, $sp, -4\n");
			fprintf(fout,"sw $fp, 0($sp)\n");

			//ebp = esp
			fprintf(fout,"move $fp, $sp\n");	
			st_top = 0;
			
			//为s0-s7保留空间
			fprintf(fout,"addi $sp, $sp ,-32\n");
			st_top -= 32;
			break;
		case LABEL:		
			storeDirtyVar();
			initRegs();
			fprintf(fout,"%s:\n",ic->name);
			break;
		case RET:
			//storeDirtyVar();再也用不到了
			
			fprintf(fout,"move $sp, $fp\n");

			fprintf(fout,"lw $fp, 0($sp)\n");
			fprintf(fout,"addi $sp, $sp, 4\n");

			fprintf(fout,"lw $ra, 0($sp)\n");
			fprintf(fout,"addi $sp, $sp ,4\n");
		
			fprintf(fout,"move $v0, %s\n",ensure(ic->name,0));
			fprintf(fout,"jr $ra\n");

			break;
		case ASSIGN:
		{
			Operand op1 = ic->assign.left;
			Operand op2 = ic->assign.right;
			if(op2->kind == CONSTANT){
				print_reg_i("li",op1->info,op2->info);
			}
			else{
				print_reg_2("move",op1->info,op2->info);
			}
			break;
		}
		case ADD:case SUB:
		{
			Operand op1 = ic->binop.op1;
			Operand op2 = ic->binop.op2;
			Operand op3 = ic->binop.result;
			if(op2->kind == CONSTANT){
				if(ic->kind == SUB){
					char *t = (char *)malloc(sizeof(char)*(2+strlen(op2->info)));
					strcpy(t+1,op2->info);
					t[0]='-';
					print_reg_2_i("addi",op3->info,op1->info,t);
				}
				else
					print_reg_2_i("addi",op3->info,op1->info,op2->info);
			}							
			else{
				if(ic->kind == ADD)
					print_reg_3("add",op3->info,op1->info,op2->info);
				else
					print_reg_3("sub",op3->info,op1->info,op2->info);
			}		
			break;		
		}
		case MUL:case DIV:
		{
			Operand op1 = ic->binop.op1;
			Operand op2 = ic->binop.op2;
			Operand op3 = ic->binop.result;
			const char *v = op2->info;
			if(op2->kind == CONSTANT){
				print_reg_i("li","@temp",op2->info);
				v = "@temp";
			}
			if(ic->kind == MUL){
				print_reg_3("mul",op3->info,op1->info,v);	
			}
			else{
				fprintf(fout,"div %s, %s\n",ensure_v(op1->info,v),ensure(v,0));
				fprintf(fout,"mflo %s\n",ensure(op3->info,1));
			}
			break;
	   	}
		case ASSIGN_STAR:	
							fprintf(fout,"lw %s, 0(%s)\n",ensure(ic->assign.left->info,1),ensure(ic->assign.right->info,0));
							break;
		case STAR_ASSIGN:	
							fprintf(fout,"sw %s, 0(%s)\n",ensure_v(ic->assign.right->info,ic->assign.left->info),ensure(ic->assign.left->info,0));
							break;
		case GOTO:	storeDirtyVar();
					fprintf(fout,"j %s\n",ic->name);
					break;
		case ARG:case PARAM:	
					assert(0);
					break;
		case FUNC_CALL:	
					initRegs();
					storeDirtyVar();
					fprintf(fout,"jal %s\n",ic->func.func_name);
					fprintf(fout,"move %s, $v0\n",ensure(ic->func.place->info,1));
					break;
		case IF:
		{
			storeDirtyVar();
			const char *op = ic->cond.op;
			char *s1 = ensure(ic->cond.right->info,0);
			char *s2 = ensure_v(ic->cond.left->info,ic->cond.right->info);
			if(strcmp(op,"==")==0)
				fprintf(fout,"beq");
			else if(strcmp(op,"!=")==0)
				fprintf(fout,"bne");
			else if(strcmp(op,">") ==0)
				fprintf(fout,"bgt");
			else if(strcmp(op,"<") ==0)
				fprintf(fout,"blt");
			else if(strcmp(op,">=")==0)
				fprintf(fout,"bge");
			else if(strcmp(op,"<=")==0)
				fprintf(fout,"ble");
			fprintf(fout," ");
			fprintf(fout,"%s, %s, %s\n",s2,s1,ic->cond.label);
			break;
		}
		case READ:	print_func("read");	
					fprintf(fout,"move %s, $v0\n",ensure(ic->op->info,1));
					break;
		case WRITE:	fprintf(fout,"move $a0, %s\n",ensure(ic->op->info,0));
					print_func("write");
					break;
		case DEC:	st_top -= ic->dec.size;
					new_var_addr(ic->dec.op->info,st_top);	
					break;
		case ASSIGN_ADDR:
					fprintf(fout,"addi %s, $fp, %d\n",ensure(ic->assign.left->info,1),get_var_addr(ic->assign.right->info));
					break;
		default:fprintf(fout,"TBD\n");
	}
}

void print_init(){
	fprintf(fout,"\
.data\n\
_prompt: .asciiz \"Enter an integer:\"\n\
_ret: .asciiz \"\\n\"\n\
.globl main\n\
.text\n\
read:\n\
li $v0, 4\n\
la $a0, _prompt\n\
syscall\n\
li $v0, 5\n\
syscall\n\
jr $ra\n\
\n\
write:\n\
li $v0, 1\n\
syscall\n\
li $v0, 4\n\
la $a0, _ret\n\
syscall\n\
move $v0, $0\n\
jr $ra\n\
");

}
void print_ASMs(){
	print_init();
	InterCodes* p = ICROOT;
	while(true){
		if(p->code->kind == ARG){
			storeDirtyVar();
			fprintf(fout,"addi $sp, $sp, %d\n",st_top+32);
			InterCodes *t = p;
			int cnt = 0;
			while(t->code->kind == ARG){
				t = t->next;
				cnt++;
			}
			//p = t;	

			for(;cnt>=5;cnt--){
				fprintf(fout,"addi $sp, $sp, -4\n");
				fprintf(fout,"lw %s,0($sp)\n",ensure(p->code->op->info,0));
				p = p->next;
			}
			for(;cnt>=1;cnt--){
				fprintf(fout,"move $a%d, %s\n",cnt-1,ensure(p->code->op->info,0));
				p = p->next;
			}
			assert(p==t);
		}
		
		else if(p->code->kind == PARAM){
			int cnt = 0;
			while(p->code->kind == PARAM){
				if(cnt <=3){
					fprintf(fout,"move %s,$a%d\n",ensure(p->code->op->info,1),cnt);
				}
				else{
					fprintf(fout,"lw %s, %d($sp)\n",ensure(p->code->op->info,1),(cnt-4+2)*4);
				}

				cnt ++;
				p = p->next;
			}
		}
		print_ASM(p->code);	
		p = p->next;	
		if(p==ICROOT)
			break;
	}
}
