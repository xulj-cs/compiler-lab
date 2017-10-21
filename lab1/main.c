#include <stdio.h>
#include "node.h"
extern FILE* yyin;
int yyparse();

int main(int argc, char** argv) {
    if (argc < 2)
		return 1;
	else {
        if (!(yyin = fopen(argv[1],"r"))) {
            perror(argv[1]);
            return 1;
        }
    }
    yyparse();
	printAST();
    return 0;
}

