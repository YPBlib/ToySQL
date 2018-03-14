%{
int word_counter=0;
%}
numbers ([0-9])+
letter [A-Za-z]
chars [A-Za-z\_\'\"\.]
delim [" "\n\t]
whitespace {delim}+
words {chars}+

%%

{words} { wordCount++; /*
 increase the word count by one*/ }
 {whitespace} { /* do
 nothing*/ }
 {numbers} { /* one may
 want to add some processing here*/ }

%%

void main()
{
yylex();
printf("%d\n",wordCount);
}
int yywrap()
{
return 1;
}
