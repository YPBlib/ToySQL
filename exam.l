%{
#include<stdio.h>
int words=0;
%}
%%
stop printf("STOP!\n");
start printf("START!\n");
[a-zA-Z][0-9a-zA-Z]* printf("%d\n",++words);

