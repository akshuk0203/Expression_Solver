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
}

%token <str> NUMBER VARIABLE
%token PLUS MINUS MUL DIV ASSIGN LPAREN RPAREN
%type <node> expr term factor

%%
input:
    | assignment input;

assignment:
    VARIABLE ASSIGN expr {
        addVariable($1, $3);
    };

expr:
    term
    | expr PLUS term { $$ = createNode("+", "operator", $1, $3); }
    | expr MINUS term { $$ = createNode("-", "operator", $1, $3); };

term:
    factor
    | term MUL factor { $$ = createNode("*", "operator", $1, $3); }
    | term DIV factor { $$ = createNode("/", "operator", $1, $3); };

factor:
    NUMBER { $$ = createNode($1, "operand", NULL, NULL); }
    | VARIABLE { $$ = createNode($1, "operand", NULL, NULL); }
    | LPAREN expr RPAREN { $$ = $2; };
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
    return 0;
}
