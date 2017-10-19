%{

%}


digit	[0-9]
letter	[_a-zA-Z]
DELIM	[ \t]

%option yylineno

%%
{DELIM}+						{printf("WS\t");}
\n								{printf("\n");}
0|[1-9]{digit}*					{printf("INT\t");}
{digit}+\.{digit}+				{printf("FLOAT\t");}
{letter}({letter}|{digit})*		{printf("ID\t");}
.								{printf("Error type A at line %d: Mysterious character '%s'\t",yylineno,yytext);}

%%

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
