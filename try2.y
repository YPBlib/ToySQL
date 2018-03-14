%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
int yyerror();
int yylex();
%} 
%token NUMBER

%%


%%

int yyerror(char *s) {
	fprintf(stderr,"error: %s\n",s);
	return -1;
}
