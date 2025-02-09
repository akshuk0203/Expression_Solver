%{
	#include "parser.tab.h"
%}

%%
[0-9]+	{ yylval.str= strdup(yytext);
		return NUMBER; }
"=="	return EQ;
"!="	return NEQ;
"="		return ASSIGN;
"+"		return PLUS;
"-"		return MINUS;
"*"		return MUL;
"/"		return DIV;
"("		return LPAREN;
")"		return RPAREN;
"&"	return AND;
"|"	return OR;
">"		return GT;
"<"		return LT;
">="	return GE;
"<="	return LE;
"!"		return NOT;
"T"		return T;
"F"		return F;
[\t ]+                  ; 
[\n ]+                  ;
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str = strdup(yytext); return VARIABLE; }
.                       { 
                            fprintf(stderr, "Unknown character: '%s'\n", yytext); 
                            return yytext[0]; 
                        }
%%

int yywrap() {
    return 1;
}

