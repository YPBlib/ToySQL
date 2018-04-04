%{
    #include<stdio.h>
    #include<stdlib.h>
    #include<math.h>
    int yylex(void);
    void yyerror(char*);
%}

%token INTEGER
%token DOUBLE
%%

expres:
    expres expre '\n' {printf("%d\n",$2);}
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
    ex '^' ex {double x=0.,y=0.,z=0.;x=$1,y=$3,z=pow(x,y);$$=z; }
    | ex {$$ = $1; }
    ;

ex:
    INTEGER {$$ = $1; }
    |DOUBLE {$$ = $1; }
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

