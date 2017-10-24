%{
	#include "syntax.tab.h"
	#define YY_USER_ACTION \
		yylloc.first_line = yylineno; 
	int fileno(FILE*);
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
									printf("comment1\t");
									char c = input();
									while(c!='\n') 
										c = input();
								}
"/*"							{	
									printf("comment2\t");
									char a,b;
									a = input();
									b = input();
									while(!(a=='*'&&b=='/')) {
										a = b;
										b = input();
									}
								}						
({digit}+\.{digit}+)|((({digit}*\.{digit}+)|({digit}+\.{digit}*))[eE][+-]?{digit}+)	{ 
									printf("FLOAT\t");
									yylval.type_node = newLeaf("FLOAT",yytext);
									return FLOAT;	

								}
0|[1-9]{digit}*					{
									printf("INT\t");
									yylval.type_node = newLeaf("INT",yytext);
									return INT;	
								}
;								{
									printf("semi\t");
									yylval.type_node = newLeaf("SEMI",yytext);
									return SEMI;	

								}
,								{
									printf("comma\t");
									yylval.type_node = newLeaf("COMMA",yytext);
									return COMMA;	
										
								}
=								{
									printf("assignop\t");
									yylval.type_node = newLeaf("ASSIGNOP",yytext);
									return ASSIGNOP;
								}
>|<|>=|<=|==|!=					{	printf("relop\t");
									yylval.type_node = newLeaf("RELOP",yytext);
									return RELOP;	
									
								}
\+								{
									printf("plus\t");
									yylval.type_node = newLeaf("PLUS",yytext);
									return PLUS;	
								}
-								{	printf("minus\t");
									yylval.type_node = newLeaf("MINUS",yytext);
									return MINUS;	
								}
\*								{
									printf("star\t");
									yylval.type_node = newLeaf("STAR",yytext);
									return STAR;	
								}
\/								{
									printf("div\t");
									yylval.type_node = newLeaf("DIV",yytext);
									return DIV;	
								}
&&								{
									printf("and\t");
									yylval.type_node = newLeaf("AND",yytext);
									return AND;	
								}
\|\|							{	printf("or\t");
									yylval.type_node = newLeaf("OR",yytext);
									return OR;	
								}
\.								{printf("dot\t");
									yylval.type_node = newLeaf("DOT",yytext);
									return DOT;	
								}
!								{printf("not\t");
									yylval.type_node = newLeaf("NOT",yytext);
									return NOT;	
								}
int|float						{printf("type\t");
									yylval.type_node = newLeaf("TYPE",yytext);
									return TYPE;	
								}
\(								{
									printf("lp\t");
									yylval.type_node = newLeaf("LP",yytext);
									return LP;	
								}
\)								{
									printf("rp\t");
									yylval.type_node = newLeaf("RP",yytext);
									return RP;	
								}
\[								{
									printf("lb\t");
									yylval.type_node = newLeaf("LB",yytext);
									return LB;	
								}
\]								{
									printf("RB\t");
									yylval.type_node = newLeaf("RB",yytext);
									return RB;	
								}
\{								{
									printf("lc\t");
									yylval.type_node = newLeaf("LC",yytext);
									return LC;	
								}
\}								{
									printf("rc\t");
									yylval.type_node = newLeaf("RC",yytext);
									return RC;	
								}
struct							{
									printf("struct\t");
									yylval.type_node = newLeaf("STRUCT",yytext);
									return STRUCT;	
								}
return							{	printf("return\t");
									yylval.type_node = newLeaf("RETURN",yytext);
									return RETURN;	
								}
if								{	printf("if\t");
									yylval.type_node = newLeaf("IF",yytext);
									return IF;	
								}
else							{	printf("else\t");
									yylval.type_node = newLeaf("ELSE",yytext);
									return ELSE;	
								}
								
while							{	printf("while\t");
									yylval.type_node = newLeaf("WHILE",yytext);
									return WHILE;	
								}

{letter}({letter}|{digit})*		{	printf("ID\t");	
									yylval.type_node = newLeaf("ID",yytext);
									return ID;	
								}

.								{printf("Error type A at line %d: Mysterious character '%s'\t",yylineno,yytext);}

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
