// %debug // Uncomment to get debugging info on stdout
%filenames = "scanner"

%%

[ \t\n]+                            // skip white space chars
[[:alpha:]_][[:alpha:][:digit:]_]*  return Parser::ID;
\&                                   return Parser::AND;
\|                                   return Parser::OR;
\!                                   return Parser::NOT;
