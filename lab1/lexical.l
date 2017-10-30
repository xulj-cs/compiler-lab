%{
	#include "syntax.tab.h"
	int fileno(FILE*);
	extern int ERROR;
%}

digit	[0-9]
letter	[_a-zA-Z]
DELIM	[ \t]

%option yylineno

%%
{DELIM}+						{}
\n								{
									printf("\n");
								}
"//"							{
									printf("comment1\n");
									char c = input();
									while(c!='\n') 
										c = input();
								}
"/*"							{	
									printf("comment2\n");
									int begin_lineno = yylineno;
									char a,b;
									a = input();
									b = input();
									while(!(a=='*'&&b=='/') && b!=EOF) {
										a = b;
										b = input();
									}
									if(b == EOF){
										printf("Error type B at Line %d: Unterminated comment\n",begin_lineno);
										ERROR++;
									}
								}						
({digit}+\.{digit}+)|((({digit}*\.{digit}+)|({digit}+\.{digit}*))[eE][+-]?{digit}+)	{ 
									printf("FLOAT\t");
									yylval.type_node = newLeaf("FLOAT");
									return FLOAT;	

								}
0|[1-9]{digit}*					{
									printf("INT\t");
									yylval.type_node = newLeaf("INT");
									return INT;	
								}
;								{
									printf("semi\t");
									yylval.type_node = newLeaf("SEMI");
									return SEMI;	

								}
,								{
									printf("comma\t");
									yylval.type_node = newLeaf("COMMA");
									return COMMA;	
										
								}
>|<|>=|<=|==|!=					{	printf("relop\t");
									yylval.type_node = newLeaf("RELOP");
									return RELOP;	
									
								}
=								{
									printf("assignop\t");
									yylval.type_node = newLeaf("ASSIGNOP");
									return ASSIGNOP;
								}
\+								{
									printf("plus\t");
									yylval.type_node = newLeaf("PLUS");
									return PLUS;	
								}
-								{	printf("minus\t");
									yylval.type_node = newLeaf("MINUS");
									return MINUS;	
								}
\*								{
									printf("star\t");
									yylval.type_node = newLeaf("STAR");
									return STAR;	
								}
\/								{
									printf("div\t");
									yylval.type_node = newLeaf("DIV");
									return DIV;	
								}
&&								{
									printf("and\t");
									yylval.type_node = newLeaf("AND");
									return AND;	
								}
\|\|							{	printf("or\t");
									yylval.type_node = newLeaf("OR");
									return OR;	
								}
\.								{printf("dot\t");
									yylval.type_node = newLeaf("DOT");
									return DOT;	
								}
!								{printf("not\t");
									yylval.type_node = newLeaf("NOT");
									return NOT;	
								}
\(								{
									printf("lp\t");
									yylval.type_node = newLeaf("LP");
									return LP;	
								}
\)								{
									printf("rp\t");
									yylval.type_node = newLeaf("RP");
									return RP;	
								}
\[								{
									printf("lb\t");
									yylval.type_node = newLeaf("LB");
									return LB;	
								}
\]								{
									printf("RB\t");
									yylval.type_node = newLeaf("RB");
									return RB;	
								}
\{								{
									printf("lc\t");
									yylval.type_node = newLeaf("LC");
									return LC;	
								}
\}								{
									printf("rc\t");
									yylval.type_node = newLeaf("RC");
									return RC;	
								}
int|float						{printf("type\t");
									yylval.type_node = newLeaf("TYPE");
									return TYPE;	
								}
struct							{
									printf("struct\t");
									yylval.type_node = newLeaf("STRUCT");
									return STRUCT;	
								}
return							{	printf("return\t");
									yylval.type_node = newLeaf("RETURN");
									return RETURN;	
								}
if								{	printf("if\t");
									yylval.type_node = newLeaf("IF");
									return IF;	
								}
else							{	printf("else\t");
									yylval.type_node = newLeaf("ELSE");
									return ELSE;	
								}
								
while							{	printf("while\t");
									yylval.type_node = newLeaf("WHILE");
									return WHILE;	
								}

{letter}({letter}|{digit})*		{	printf("ID\t");	
									yylval.type_node = newLeaf("ID");
									return ID;	
								}

.								{	printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno,yytext);
									ERROR++;
								}

%%
/*
int main(int argc, char** argv) {
	if (argc > 1) {
		if (!(yyin = fopen(argv[1],"r"))) {
			perror(argv[1]);
			return 1;
		}
	}
	//while (yylex() != 0) ;
	yylex();
	return 0;
}
*/
