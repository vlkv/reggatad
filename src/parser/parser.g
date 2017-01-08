%filenames parser
%scanner ../scanner/scanner.h
%scanner-token-function d_scanner.lex()

%token ID AND OR NOT LPAR RPAR

%%

startrule:
    expr_and
;

expr_and:
    expr_and AND expr_or
|
    expr_and expr_or
|
    expr_or
;

expr_or:
    expr_or OR expr_not
|
    expr_not
;

expr_not:
    NOT oper
|
    oper;

oper:
    tag
|
    LPAR expr_and RPAR
;

tag:
    ID
;
