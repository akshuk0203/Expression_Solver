%{
    #include "parser.tab.h"
%}

%%
[0-9]+    { 
            yylval.str = strdup(yytext); 
            return NUMBER; 
          }

"("       { return LPAREN; }
")"       { return RPAREN; }
"+"       { return PLUS; }
"-"       { return MINUS; }
"*"       { return MUL; }
"/"       { return DIV; }
[\t\s]  	  { }
[\n]	  {return yytext[0];}
.         { printf("Invalid character: %s \n", yytext); }
%%

int yywrap() {
    return 1;
}
