%{
    #include<stdio.h>
    #include<stdlib.h>
    #define YYSTYPE double
    void yyerror(char*);
    char* string_tail;
    #include"y.tab.h"
%}
%%
[0-9]+ { yylval=(double)atoi(yytext); return INTEGER;}

[-.(/^)*+\n] return *yytext;

[ \t]

.     yyerror("invliad input");

%%

int yywrap(void)
{
    return 1;
}
