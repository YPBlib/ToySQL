%{
#include<stdio.h>

%}
qq \\\"
%%
{qq} printf("qq");
%%

int main()
{
yylex();
return 0;
}

