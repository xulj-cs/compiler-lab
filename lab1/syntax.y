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

/* declared non-terminals */
%type <type_node> Program ExtDefList ExtDef ExtDecList 
%type <type_node> Specifier StructSpecifier OptTag Tag 
%type <type_node> VarDec FunDec VarList ParamDec
%type <type_node> CompSt StmtList Stmt
%type <type_node> DefList Def DecList Dec
%type <type_node> Exp Args

%%
/* High-level Definitions */

Program     : ExtDefList        {$$ = newNode("Program",@$.first_line);insertChild($$, 1, $1);root=$$;}
            ;

ExtDefList  : ExtDef ExtDefList     {$$ = newNode("ExtDefList",@$.first_line);insertChild($$, 2, $1, $2);}
            |  /* empty*/           {$$ = NULL;}
            ;
                
ExtDef      : Specifier ExtDecList SEMI     {$$ = newNode("ExtDef",@$.first_line);insertChild($$, 3, $1, $2, $3);}
            | Specifier SEMI                {$$ = newNode("ExtDef",@$.first_line);insertChild($$, 2, $1, $2);}    
            | Specifier FunDec CompSt       {$$ = newNode("ExtDef",@$.first_line);insertChild($$, 3, $1, $2, $3);}
            ;
ExtDecList  : VarDec                        {$$ = newNode("ExtDecList",@$.first_line);insertChild($$, 1, $1);} 
            | VarDec COMMA ExtDecList       {$$ = newNode("ExtDecList",@$.first_line);insertChild($$, 3, $1, $2, $3);} 
            ;

            

/* Specifiers */

Specifier   : TYPE                  {$$ = newNode("Specifier",@$.first_line);insertChild($$, 1, $1);}
            | StructSpecifier       {$$ = newNode("Specifier",@$.first_line);insertChild($$, 1, $1);}
            ;
            
StructSpecifier : STRUCT OptTag LC DefList RC   {$$ = newNode("StructSpecifier",@$.first_line);insertChild($$, 5, $1, $2, $3, $4, $5);}
                | STRUCT Tag                    {$$ = newNode("StructSpecifier",@$.first_line);insertChild($$, 2, $1, $2);}
                ;
            
OptTag  : ID                {$$ = newNode("OptTag",@$.first_line);insertChild($$, 1, $1);}
        | /* empty*/        {$$ = NULL;}
        ;
        
Tag     : ID                {$$ = newNode("Tag",@$.first_line);insertChild($$, 1, $1);}
        ;


/* Declarators */

VarDec      : ID                            {$$ = newNode("VarDec",@$.first_line);insertChild($$, 1, $1);}
            | VarDec LB INT RB              {$$ = newNode("VarDec",@$.first_line);insertChild($$, 4, $1, $2, $3, $4);}
            ;
            
FunDec      : ID LP VarList RP              {$$ = newNode("FunDec",@$.first_line);insertChild($$, 4, $1, $2, $3, $4);}
            | ID LP RP                      {$$ = newNode("FunDec",@$.first_line);insertChild($$, 3, $1, $2, $3);}
            ;
            
VarList     : ParamDec COMMA VarList        {$$ = newNode("VarList",@$.first_line);insertChild($$, 3, $1, $2, $3);}
            | ParamDec                      {$$ = newNode("VarList",@$.first_line);insertChild($$, 1, $1);}
            ;
            
ParamDec    : Specifier VarDec              {$$ = newNode("ParamDec",@$.first_line);insertChild($$, 2, $1, $2);}
            ;

            
/* Statements */

CompSt      : LC DefList StmtList RC        {$$ = newNode("CompSt",@$.first_line);insertChild($$, 4, $1, $2, $3, $4);}
            ;
            
StmtList    : Stmt StmtList                 {$$ = newNode("StmtList",@$.first_line);insertChild($$, 2, $1, $2);}
            | /*empty*/                     {$$ = NULL;}
            ;
            
