#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "interCode.h"

extern InterCodes *ICROOT;
extern int st_top;
const char *ensure(const char *,int);
void storeDirty();
/*static void print_reg(const char *s){
	if(s[0]=='$')
		printf("%s",s);
	else{
		//查找x对应的寄存器--寄存器分配
		printf("%s",ensure(s));
	}
}
*/
static void print_reg_i(const char *op,const char *x,const char *n){
	printf("%s %s, %s\n",op,ensure(x,1),n);
}
static void print_reg_2(const char *op,const char *x,const char *y){
	printf("%s %s, %s\n",op, ensure(x,1),ensure(y,0));
}
static void print_reg_2_i(const char *op,const char *x,const char *y,const char *n){
	printf("%s %s, %s, %s\n",op,ensure(x,1),ensure(y,0),n);
}
static void print_reg_3(const char *op,const char *x,const char *y,const char *z){
	printf("%s %s, %s, %s\n",op,ensure(x,1),ensure(y,0),ensure(z,0));
}

static void print_func(char *name){
	printf("\
jal %s\n\
",name);
}

void print_ASM(InterCode* ic){
	assert(ic);
	switch(ic->kind){
		case LABEL:		storeDirtyVar();
						initRegs();
						printf("%s:\n",ic->name);
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
				assert(0);
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
				printf("div %s, %s\n",ensure(op1->info,0),ensure(v,0));
				printf("mflo %s\n",ensure(op3->info,1));
			}
			break;
	   	}
		case ASSIGN_STAR:	
							printf("lw %s, 0(%s)",ensure(ic->assign.left->info,1),ensure(ic->assign.right->info,0));
							break;
		case STAR_ASSIGN:	
							printf("sw %s, 0(%s)",ensure(ic->assign.left->info,0),ensure(ic->assign.right->info,0));
							break;
		case GOTO:	storeDirtyVar();
					printf("j %s\n",ic->name);break;
		case FUNC_CALL:	printf("jar %s\n",ic->func.func_name);
						printf("move %s, $v0\n",ensure(ic->func.place->info,1));
						break;
					break;
		case IF:
		{
			storeDirtyVar();
			const char *op = ic->cond.op;
			if(strcmp(op,"==")==0)
				printf("beq");
			else if(strcmp(op,"!=")==0)
				printf("bne");
			else if(strcmp(op,">") ==0)
				printf("bgt");
			else if(strcmp(op,"<") ==0)
				printf("blt");
			else if(strcmp(op,">=")==0)
				printf("bge");
			else if(strcmp(op,"<=")==0)
				printf("ble");
			printf(" ");
			printf("%s, %s, %s\n",ensure(ic->cond.left->info,0),ensure(ic->cond.right->info,0),ic->cond.label);
			break;
		}
		case READ:	print_func("read");	
					printf("move %s, $v0\n",ensure(ic->op->info,1));
					break;
		case WRITE:	printf("move $a0, %s\n",ensure(ic->op->info,0));
					print_func("write");
					break;
			
		default:printf("TBD\n");
	}
}

void print_init(){
	printf("\
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
\n\
");

}
void print_ASMs(){
	InterCodes* p = ICROOT;
	print_init();
	while(true){

		if(p->code->kind == FUNC_DEC){
			initStack();
			initRegs();
			
			printf("%s:\n",p->code->name);
			//保存返回地址（如果此函数中没有调用函数，则可以不需要保存）
			printf("addi $sp, $sp, -4\n");
			printf("sw $ra, 0($sp)\n");

			//保存旧的ebp
			printf("addi $sp, $sp, -4\n");
			printf("sw $fp, 0($sp)\n");

			//ebp = esp
			printf("move $fp, $sp\n");	
			st_top = 0;
			
			//为s0-s7保留空间
			printf("addi $sp, $sp ,-32\n");
			st_top -= 32;
			
			//为函数中的中间变量保留空间，并建立变量名与栈位置的映射关系Vars st_top
			
			p = p->next;
			while(p->code->kind != RET){
				print_ASM(p->code);
				p = p->next;	
			}

			//storeDirtyVar();再也用不到了
			
			printf("move $sp, $fp\n");

			printf("lw $fp, 0($sp)\n");
			printf("addi $sp, $sp, 4\n");

			printf("lw $ra, 0($sp)\n");
			printf("addi $sp, $sp ,4\n");
		
			printf("move $v0, %s\n",ensure(p->code->name,0));
			printf("jr $ra\n");
			
			p = p->next;	
		}

		if(p==ICROOT)
			break;
	}
}
