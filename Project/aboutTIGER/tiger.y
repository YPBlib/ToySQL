%{
#include<stdio.h>
#include<stdlib.h>
%}

%union
%token
%left
%right
%type
%start program


%%
program:
    expres
    |
    ;
%%
void yyerror(char *msg)
{
    fprintf(stderr,"%s\n",msg);
}

int main(void)
{
    yyparse();
    return 0;
}
