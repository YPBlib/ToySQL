%{
    #define YYSTYPE double
    #include<stdio.h>
    #include<stdlib.h>
    #include<math.h>
    int yylex(void);
    void yyerror(char*);
%}

%token DOUBLE
%%

expres:
    expres expre '\n' {printf("%lf\n",$2);}
    |expres single_neg '\n' {printf("%lf\n",$2);} 
    |
    ;

single_neg:
'-' DOUBLE {$$= -$2; };

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
    DOUBLE {$$ = $1; }
    | '(' expre ')' {$$ = $2; }
    | '(' '-' DOUBLE ')' {$$ = -$3; }
    ;

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

