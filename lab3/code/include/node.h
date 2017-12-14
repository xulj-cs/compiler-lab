#ifndef _NODE_H_
#define _NODE_H_

//#define MAX_SYMBOL_NAME_LEN 20
#define MAX_LEXEME_NAME_LEN 32
#define MAX_CHILD_NUM 7

typedef struct AST_Node { //abstract syntax tree
//	char symbol[MAX_SYMBOL_NAME_LEN];
	char lexeme[MAX_LEXEME_NAME_LEN];	//for leaf
	const char *symbol;
//	const char *lexeme;
	int lineno;	//for internal node 
	struct AST_Node* child[MAX_CHILD_NUM];
	int num_of_child;
}Node;

extern Node* root;

Node *newLeaf(const char* symbol);
Node *newNode(const char* symbol,int num_of_child,...);
void printAST();

#endif
