#include <stdlib.h>
#include <stdio.h>
#include "interCode.h"

InterCodes* ICs;

void init_IC(){
	ICs = NULL;
}

void insert_IC(){

}

InterCode gen_IC(int kind){


}


static void print_IC(InterCode* ic){

	
}

void print_ICs(){
	InterCodes* p = ICs;
	while(p){
		print_IC(&p->code);	
	}	
}
