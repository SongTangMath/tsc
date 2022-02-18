%e  1019
%p  2807
%n  371
%k  284
%a  1213
%o  1117

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
#include "parser.h"
#include "tsc.tab.hpp"
#include "stdio.h"

extern "C" int yywrap(void)        /* called at end of input */
{
    printf("eof\n");
    return 1;           /* terminate now */
}

extern void yyerror(const char *);  /* prints grammar violation message */

extern int sym_type(const char *);  /* returns type from symbol table */

#define sym_type(identifier) IDENTIFIER /* with no symbol table, fake it */

static void comment(void);
static int check_type(void);
%}

%%
"/*"                                    { comment(); }
"//".*                                    { /* consume //-comment */ }

"auto"					{ yylval.expression=yytext; return(AUTO); }
"break"					{ yylval.expression=yytext; return(BREAK); }
"case"					{ yylval.expression=yytext; return(CASE); }
"char"					{ yylval.expression=yytext; return(CHAR); }
"const"					{ yylval.expression=yytext; return(CONST); }
"continue"				{ yylval.expression=yytext; return(CONTINUE); }
"default"				{ yylval.expression=yytext; return(DEFAULT); }
"do"					{ yylval.expression=yytext; return(DO); }
"double"				{ yylval.expression=yytext; return(DOUBLE); }
"else"					{ yylval.expression=yytext; return(ELSE); }
"enum"					{ yylval.expression=yytext; return(ENUM); }
"extern"				{ yylval.expression=yytext; return(EXTERN); }
"float"					{ yylval.expression=yytext; return(FLOAT); }
"for"					{ yylval.expression=yytext; return(FOR); }
"goto"					{ yylval.expression=yytext; return(GOTO); }
"if"					{ yylval.expression=yytext; return(IF); }
"inline"				{ yylval.expression=yytext; return(INLINE); }
"int"					{ yylval.expression=yytext; return(INT); }
"long"					{ yylval.expression=yytext; return(LONG); }
"register"				{ yylval.expression=yytext; return(REGISTER); }
"restrict"				{ yylval.expression=yytext; return(RESTRICT); }
"return"				{ yylval.expression=yytext; return(RETURN); }
"short"					{ yylval.expression=yytext; return(SHORT); }
"signed"				{ yylval.expression=yytext; return(SIGNED); }
"sizeof"				{ yylval.expression=yytext; return(SIZEOF); }
"static"				{ yylval.expression=yytext; return(STATIC); }
"struct"				{ yylval.expression=yytext; return(STRUCT); }
"switch"				{ yylval.expression=yytext; return(SWITCH); }
"typedef"				{ yylval.expression=yytext; return(TYPEDEF); }
"union"					{ yylval.expression=yytext; return(UNION); }
"unsigned"				{ yylval.expression=yytext; return(UNSIGNED); }
"void"					{ yylval.expression=yytext; return(VOID); }
"volatile"				{ yylval.expression=yytext; return(VOLATILE); }
"while"					{ yylval.expression=yytext; return(WHILE); }
"_Alignas"                              { yylval.expression=yytext; return ALIGNAS; }
"_Alignof"                              { yylval.expression=yytext; return ALIGNOF; }
"_Atomic"                               { yylval.expression=yytext; return ATOMIC; }
"_Bool"                                 { yylval.expression=yytext; return BOOL; }
"_Complex"                              { yylval.expression=yytext; return COMPLEX; }
"_Generic"                              { yylval.expression=yytext; return GENERIC; }
"_Imaginary"                            { yylval.expression=yytext; return IMAGINARY; }
"_Noreturn"                             { yylval.expression=yytext; return NORETURN; }
"_Static_assert"                        { yylval.expression=yytext; return STATIC_ASSERT; }
"_Thread_local"                         { yylval.expression=yytext; return THREAD_LOCAL; }
"__func__"                              { yylval.expression=yytext; return FUNC_NAME; }

{L}{A}*					{ yylval.expression=yytext; return check_type(); }

