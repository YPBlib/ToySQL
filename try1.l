%{
#include<stdio.h>
#include"y.tab.h"
%}
%%
0|[1-9][0-9]* return NUMBER;
