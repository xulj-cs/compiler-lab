%{
	#include <stdio.h>
	#include "node.h"
	#include "lex.yy.c"
	int yyerror(const char* msg);
%}

/* declared type */
%union {
	Node* type_node;
}
/* declared tokens */
%token <type_node> INT FLOAT ID TYPE
%token <type_node> SEMI COMMA DOT 
%token <type_node> ASSIGNOP RELOP
%token <type_node> PLUS MINUS STAR DIV AND OR NOT 
%token <type_node> LP RP LB RB LC RC
%token <type_node> STRUCT RETURN IF ELSE WHILE

/* priority & association */
%right ASSIGNOP
%left OR
%left AND
%left RELOP 
%left PLUS MINUS
%left STAR DIV
%right NOT
%left DOT LP RP LB RB

/* solution to suspended-else problem */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

/* declared non-terminals */
%type <type_node> Program ExtDefList ExtDef ExtDecList 
%type <type_node> Specifier StructSpecifier OptTag Tag 
%type <type_node> VarDec FunDec VarList ParamDec
%type <type_node> CompSt StmtList Stmt
%type <type_node> DefList Def DecList Dec
%type <type_node> Exp Args

%%
/* High-level Definitions */

Program     : ExtDefList        {$$ = newNode("Program", 1, $1);root=$$;}
            ;

ExtDefList  : ExtDef ExtDefList     {$$ = newNode("ExtDefList", 2, $1, $2);}
            |  /* empty*/           {$$ = NULL;}
            ;
                
ExtDef      : Specifier ExtDecList SEMI     {$$ = newNode("ExtDef", 3, $1, $2, $3);}
            | Specifier SEMI                {$$ = newNode("ExtDef", 2, $1, $2);}    
            | Specifier FunDec CompSt       {$$ = newNode("ExtDef", 3, $1, $2, $3);}
            ;
ExtDecList  : VarDec                        {$$ = newNode("ExtDecList", 1, $1);} 
            | VarDec COMMA ExtDecList       {$$ = newNode("ExtDecList", 3, $1, $2, $3);} 
            ;

            

/* Specifiers */

Specifier   : TYPE                  {$$ = newNode("Specifier", 1, $1);}
            | StructSpecifier       {$$ = newNode("Specifier", 1, $1);}
            ;
            
StructSpecifier : STRUCT OptTag LC DefList RC   {$$ = newNode("StructSpecifier", 5, $1, $2, $3, $4, $5);}
                | STRUCT Tag                    {$$ = newNode("StructSpecifier", 2, $1, $2);}
                ;
            
OptTag  : ID                {$$ = newNode("OptTag", 1, $1);}
        | /* empty*/        {$$ = NULL;}
        ;
        
Tag     : ID                {$$ = newNode("Tag", 1, $1);}
        ;


/* Declarators */

VarDec      : ID                            {$$ = newNode("VarDec", 1, $1);}
            | VarDec LB INT RB              {$$ = newNode("VarDec", 4, $1, $2, $3, $4);}
            ;
            
FunDec      : ID LP VarList RP              {$$ = newNode("FunDec", 4, $1, $2, $3, $4);}
            | ID LP RP                      {$$ = newNode("FunDec", 3, $1, $2, $3);}
            ;
            
VarList     : ParamDec COMMA VarList        {$$ = newNode("VarList", 3, $1, $2, $3);}
            | ParamDec                      {$$ = newNode("VarList", 1, $1);}
            ;
            
ParamDec    : Specifier VarDec              {$$ = newNode("ParamDec", 2, $1, $2);}
            ;

            
/* Statements */

CompSt      : LC DefList StmtList RC        {$$ = newNode("CompSt", 4, $1, $2, $3, $4);}
            ;
            
StmtList    : Stmt StmtList                 {$$ = newNode("StmtList", 2, $1, $2);}
            | /*empty*/                     {$$ = NULL;}
            ;
            
Stmt    : Exp SEMI                          {$$ = newNode("Stmt", 2, $1, $2);}
        | CompSt                            {$$ = newNode("Stmt", 1, $1);}
        | RETURN Exp SEMI                   {$$ = newNode("Stmt", 3, $1, $2, $3);}
		| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$ = newNode("Stmt", 5, $1, $2, $3, $4, $5);}
        | IF LP Exp RP Stmt ELSE Stmt       {$$ = newNode("Stmt", 7, $1, $2, $3, $4, $5, $6, $7);}
        | WHILE LP Exp RP Stmt              {$$ = newNode("Stmt", 5, $1, $2, $3, $4, $5);}
        ;


/* Local Definitions */

DefList : Def DefList               {$$ = newNode("DefList", 2, $1, $2);}
        | /*empty*/                 {$$ = NULL;}
        ;
        
Def     : Specifier DecList SEMI    {$$ = newNode("Def", 3, $1, $2, $3);}
        ;

DecList : Dec                       {$$ = newNode("DecList", 1, $1);}
        | Dec COMMA DecList         {$$ = newNode("DecList", 3, $1, $2, $3);}
        ;
        
Dec     : VarDec                    {$$ = newNode("Dec", 1, $1);}
        | VarDec ASSIGNOP Exp       {$$ = newNode("Dec", 3, $1, $2, $3);}
        ;
            
            
/* Expressions */
Exp     : Exp ASSIGNOP Exp      {$$ = newNode("Exp", 3, $1, $2, $3);}
        | Exp AND Exp           {$$ = newNode("Exp", 3, $1, $2, $3);}
        | Exp OR Exp            {$$ = newNode("Exp", 3, $1, $2, $3);}
        | Exp RELOP Exp         {$$ = newNode("Exp", 3, $1, $2, $3);}
        | Exp PLUS Exp          {$$ = newNode("Exp", 3, $1, $2, $3);}
        | Exp MINUS Exp         {$$ = newNode("Exp", 3, $1, $2, $3);}
        | Exp STAR Exp          {$$ = newNode("Exp", 3, $1, $2, $3);}
        | Exp DIV Exp           {$$ = newNode("Exp", 3, $1, $2, $3);}
        | LP Exp RP             {$$ = newNode("Exp", 3, $1, $2, $3);}
        | MINUS Exp             {$$ = newNode("Exp", 2, $1, $2);}
        | NOT Exp               {$$ = newNode("Exp", 2, $1, $2);}
        | ID LP Args RP         {$$ = newNode("Exp", 4, $1, $2, $3, $4);}
        | ID LP RP              {$$ = newNode("Exp", 3, $1, $2, $3);}
        | Exp LB Exp RB         {$$ = newNode("Exp", 4, $1, $2, $3, $4);}
        | Exp DOT ID            {$$ = newNode("Exp", 3, $1, $2, $3);}
        | ID                    {$$ = newNode("Exp", 1, $1);}
        | INT                   {$$ = newNode("Exp", 1, $1);}
        | FLOAT                 {$$ = newNode("Exp", 1, $1);}
        ;
    
Args    : Exp COMMA Args        {$$ = newNode("Exp", 3, $1, $2, $3);}
        | Exp                   {$$ = newNode("Exp", 1, $1);}
        ;


%%
int yyerror(const char* msg){
	printf("Error type B %s\n",msg);
}