{HP}{H}+{IS}?				{ yylval.expression=yytext; return I_CONSTANT; }
{NZ}{D}*{IS}?				{ yylval.expression=yytext; return I_CONSTANT; }
"0"{O}*{IS}?				{ yylval.expression=yytext; return I_CONSTANT; }
{CP}?"'"([^'\\\n]|{ES})+"'"		{ yylval.expression=yytext; return I_CONSTANT; }

{D}+{E}{FS}?				{ yylval.expression=yytext; return F_CONSTANT; }
{D}*"."{D}+{E}?{FS}?			{ yylval.expression=yytext; return F_CONSTANT; }
{D}+"."{E}?{FS}?			{ yylval.expression=yytext; return F_CONSTANT; }
{HP}{H}+{P}{FS}?			{ yylval.expression=yytext; return F_CONSTANT; }
{HP}{H}*"."{H}+{P}{FS}?			{ yylval.expression=yytext; return F_CONSTANT; }
{HP}{H}+"."{P}{FS}?			{ yylval.expression=yytext; return F_CONSTANT; }

({SP}?\"([^"\\\n]|{ES})*\"{WS}*)+	{ yylval.expression=yytext; return STRING_LITERAL; }

"..."					{ yylval.expression=yytext; return ELLIPSIS; }
">>="					{ yylval.expression=yytext; return RIGHT_SHIFT_ASSIGN; }
"<<="					{ yylval.expression=yytext; return LEFT_SHIFT_ASSIGN; }
"+="					{ yylval.expression=yytext; return ADD_ASSIGN; }
"-="					{ yylval.expression=yytext; return SUB_ASSIGN; }
"*="					{ yylval.expression=yytext; return MUL_ASSIGN; }
"/="					{ yylval.expression=yytext; return DIV_ASSIGN; }
"%="					{ yylval.expression=yytext; return MOD_ASSIGN; }
"&="					{ yylval.expression=yytext; return AND_ASSIGN; }
"^="					{ yylval.expression=yytext; return XOR_ASSIGN; }
"|="					{ yylval.expression=yytext; return OR_ASSIGN; }
">>"					{ yylval.expression=yytext; return RIGHT_SHIFT; }
"<<"					{ yylval.expression=yytext; return LEFT_SHIFT; }
"++"					{ yylval.expression=yytext; return INC_OP; }
"--"					{ yylval.expression=yytext; return DEC_OP; }
"->"					{ yylval.expression=yytext; return PTR_OP; }
"&&"					{ yylval.expression=yytext; return AND_OP; }
"||"					{ yylval.expression=yytext; return OR_OP; }
"<="					{ yylval.expression=yytext; return LE_OP; }
">="					{ yylval.expression=yytext; return GE_OP; }
"=="					{ yylval.expression=yytext; return EQ_OP; }
"!="					{ yylval.expression=yytext; return NE_OP; }
";"					{ yylval.expression=yytext; return SEMI_COLON; }
("{"|"<%")				{ yylval.expression=yytext; return LEFT_BRACE; }
("}"|"%>")				{ yylval.expression=yytext; return RIGHT_BRACE; }
","					{ yylval.expression=yytext; return COMMA; }
":"					{ yylval.expression=yytext; return COLON; }
"="					{ yylval.expression=yytext; return ASSIGN; }
"("					{ yylval.expression=yytext; return LEFT_PARENTHESIS; }
")"					{ yylval.expression=yytext; return RIGHT_PARENTHESIS; }
("["|"<:")				{ yylval.expression=yytext; return LEFT_BRACKET; }
("]"|":>")				{ yylval.expression=yytext; return RIGHT_BRACKET; }
"."					{ yylval.expression=yytext; return DOT; }
"&"					{ yylval.expression=yytext; return BITAND; }
"!"					{ yylval.expression=yytext; return OPERATOR_NOT; }
"~"					{ yylval.expression=yytext; return BITNOT; }
"-"					{ yylval.expression=yytext; return SUB; }
"+"					{ yylval.expression=yytext; return ADD; }
"*"					{ yylval.expression=yytext; return MUL; }
"/"					{ yylval.expression=yytext; return DIV; }
"%"					{ yylval.expression=yytext; return MOD; }
"<"					{ yylval.expression=yytext; return LESS_THAN; }
">"					{ yylval.expression=yytext; return GREATER_THAN; }
"^"					{ yylval.expression=yytext; return BITXOR; }
"|"					{ yylval.expression=yytext; return BITOR; }
"?"					{ yylval.expression=yytext; return QUESTION; }

{WS}+					{ /* whitespace separates tokens */ }
.					{ /* discard bad characters */ }

%%


static void comment(void)
{
    int c;

    while ((c = yyinput()) != 0)
        if (c == '*')
        {
            while ((c = yyinput()) == '*')
                ;

            if (c == '/')
                return;

            if (c == 0)
                break;
        }
    yyerror("unterminated comment");
}

static int check_type(void)
{
    switch (sym_type(yytext))
    {
    case TYPEDEF_NAME:                /* previously defined */
        return TYPEDEF_NAME;
    case ENUMERATION_CONSTANT:        /* previously defined */
        return ENUMERATION_CONSTANT;
    default:                          /* includes undefined */
        return IDENTIFIER;
    }
}