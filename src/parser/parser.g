%filenames parser
%scanner                ../scanner/scanner.h
%scanner-token-function d_scanner.lex()

%token ID AND OR NOT

%%

startrule:
    startrule tokenshow
|
    tokenshow
;

tokenshow:
    token
    {
        std::cout << "matched: " << d_scanner.matched() << '\n';
    }
;

token:
    ID
|
    AND
|
    OR
|
    NOT
;