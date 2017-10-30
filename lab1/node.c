#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "node.h"

extern int yylineno;
extern char *yytext;

Node *root = NULL;

static void initNode(Node *p){
	p->num_of_child = 0;
	memset(p->child ,0, sizeof(Node*) * MAX_CHILD_NUM);
}

Node *newLeaf(const char*symbol){
	Node *p = (Node *)malloc(sizeof(Node));
	strcpy(p->symbol,symbol);
	strcpy(p->lexeme,yytext);
	p->lineno = yylineno;
	initNode(p);
	return p;
}

Node *newNode(const char* symbol,int num_of_child,...){
	Node *p = (Node *)malloc(sizeof(Node));
	strcpy(p->symbol,symbol);
//	p->lineno = yylineno;
	initNode(p);

	// add children
	va_list argList;
	va_start(argList,num_of_child);
	p->num_of_child = num_of_child;
	for(int i=0;i<num_of_child;i++){
		p->child[i] = va_arg(argList,Node *);
	}
	va_end(argList);
	p->lineno = p->child[0]->lineno;
	return p;
}

static void printSubtree(Node *p,int depth){
	if (p == NULL)
		return ;	//for the non-terminals that product epsilon
	int t = depth;
	while(t--) 
		printf("  ");
	
	if(p->num_of_child){
		printf("%s (%d)\n",p->symbol,p->lineno);
		for(int i=0;i<p->num_of_child;i++){
			printSubtree(p->child[i],depth+1);
		}
	}	
	else{
		printf("%s: %s\n",p->symbol,p->lexeme);	
	}
}
void printAST(){
	printf("here we begin to print the tree... \n");
	printSubtree(root,0);
}
