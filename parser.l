%{
#include "parser.tab.h"
#define YY_DECL extern "C" int yylex()
%}

INDEX    [0-9]+
GATE     and|or|not|pass|xor
NODE     n|i|o

%%

{GATE}      { yylval.gate = yytext[0]; return GATE; }

{NODE}      { yylval.node = yytext[0]; return NODE; }

{INDEX}     { yylval.index = atoi(yytext); return INDEX; }

[\(\)=]     { return yytext[0]; }

inputs      { return INPUTS; }

end         { return END; }

.|\n ;/* ignore */
%%
;
