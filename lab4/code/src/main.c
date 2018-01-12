#include <stdio.h>
#include <stdlib.h>
extern FILE* yyin;
int yyparse();
void printAST();
void semanticCheck();
void InterCodeGen();
void print_ICROOT();
void print_ASMs();

int ERROR = 0;
int SERROR = 0;
FILE* fout = NULL;

int main(int argc, char** argv) {
    if (argc < 3)
		return 1;
	else {
        if (!(yyin = fopen(argv[1],"r"))) {
            perror(argv[1]);
            return 1;
        }
        if (!(fout = fopen(argv[2],"w"))) {
            perror(argv[2]);
            return 1;
        }
    }

	/*****  LAB1 *****/
    yyparse();
	if(ERROR)
		return -1;
	printAST();		
	/*****  LAB2 *****/
	semanticCheck();
	
	/*****  LAB3 *****/
	if(SERROR)
		return -1;
	InterCodeGen();
	//print_ICROOT();
	
	/*****  LAB4 *****/
	print_ASMs();
	
	fclose(fout);
	return 0;
}

