%{
    #include<stdio.h>
    #include<stdlib.h>
    #include<math.h>
    #define YYSTYPE double
    int yylex(void);
    void yyerror(char*);
%}

%token INTEGER

%%

expres:
    expres expre '\n' {printf("%lf\n",$2);}
    |
    ;



expre:
    expr {$$ = $1; }
    |expre '+' expr {$$ = $1 + $3; }
    | expre '-' expr {$$ = $1 - $3; }
    ;

expr:
    expr '*' exp {$$ = $1 * $3; }
    | expr '/' exp {$$ = $1 / $3; }
    | exp {$$ = $1; }
    ;

exp:
    ex '^' ex {int i=1,j=1;for(i=0;i<$3;++i)j*=$1; $$=j; }
    | ex {$$ = $1; }
    ;

ex:
    INTEGER {$$ = $1; }
    | '(' expre ')' {$$ = $2; }

%%

void yyerror(char*s)
{
    fprintf(stderr,"%s\n",s);
}

int main(void)
{
    yyparse();
    return 0;
}

