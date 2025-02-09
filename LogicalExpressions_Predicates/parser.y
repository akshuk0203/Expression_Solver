%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdbool.h>
	#include <ctype.h>
	#include "parser.tab.h"
	#include "functions.h"

	extern int yylex();
	extern int yyparse();
	extern FILE *yyin;
	void yyerror(const char *s);
%}

%union{
    char *str;
    struct Node *node;
	_Bool bval;
}

%token <bval> T F
%token <str> NUMBER VARIABLE BOOLEAN
%token PLUS MINUS MUL DIV ASSIGN LPAREN RPAREN AND OR NOT EQ NEQ GE LE GT LT
%type <node> expr term factor predicate boolean_expr
%nonassoc VARIABLE
%nonassoc ASSIGN
%right OR AND NOT
%left PLUS MINUS
%left MUL DIV

%%
input:
    | assignment input;

assignment: VARIABLE ASSIGN boolean_expr  { addVariable($1, $3); }

boolean_expr:T                      	{ $$ = createNode("T", BOOL_OPERAND, NULL, NULL); }
    | F                               	{ $$ = createNode("F", BOOL_OPERAND, NULL, NULL); }
    | boolean_expr AND boolean_expr     { $$ = createNode("&&", BOOLEAN_OP, $1, $3); }
    | boolean_expr OR boolean_expr      { $$ = createNode("||", BOOLEAN_OP, $1, $3); }
    | NOT boolean_expr                  { $$ = createNode("!" , BOOLEAN_OP, NULL, $2); } 
	|  predicate                        { $$ = $1; }
    ;

predicate:
    expr
    | expr EQ expr                  { $$ = createNode("==", PREDICATE_OP, $1, $3); }
	| expr NEQ expr                 { $$ = createNode("!=", PREDICATE_OP, $1, $3); }
    | expr GE expr                  { $$ = createNode(">=", PREDICATE_OP, $1, $3); }
    | expr LE expr                  { $$ = createNode("<=", PREDICATE_OP, $1, $3); }
    | expr GT expr                  { $$ = createNode(">", PREDICATE_OP, $1, $3); }
    | expr LT expr                  { $$ = createNode("<", PREDICATE_OP, $1, $3); }
    ;

expr:
    term
    | expr PLUS term 	{ $$ = createNode("+", NUMERIC_OP, $1, $3); }
    | expr MINUS term 	{ $$ = createNode("-", NUMERIC_OP, $1, $3); };

term:
    factor
    | term MUL factor { $$ = createNode("*", NUMERIC_OP, $1, $3); }
    | term DIV factor { $$ = createNode("/", NUMERIC_OP, $1, $3); };

factor:
    NUMBER 		{ $$ = createNode($1, NUM_OPERAND, NULL, NULL); }
    | VARIABLE 	{ $$ = createNode($1, VAR, NULL, NULL); }
    | LPAREN boolean_expr RPAREN { $$ = $2; }
    ;

%%

void yyerror(const char *s){
    fprintf(stderr, "Error: %s\n", s);
}

int main(int argc, char *argv[]){
    if (argc < 2) {
        fprintf(stderr, "Input file name missing\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file){
        fprintf(stderr, "Error opening file\n");
        return EXIT_FAILURE;
    }

    yyin = file;
    yyparse();
    fclose(file);

    for (int i = 0; i < variableCount; i++)
        extractDependencies(variables[i].expression, i);

    topologicalSort();
	freeVariables();
    return 0;
}
