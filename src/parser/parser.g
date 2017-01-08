%filenames parser
%scanner                ../scanner/scanner.h
%scanner-token-function d_scanner.lex()

%token IDENTIFIER NUMBER CHAR

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
    IDENTIFIER
|
    NUMBER
|
    CHAR
;