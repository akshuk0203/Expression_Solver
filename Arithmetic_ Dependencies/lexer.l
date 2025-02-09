%{
    #include "parser.tab.h"
%}

%%
[0-9]+			  	{ yylval.str = strdup(yytext); return NUMBER; }
"="               	return ASSIGN;
"+"                 return PLUS;
"-"                 return MINUS;
"*"                 return MUL;
"/"                 return DIV;
"("                 return LPAREN;
")"                 return RPAREN;
[\t ]+    			; 
[\n ]+              ;
[a-zA-Z_][a-zA-Z0-9_]*    { yylval.str = strdup(yytext); return VARIABLE; }
.                   { 
    					fprintf(stderr, "Unknown character: '%s'\n", yytext); 
    					return yytext[0]; 
					}
%%

int yywrap() {
    return 1;
}

