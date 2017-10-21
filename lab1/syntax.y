%{
	#include <stdio.h>
	#include "node.h"
	#include "lex.yy.c"
	int yyerror(const char* msg);
%}

/* declared type */
%union {
	Node* type_node;
}
/* declared tokens */
%token <type_node> INT FLOAT ID TYPE
%token <type_node> SEMI COMMA DOT 
%token <type_node> ASSIGNOP RELOP
%token <type_node> PLUS MINUS STAR DIV AND OR NOT 
%token <type_node> LP RP LB RB LC RC
%token <type_node> STRUCT RETURN IF ELSE WHILE

/* declared non-terminals */
%type <type_node>  Exp

%%

/* high-level definitions */
/* Program : ExtDefList
	;
ExtDefList : ExtDef ExtDefList { }
	|  empty 
	;
*/
/* expressions */
Exp : Exp ASSIGNOP Exp	{$$ = newNode("Exp",@$.first_line);insertChild($$,3,$1,$2,$3);root = $$;}
/*	| Exp PLUS Exp
	| Exp MINUS Exp 
	| ID		*/
	| INT				{$$ = newNode("Exp",@$.first_line);insertChild($$,1,$1);} 
/*	| FLOAT		*/
	;

%%
int yyerror(const char* msg){
	printf("Error type B %s\n",msg);
}
