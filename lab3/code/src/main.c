#include <stdio.h>
extern FILE* yyin;
extern FILE* fout;
int yyparse();
void printAST();
void semanticCheck();
void InterCodeGen();

int ERROR = 0;
int SERROR = 0;

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

	fclose(fout);
	return 0;
}

