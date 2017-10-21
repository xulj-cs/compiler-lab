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
								}
0|[1-9]{digit}*					{
									printf("INT\t");
									yylval.type_node = newLeaf("INT",yytext);
									return INT;	
								}
;								{printf("semi\t");	}
,								{printf("comma\t");	}
=								{
									//printf("assignop\t");
									yylval.type_node = newLeaf("ASSIGNOP","=");
									return ASSIGNOP;
								}
>|<|>=|<=|==|!=					{printf("relop\t");}
\+								{printf("plus\t");}
-								{printf("minus\t");}
\*								{printf("star\t");}
\/								{printf("div\t");}
&&								{printf("and\t");}
\|\|							{printf("or\t");}
\.								{printf("dot\t");}
!								{printf("not\t");}
int|float						{printf("type\t");}
\(								{printf("lp\t");}
\)								{printf("rp\t");}
\[								{printf("lb\t");}
\]								{printf("rb\t");}
\{								{printf("lc\t");}
\}								{printf("rc\t");	}
struct							{printf("struct\t");	}
return							{printf("return\t");	}
if								{printf("if\t");}
else							{printf("else\t");}
while							{printf("while\t");}

{letter}({letter}|{digit})*		{printf("ID\t");	}

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
