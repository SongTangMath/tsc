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
#include "tsc.tab.hh"
#include "stdio.h"

extern "C" int yywrap(void)        /* called at end of input */
{
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
"#".*                                    { /* ignore preprocessor directives */ }

"auto"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(AUTO); }
"break"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(BREAK); }
"case"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(CASE); }
"char"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(CHAR); }
"const"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(CONST); }
"continue"				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(CONTINUE); }
"default"				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(DEFAULT); }
"do"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(DO); }
"double"				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(DOUBLE); }
"else"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(ELSE); }
"enum"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(ENUM); }
"extern"				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(EXTERN); }
"float"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(FLOAT); }
"for"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(FOR); }
"goto"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(GOTO); }
"if"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(IF); }
"inline"				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(INLINE); }
"int"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(INT); }
"long"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(LONG); }
"register"				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(REGISTER); }
"restrict"				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(RESTRICT); }
"return"				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(RETURN); }
"short"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(SHORT); }
"signed"				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(SIGNED); }
"sizeof"				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(SIZEOF); }
"static"				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(STATIC); }
"struct"				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(STRUCT); }
"switch"				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(SWITCH); }
"typedef"				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(TYPEDEF); }
"union"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(UNION); }
"unsigned"				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(UNSIGNED); }
"void"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(VOID); }
"volatile"				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(VOLATILE); }
"while"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return(WHILE); }
"_Alignas"                              { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return ALIGNAS; }
"_Alignof"                              { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return ALIGNOF; }
"_Atomic"                               { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return ATOMIC; }
"_Bool"                                 { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return BOOL; }
"_Complex"                              { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return COMPLEX; }
"_Generic"                              { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return GENERIC; }
"_Imaginary"                            { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return IMAGINARY; }
"_Noreturn"                             { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return NORETURN; }
"_Static_assert"                        { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return STATIC_ASSERT; }
"_Thread_local"                         { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return THREAD_LOCAL; }
"__func__"                              { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return FUNC_NAME; }

{L}{A}*					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return check_type(); }

{HP}{H}+{IS}?				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); yylval.lexeme_sub_type=I_CONSTANT_SUBTYPE_HEX_DIGIT; return I_CONSTANT; }
{NZ}{D}*{IS}?				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); yylval.lexeme_sub_type=I_CONSTANT_SUBTYPE_DECIMAL_DIGIT; return I_CONSTANT; }
"0"{O}*{IS}?				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); yylval.lexeme_sub_type=I_CONSTANT_SUBTYPE_OCTAL_DIGIT; return I_CONSTANT; }
{CP}?"'"([^'\\\n]|{ES})+"'"	{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); yylval.lexeme_sub_type=I_CONSTANT_SUBTYPE_CHAR_DIGIT; return I_CONSTANT; }

{D}+{E}{FS}?				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); yylval.lexeme_sub_type=F_CONSTANT_SUBTYPE_INTEGER_SIGNIFICANT_PART; return F_CONSTANT; }
{D}*"."{D}+{E}?{FS}?		{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); yylval.lexeme_sub_type=F_CONSTANT_SUBTYPE_FLOATING_SIGNIFICANT_PART; return F_CONSTANT; }
{D}+"."{E}?{FS}?			{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); yylval.lexeme_sub_type=F_CONSTANT_SUBTYPE_INTEGER_DOT_SIGNIFICANT_PART; return F_CONSTANT; }
{HP}{H}+{P}{FS}?			{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); yylval.lexeme_sub_type=F_CONSTANT_SUBTYPE_HEX_INTEGER_SIGNIFICANT_PART; return F_CONSTANT; }
{HP}{H}*"."{H}+{P}{FS}?		{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); yylval.lexeme_sub_type=F_CONSTANT_SUBTYPE_HEX_FLOATING_SIGNIFICANT_PART; return F_CONSTANT; }
{HP}{H}+"."{P}{FS}?			{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); yylval.lexeme_sub_type=F_CONSTANT_SUBTYPE_HEX_INTEGER_DOT_SIGNIFICANT_PART; return F_CONSTANT; }

({SP}?\"([^"\\\n]|{ES})*\"{WS}*)+	{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return STRING_LITERAL; }

"..."					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return ELLIPSIS; }
">>="					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return RIGHT_SHIFT_ASSIGN; }
"<<="					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return LEFT_SHIFT_ASSIGN; }
"+="					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return ADD_ASSIGN; }
"-="					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return SUB_ASSIGN; }
"*="					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return MUL_ASSIGN; }
"/="					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return DIV_ASSIGN; }
"%="					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return MOD_ASSIGN; }
"&="					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return AND_ASSIGN; }
"^="					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return XOR_ASSIGN; }
"|="					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return OR_ASSIGN; }
">>"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return RIGHT_SHIFT; }
"<<"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return LEFT_SHIFT; }
"++"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return INC_OP; }
"--"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return DEC_OP; }
"->"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return PTR_OP; }
"&&"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return AND_OP; }
"||"					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return OR_OP; }
"<="					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return LE_OP; }
">="					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return GE_OP; }
"=="					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return EQ_OP; }
"!="					{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return NE_OP; }
";"					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return SEMI_COLON; }
("{"|"<%")				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return LEFT_BRACE; }
("}"|"%>")				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return RIGHT_BRACE; }
","					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return COMMA; }
":"					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return COLON; }
"="					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return ASSIGN; }
"("					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return LEFT_PARENTHESIS; }
")"					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return RIGHT_PARENTHESIS; }
("["|"<:")				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return LEFT_BRACKET; }
("]"|":>")				{ yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return RIGHT_BRACKET; }
"."					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return DOT; }
"&"					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return BITAND; }
"!"					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return OPERATOR_NOT; }
"~"					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return BITNOT; }
"-"					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return SUB; }
"+"					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return ADD; }
"*"					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return MUL; }
"/"					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return DIV; }
"%"					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return MOD; }
"<"					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return LESS_THAN; }
">"					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return GREATER_THAN; }
"^"					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return BITXOR; }
"|"					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return BITOR; }
"?"					    { yylval.line_no=yyget_lineno(); yylval.lexeme=std::make_shared<std::string>(yytext); return QUESTION; }


[ \t]*"\r\n"     { yyset_lineno(yyget_lineno()+1); }
[ \t]*"\r"       { yyset_lineno(yyget_lineno()+1); }
[ \t]*"\n"       { yyset_lineno(yyget_lineno()+1); }
[ \t]+           { /* whitespace separates tokens */ }

.					    { /* discard bad characters */ }

%%


static void comment(void)
{
    int c;
    char previous = 0;

    while ((c = yyinput()) != 0){

    if(previous == '\r' && c == '\n' || c == '\r' || c == '\n')
        yyset_lineno(yyget_lineno()+1);

    previous = c;
        if (c == '*')
        {
            while ((c = yyinput()) == '*')
                ;

            if (c == '/')
                return;

            if (c == 0)
                break;
        }
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