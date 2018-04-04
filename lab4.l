%{
    #include<stdio.h>
    #include<stdlib.h>
    #include<math.h>
    void yyerror(char*);
    #include"y.tab.h"
%}
%%
[0-9]+ { yylval=atoi(yytext); return INTEGER;}
([0-9])|[1-9][0-9]*\.[0-9]* { yylval=atof(yytext); return DOUBLE;}

[-)/^(*.+\n] return *yytext;

[ \t]

.     ;//  yyerror("invliad input");

%%

int yywrap(void)
{
    return 1;
}
