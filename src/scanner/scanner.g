// %debug // Uncomment to get debugging info on stdout
%filenames = "scanner"

%%

[ \t\n]+                            // skip white space chars.
[0-9]+                              return Parser::NUMBER;
[[:alpha:]_][[:alpha:][:digit:]_]*  return Parser::IDENTIFIER;
.                                   return Parser::CHAR;
