#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "node.h"

Node *root = NULL;

static void initNode(Node *p){
	p->num_of_child = 0;
	memset(p->child ,0, sizeof(Node*) * MAX_CHILD_NUM);
}


Node *newNode(const char* symbol,int lineno){
	Node *p = (Node *)malloc(sizeof(Node));
	strcpy(p->symbol,symbol);
	p->lineno = lineno;
	initNode(p);
	return p;
}
Node *newLeaf(const char*symbol,const char* lexeme){
	Node *p = (Node *)malloc(sizeof(Node));
	strcpy(p->symbol,symbol);
	strcpy(p->lexeme,lexeme);
	initNode(p);
	return p;
}
void insertChild(Node *parent,int num_of_child,...){
	va_list argList;
	va_start(argList,num_of_child);
	parent->num_of_child = num_of_child;
	for(int i=0;i<num_of_child;i++){
		parent->child[i] = va_arg(argList,Node *);
	}
	va_end(argList);
}

static void printSubtree(Node *p,int depth){
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
	printSubtree(root,0);
}
