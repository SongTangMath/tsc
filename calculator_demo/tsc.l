
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

{HP}{H}+{IS}?				{ printf("I_CONSTANT1 %s\n",yytext); yylval.expression=yytext; return I_CONSTANT; }
{NZ}{D}*{IS}?				{ printf("I_CONSTANT2 %s\n",yytext); yylval.expression=yytext; return I_CONSTANT; }
"0"{O}*{IS}?				{ printf("I_CONSTANT3 %s\n",yytext); yylval.expression=yytext; return I_CONSTANT; }
{CP}?"'"([^'\\\n]|{ES})+"'"		{ printf("I_CONSTANT4 %s\n",yytext); yylval.expression=yytext; return I_CONSTANT; }

{D}+{E}{FS}?				{ printf("F_CONSTANT1 %s\n",yytext); yylval.expression=yytext; return F_CONSTANT; }
{D}*"."{D}+{E}?{FS}?			{ printf("F_CONSTANT2 %s\n",yytext); yylval.expression=yytext; return F_CONSTANT; }
{D}+"."{E}?{FS}?			{ printf("F_CONSTANT3 %s\n",yytext); yylval.expression=yytext; return F_CONSTANT; }
{HP}{H}+{P}{FS}?			{ printf("F_CONSTANT4 %s\n",yytext); yylval.expression=yytext; return F_CONSTANT; }
{HP}{H}*"."{H}+{P}{FS}?			{ printf("F_CONSTANT5 %s\n",yytext); yylval.expression=yytext; return F_CONSTANT; }
{HP}{H}+"."{P}{FS}?			{ printf("F_CONSTANT6 %s\n",yytext); yylval.expression=yytext; return F_CONSTANT; }

"("					{ printf("LEFT_PARENTHESIS %s\n",yytext); yylval.expression=yytext; return LEFT_PARENTHESIS; }
")"					{ printf("LEFT_PARENTHESIS %s\n",yytext); yylval.expression=yytext; return RIGHT_PARENTHESIS; }

("+"|"-") { printf("Add_minus\n"); yylval.expression=yytext; return Add_minus; }
("*"|"/"|"%") { printf("Multiply_divide_mod\n"); yylval.expression=yytext; return Multiply_divide_mod; }


{WS}+					{ /* whitespace separates tokens */ printf("whitespace length %lu\n",strlen(yytext));}
.					{ /* discard bad characters */ }

%%

