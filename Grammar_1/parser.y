%{
    #include "parser.h"
    #include<stdio.h>
    #include<stdlib.h>
    #include <string.h>
    #include <stdarg.h>

    int nextId = 0;
    int idsValue[101];
    char* ids[101];
    void free_node(Node *p);
    Node* add_node(int opr, int count,...);
    Node* create_node(int op, int type);
    int getId(char* var_name);
    Node* generate_code(Node *p);
    int yylex(void);
    void yyerror (char *s);

%}

%union {int num; char* id; struct Node* nPtr;}

%token <num> NUM
%token <id>VAR
%token WHILE IF FUN WRITE_VALUE READ_VALUE DO
%nonassoc IFX
%nonassoc ELSE
%left GE LE EE NE AND OR '>' '<'
%left '+' '-'
%left '*' '/'
%type <nPtr> Stmt E Stmts IfStmt IfElseStmt AsgStmt ReadStmt WriteStmt WhileStmt

%start Program

%%

Program : FUN'('')''{'Stmts'}'              { generate_code($5); free_node($5); }
        ;
Stmts   :   Stmts Stmt                      { $$ = add_node(';',2, $1, $2); }
        |   Stmt                            { $$=$1;}
        ;                             
Stmt    : AsgStmt
        | ReadStmt
        | WriteStmt
        | IfStmt
        | IfElseStmt
        | WhileStmt
        ;
AsgStmt : VAR'='E';'                              { $$=add_node('=',2,create_node(getId($1),VARIABLE_TYPE),$3);}
        ;
IfStmt      : IF'('E')' '{'Stmts'}' %prec IFX     { $$=add_node(IF,2,$3,$6);}
            ;
IfElseStmt  : IF'('E')''{'Stmts'}'ELSE'{'Stmts'}' { $$=add_node(IF,3,$3,$6,$10);}
            ;
WhileStmt   : DO '{' Stmts '}' WHILE '('E')'';'   { $$=add_node(WHILE,2,$3,$7);}
            ;
ReadStmt    : READ_VALUE'('VAR')'';'   { $$ = add_node(READ_VALUE,1,create_node(getId($3),VARIABLE_TYPE));};
            ;
WriteStmt   : WRITE_VALUE'('E')'';'    { $$ = add_node(WRITE_VALUE,1,$3);}
            ;
E       :   NUM                     { $$ = create_node($1,NUM_TYPE); }
        |   VAR                     { $$ = create_node(getId($1),VARIABLE_TYPE); }
        |   E '+' E                 { $$ = add_node('+',2,$1,$3);}
        |   E '*' E                 { $$ = add_node('*',2,$1,$3);}
        |   E '>' E                 { $$ = add_node('>',2,$1,$3);}
        |   E '<' E                 { $$ = add_node('<',2,$1,$3);}
        |   E LE E                  { $$ = add_node(LE,2,$1,$3);}
        |   E GE E                  { $$ = add_node(GE,2,$1,$3);}
        |   E NE E                  { $$ = add_node(NE,2,$1,$3);}
        |   E EE E                  { $$ = add_node(EE,2,$1,$3);}
        |   '('E')'                 { $$ = $2;}
        ;
%%

int getId(char* var_name){
    for(int i=0;i<nextId;i++){
        if(strcmp(var_name,ids[i])==0){
            return i;
        }
    }

    ids[nextId] = strdup(var_name);
    return nextId++;
}

void setValue(int id,int value){
    idsValue[id] = value;
}

int getValue(int id){
    return idsValue[id];
}

Node *create_node(int operand,int type){
    Node *node = (Node*)malloc(sizeof(Node));
    if(node == NULL)
        yyerror("out of memory");
    
    node->oper = operand;
    node->type = type;
    node->child_count = 0;
    
    for(int i=0;i<3;i++){
        node->children[i] = NULL;
    }

    return node;
}

Node *add_node(int opr, int count, ...) {
    Node *node = (Node*)malloc(sizeof(Node));
    if(node == NULL)
        yyerror("out of memory");

    va_list list;

    node->child_count = count;
    node->oper = opr;
    node->type = OP_TYPE;

    for(int i=0;i<3;i++){
        node->children[i] = NULL;
    }

    va_start(list, count);
    for(int i=0;i<count;i++){
        node->children[i] = va_arg(list, Node*);
    }
    va_end(list);
    
    return node;
}

void free_node(Node *p) {
    if (!p) return;
    /* for (int i = 0; i < p->child_count; i++)
        free_node(p->children[i]); */
    
    free (p);
}

int main(void){

    for(int i=0;i<101;i++){
        idsValue[i] = 0;
        ids[i] = NULL;
    }
    return yyparse();
}

void yyerror(char* s){fprintf(stderr,"%s\n",s);}