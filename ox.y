%{
#include<stdio.h>
#include<string.h>
void yywrap()
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
%token NUMBER TOKHEAT STATE TOKTARGET TOKTEMPERATURE


