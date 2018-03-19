%{
#include<stdio.h>
int char_num=0;
int word_num=0;
int line_num=0;
%}
%%
[' '\t] char_num++;
[0-9a-zA-Z]* {word_num++;char_num+=yyleng;}
[\n] {++line_num;char_num++;}
[^' '\n\t0-9a-zA-Z] char_num++;
%%
int main()
{
yylex();
printf("char_num is %d\n",char_num);
printf("word_num is %d\n",word_num);
printf("line_num is %d\n",line_num);
return 0;
}

