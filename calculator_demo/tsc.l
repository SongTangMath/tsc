
O   [0-7]
D   [0-9]
NZ  [1-9]
L   [a-zA-Z_]
A   [a-zA-Z_0-9]
H   [a-fA-F0-9]
HP  (0[xX])
E   ([Ee][+-]?{D}+)
P   ([Pp][+-]?{D}+)
FS  (f|F|l|L)
IS  (((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))
CP  (u|U|L)
SP  (u8|u|U|L)
ES  (\\(['"\?\\abfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+))
WS  [ \t\v\n\f]

%{
#include "a.h"
#include "tsc.tab.hpp"
#include "stdio.h"

extern "C" int yywrap(void)        /* called at end of input */
{
    printf("eof\n");
    return 1;           /* terminate now */
}

   
    
%}

%%

{HP}{H}+{IS}?				{ yylval.lexeme=std::shared_ptr<std::string>(new std::string(yytext)); return I_CONSTANT; }
{NZ}{D}*{IS}?				{ yylval.lexeme=std::shared_ptr<std::string>(new std::string(yytext)); return I_CONSTANT; }
"0"{O}*{IS}?				{ yylval.lexeme=std::shared_ptr<std::string>(new std::string(yytext)); return I_CONSTANT; }
{CP}?"'"([^'\\\n]|{ES})+"'"		{ yylval.lexeme=std::shared_ptr<std::string>(new std::string(yytext)); return I_CONSTANT; }

{D}+{E}{FS}?				{ yylval.lexeme=std::shared_ptr<std::string>(new std::string(yytext)); return F_CONSTANT; }
{D}*"."{D}+{E}?{FS}?			{ yylval.lexeme=std::shared_ptr<std::string>(new std::string(yytext)); return F_CONSTANT; }
{D}+"."{E}?{FS}?			{ yylval.lexeme=std::shared_ptr<std::string>(new std::string(yytext)); return F_CONSTANT; }
{HP}{H}+{P}{FS}?			{ yylval.lexeme=std::shared_ptr<std::string>(new std::string(yytext)); return F_CONSTANT; }
{HP}{H}*"."{H}+{P}{FS}?			{ yylval.lexeme=std::shared_ptr<std::string>(new std::string(yytext)); return F_CONSTANT; }
{HP}{H}+"."{P}{FS}?			{ yylval.lexeme=std::shared_ptr<std::string>(new std::string(yytext)); return F_CONSTANT; }

"("					{ yylval.lexeme=std::shared_ptr<std::string>(new std::string(yytext)); return LEFT_PARENTHESIS; }
")"					{ yylval.lexeme=std::shared_ptr<std::string>(new std::string(yytext)); return RIGHT_PARENTHESIS; }

("+"|"-") { yylval.lexeme=std::shared_ptr<std::string>(new std::string(yytext)); return Add_minus; }
("*"|"/"|"%") { yylval.lexeme=std::shared_ptr<std::string>(new std::string(yytext)); return Multiply_divide_mod; }


{WS}+					{ /* whitespace separates tokens */ printf("whitespace length %lu\n",strlen(yytext));}
.					{ /* discard bad characters */ }

%%

