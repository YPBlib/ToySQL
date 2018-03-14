%{
#include<stdio.h>

int yywrap()
{
return 1;
}

int main(void)
{
 yyparse();
return 0;
}

int yyerror(msg)
char *msg;
{
        (void)fprintf(stderr, "%s\n", msg);
        return(0);
}

%}
%token NUMBER

%%
NUMBER:{printf("this is num");};
