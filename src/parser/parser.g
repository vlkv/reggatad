%filenames parser
%scanner ../scanner/scanner.h
%scanner-token-function d_scanner.lex()

%token ID AND OR NOT LPAR RPAR

%%

startrule:
    expr_and
    {
        $$ = $1;
    }
;

expr_and:
    expr_and AND expr_or
    {
        $$ = std::shared_ptr<Node>(new OperAnd($1, $3));
    }
|
    expr_and expr_or
    {
        $$ = std::shared_ptr<Node>(new OperAnd($1, $2));
    }
|
    expr_or
    {
        $$ = $1;
    }
;

expr_or:
    expr_or OR expr_not
    {
        $$ = std::shared_ptr<Node>(new OperOr($1, $3));
    }
|
    expr_not
    {
        $$ = $1;
    }
;

expr_not:
    NOT atom
    {
        $$ = std::shared_ptr<Node>(new OperNot($2));
    }
|
    atom
    {
        $$ = $1;
    }
;

atom:
    tag
    {
        $$ = $1;
    }
|
    LPAR expr_and RPAR
    {
        $$ = $2;
    }
;

tag:
    ID
    {
        $$ = std::shared_ptr<Node>(new Tag($1));
    }
;