Stmt    : Exp SEMI                          {$$ = newNode("Stmt",@$.first_line);insertChild($$, 2, $1, $2);}
        | CompSt                            {$$ = newNode("Stmt",@$.first_line);insertChild($$, 1, $1);}
        | RETURN Exp SEMI                   {$$ = newNode("Stmt",@$.first_line);insertChild($$, 3, $1, $2, $3);}
        | IF LP Exp RP Stmt ELSE Stmt       {$$ = newNode("Stmt",@$.first_line);insertChild($$, 7, $1, $2, $3, $4, $5, $6, $7);}
        | WHILE LP Exp RP Stmt              {$$ = newNode("Stmt",@$.first_line);insertChild($$, 5, $1, $2, $3, $4, $5);}
        ;


/* Local Definitions */

DefList : Def DefList               {$$ = newNode("DefList",@$.first_line);insertChild($$, 2, $1, $2);}
        | /*empty*/                 {$$ = NULL;}
        ;
        
Def     : Specifier DecList SEMI    {$$ = newNode("Def",@$.first_line);insertChild($$, 3, $1, $2, $3);}
        ;

DecList : Dec                       {$$ = newNode("DecList",@$.first_line);insertChild($$, 1, $1);}
        | Dec COMMA DecList         {$$ = newNode("DecList",@$.first_line);insertChild($$, 3, $1, $2, $3);}
        ;
        
Dec     : VarDec                    {$$ = newNode("Dec",@$.first_line);insertChild($$, 1, $1);}
        | VarDec ASSIGNOP Exp       {$$ = newNode("Dec",@$.first_line);insertChild($$, 3, $1, $2, $3);}
        ;
            
            
/* Expressions */
Exp     : Exp ASSIGNOP Exp      {$$ = newNode("Exp",@$.first_line);insertChild($$, 3, $1, $2, $3);}
        | Exp AND Exp           {$$ = newNode("Exp",@$.first_line);insertChild($$, 3, $1, $2, $3);}
        | Exp OR Exp            {$$ = newNode("Exp",@$.first_line);insertChild($$, 3, $1, $2, $3);}
        | Exp RELOP Exp         {$$ = newNode("Exp",@$.first_line);insertChild($$, 3, $1, $2, $3);}
        | Exp PLUS Exp          {$$ = newNode("Exp",@$.first_line);insertChild($$, 3, $1, $2, $3);}
        | Exp MINUS Exp         {$$ = newNode("Exp",@$.first_line);insertChild($$, 3, $1, $2, $3);}
        | Exp STAR Exp          {$$ = newNode("Exp",@$.first_line);insertChild($$, 3, $1, $2, $3);}
        | Exp DIV Exp           {$$ = newNode("Exp",@$.first_line);insertChild($$, 3, $1, $2, $3);}
        | LP Exp RP             {$$ = newNode("Exp",@$.first_line);insertChild($$, 3, $1, $2, $3);}
        | MINUS Exp             {$$ = newNode("Exp",@$.first_line);insertChild($$, 2, $1, $2);}
        | NOT Exp               {$$ = newNode("Exp",@$.first_line);insertChild($$, 2, $1, $2);}
        | ID LP Args RP         {$$ = newNode("Exp",@$.first_line);insertChild($$, 4, $1, $2, $3, $4);}
        | ID LP RP              {$$ = newNode("Exp",@$.first_line);insertChild($$, 3, $1, $2, $3);}
        | Exp LB Exp RB         {$$ = newNode("Exp",@$.first_line);insertChild($$, 4, $1, $2, $3, $4);}
        | Exp DOT ID            {$$ = newNode("Exp",@$.first_line);insertChild($$, 3, $1, $2, $3);}
        | ID                    {$$ = newNode("Exp",@$.first_line);insertChild($$, 1, $1);}
        | INT                   {$$ = newNode("Exp",@$.first_line);insertChild($$, 1, $1);}
        | FLOAT                 {$$ = newNode("Exp",@$.first_line);insertChild($$, 1, $1);}
        ;
    
Args    : Exp COMMA Args        {$$ = newNode("Exp",@$.first_line);insertChild($$, 3, $1, $2, $3);}
        | Exp                   {$$ = newNode("Exp",@$.first_line);insertChild($$, 1, $1);}
        ;


%%
int yyerror(const char* msg){
	printf("Error type B %s\n",msg);
}
