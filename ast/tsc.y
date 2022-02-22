
%{
#include <stdio.h>
#include "parser.h"
#include "lex.yy.h"
#include "tsc.tab.hpp"

#define YYERROR_VERBOSE 1  
int yylex ();
void yyerror(const char *s)
{
	printf("error line %d %s\n",yyget_lineno(), s);
}  
%}


%token	IDENTIFIER I_CONSTANT F_CONSTANT STRING_LITERAL FUNC_NAME SIZEOF
%token	PTR_OP INC_OP DEC_OP LEFT_SHIFT RIGHT_SHIFT LE_OP GE_OP EQ_OP NE_OP
%token	AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token	SUB_ASSIGN LEFT_SHIFT_ASSIGN RIGHT_SHIFT_ASSIGN AND_ASSIGN
%token	XOR_ASSIGN OR_ASSIGN
%token	TYPEDEF_NAME ENUMERATION_CONSTANT

%token	TYPEDEF EXTERN STATIC AUTO REGISTER INLINE
%token	CONST RESTRICT VOLATILE
%token	BOOL CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE VOID
%token	COMPLEX IMAGINARY 
%token	STRUCT UNION ENUM ELLIPSIS

%token	CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%token	ALIGNAS ALIGNOF ATOMIC GENERIC NORETURN STATIC_ASSERT THREAD_LOCAL

%token SEMI_COLON COMMA COLON 
%token LEFT_BRACE RIGHT_BRACE LEFT_BRACKET RIGHT_BRACKET LEFT_PARENTHESIS RIGHT_PARENTHESIS
%token OPERATOR_NOT BITNOT ADD SUB MUL DIV MOD ASSIGN
%token GREATER_THAN LESS_THAN BITAND BITXOR BITOR QUESTION DOT

%start translation_unit
%%

primary_expression
	: IDENTIFIER
	{
        $$.node_type = NODE_TYPE_PRIMARY_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_IDENTIFIER;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }
	| constant 
	{
		/* 这里有二义性.constant可以推出 IDENTIFIER constant实际上要在语义分析才能确定 */
        $$.node_type = NODE_TYPE_PRIMARY_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_CONSTANT;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }
	| string
	{
        $$.node_type = NODE_TYPE_PRIMARY_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_STRING;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }
	| LEFT_PARENTHESIS expression RIGHT_PARENTHESIS
	{
        $$.node_type = NODE_TYPE_PRIMARY_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_LEFT_PARENTHESIS_EXPRESSION_RIGHT_PARENTHESIS;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); // LEFT_PARENTHESIS
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); // expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); // RIGHT_PARENTHESIS
    }
	| generic_selection /* C11 generic_selection */
	{
        $$.node_type = NODE_TYPE_PRIMARY_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_GENERIC_SELECTION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }
	;

constant
	: I_CONSTANT
	{
		/* includes character_constant */
        $$.node_type = NODE_TYPE_CONSTANT;
        $$.node_sub_type = NODE_TYPE_CONSTANT_SUBTYPE_ICONSTANT;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }		
	| F_CONSTANT
	{		
        $$.node_type = NODE_TYPE_CONSTANT;
        $$.node_sub_type = NODE_TYPE_CONSTANT_SUBTYPE_FCONSTANT;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }	
	| ENUMERATION_CONSTANT	
	{
		/* after it has been defined as such */
        $$.node_type = NODE_TYPE_CONSTANT;
        $$.node_sub_type = NODE_TYPE_CONSTANT_SUBTYPE_ENUMERATION_CONSTANT;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }	
	;

enumeration_constant		/* before it has been defined as such */
	: IDENTIFIER
	{
        $$.node_type = NODE_TYPE_ENUMERATION_CONSTANT;
        $$.node_sub_type = NODE_TYPE_ENUMERATION_CONSTANT_SUBTYPE_IDENTIFIER;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }	
	;

string
	: STRING_LITERAL 
	{
		/* 支持多个字符串连起来如 "a" "b"等价于 "ab" */
        $$.node_type = NODE_TYPE_STRING;
        $$.node_sub_type = NODE_TYPE_STRING_SUBTYPE_STRING_LITERAL;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }
	| FUNC_NAME 
	{
		/* 在任何函数内部 "__func__" 都隐式表示当前函数名字 */
        $$.node_type = NODE_TYPE_STRING;
        $$.node_sub_type = NODE_TYPE_STRING_SUBTYPE_FUNC_NAME;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }	
	;

generic_selection
	: GENERIC LEFT_PARENTHESIS assignment_expression COMMA generic_assoc_list RIGHT_PARENTHESIS
	{
        $$.node_type = NODE_TYPE_GENERIC_SELECTION;
        $$.node_sub_type = NODE_TYPE_GENERIC_SELECTION_SUBTYPE_GENERIC_LEFT_PARENTHESIS_ASSIGNMENT_EXPRESSION_COMMA_GENERIC_ASSOC_LIST_RIGHT_PARENTHESIS;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //GENERIC
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //assignment_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //COMMA
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //generic_assoc_list
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($6))); //RIGHT_PARENTHESIS		
    }	
	;

generic_assoc_list
	: generic_association
	{
        $$.node_type = NODE_TYPE_GENERIC_ASSOC_LIST;
        $$.node_sub_type = NODE_TYPE_GENERIC_ASSOC_LIST_SUBTYPE_GENERIC_ASSOCIATION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }
	| generic_assoc_list COMMA generic_association
	{
        $$.node_type = NODE_TYPE_GENERIC_ASSOC_LIST;
        $$.node_sub_type = NODE_TYPE_GENERIC_ASSOC_LIST_SUBTYPE_GENERIC_ASSOC_LIST_COMMA_ASSOCIATION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //generic_assoc_list
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COMMA
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //generic_association
    }
	;

generic_association
	: type_name COLON assignment_expression
	{
        $$.node_type = NODE_TYPE_GENERIC_ASSOCIATION;
        $$.node_sub_type = NODE_TYPE_GENERIC_ASSOCIATION_SUBTYPE_TYPE_NAME_COLON_ASSIGNMENT_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_name
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COLON
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //assignment_expression
    }
	| DEFAULT COLON assignment_expression
	{
        $$.node_type = NODE_TYPE_GENERIC_ASSOCIATION;
        $$.node_sub_type = NODE_TYPE_GENERIC_ASSOCIATION_SUBTYPE_DEFAULT_COLON_ASSIGNMENT_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //DEFAULT
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COLON
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //assignment_expression
    }
	;

postfix_expression
	: primary_expression
	{
        $$.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_PRIMARY_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //primary_expression	
    }
	| postfix_expression LEFT_BRACKET expression RIGHT_BRACKET
	{
        $$.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_LEFT_BRACKET_EXPRESSION_RIGHT_BRACKET;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //postfix_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_BRACKET
    }
	| postfix_expression LEFT_PARENTHESIS RIGHT_PARENTHESIS
	{
        $$.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_LEFT_PARENTHESIS_RIGHT_PARENTHESIS;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //postfix_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_PARENTHESIS
    }
	| postfix_expression LEFT_PARENTHESIS argument_expression_list RIGHT_PARENTHESIS
	{
        $$.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_LEFT_PARENTHESIS_ARGUMENT_EXPRESSION_LIST_RIGHT_PARENTHESIS;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //DEFAULT
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COLON
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //assignment_expression
    }
	| postfix_expression DOT IDENTIFIER
	{
        $$.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_DOT_IDENTIFIER;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //postfix_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //DOT
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //IDENTIFIER
    }
	| postfix_expression PTR_OP IDENTIFIER
	{
        $$.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_PTR_OP_IDENTIFIER;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //DEFAULT
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //PTR_OP
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //IDENTIFIER
    }
	| postfix_expression INC_OP
	{
        $$.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_INC_OP;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //postfix_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //INC_OP
    }
	| postfix_expression DEC_OP
	{
        $$.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_DEC_OP;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //postfix_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //DEC_OP
    }
	| LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS LEFT_BRACE initializer_list RIGHT_BRACE
	{
        $$.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS_LEFT_BRACE_INITIALIZER_LIST_RIGHT_BRACE;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_PARENTHESIS
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //type_name
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_PARENTHESIS
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //LEFT_BRACE
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //initializer_list
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($6))); //RIGHT_BRACE
    }
	| LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS LEFT_BRACE initializer_list COMMA RIGHT_BRACE
	{
        $$.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS_LEFT_BRACE_INITIALIZER_LIST_COMMA_RIGHT_BRACE;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_PARENTHESIS
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //type_name
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_PARENTHESIS
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //LEFT_BRACE
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //initializer_list
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($6))); //COMMA
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($7))); //RIGHT_BRACE
    }
	;

argument_expression_list
	: assignment_expression
	{
        $$.node_type = NODE_TYPE_ARGUMENT_EXPRESSION_LIST;
        $$.node_sub_type = NODE_TYPE_ARGUMENT_EXPRESSION_LIST_SUBTYPE_ASSIGNMENT_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //assignment_expression
    }
	| argument_expression_list COMMA assignment_expression
	{
        $$.node_type = NODE_TYPE_ARGUMENT_EXPRESSION_LIST;
        $$.node_sub_type = NODE_TYPE_ARGUMENT_EXPRESSION_LIST_SUBTYPE_ARGUMENT_EXPRESSION_LIST_COMMA_ASSIGNMENT_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //argument_expression_list
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COMMA
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //assignment_expression
    }
	;

unary_expression
	: postfix_expression
	{
        $$.node_type = NODE_TYPE_UNARY_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //postfix_expression
    }
	| INC_OP unary_expression
	{
        $$.node_type = NODE_TYPE_UNARY_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_INC_OP_UNARY_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //INC_OP
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //unary_expression
    }
	| DEC_OP unary_expression
	{
        $$.node_type = NODE_TYPE_UNARY_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_DEC_OP_UNARY_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //DEC_OP
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //unary_expression
    }
	| unary_operator cast_expression
	{
        $$.node_type = NODE_TYPE_UNARY_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_UNARY_OPERATOR_CAST_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //unary_operator
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //cast_expression
    }
	| SIZEOF unary_expression
	{
        $$.node_type = NODE_TYPE_UNARY_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_SIZEOF_UNARY_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //SIZEOF
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //unary_expression
    }
	| SIZEOF LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS
	{
        $$.node_type = NODE_TYPE_UNARY_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_SIZEOF_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //SIZEOF
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //type_name
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_PARENTHESIS
    }
	| ALIGNOF LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS
	{
        $$.node_type = NODE_TYPE_UNARY_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_ALIGNOF_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //ALIGNOF
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //type_name
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_PARENTHESIS
    }
	;

unary_operator
	: BITAND
	{
        $$.node_type = NODE_TYPE_UNARY_OPERATOR;
        $$.node_sub_type = NODE_TYPE_UNARY_OPERATOR_SUBTYPE_BITAND;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }
	| MUL
	{
        $$.node_type = NODE_TYPE_UNARY_OPERATOR;
        $$.node_sub_type = NODE_TYPE_UNARY_OPERATOR_SUBTYPE_MUL;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }
	| ADD
	{
        $$.node_type = NODE_TYPE_UNARY_OPERATOR;
        $$.node_sub_type = NODE_TYPE_UNARY_OPERATOR_SUBTYPE_ADD;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }
	| SUB
	{
        $$.node_type = NODE_TYPE_UNARY_OPERATOR;
        $$.node_sub_type = NODE_TYPE_UNARY_OPERATOR_SUBTYPE_SUB;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }
	| BITNOT
	{
        $$.node_type = NODE_TYPE_UNARY_OPERATOR;
        $$.node_sub_type = NODE_TYPE_UNARY_OPERATOR_SUBTYPE_BITNOT;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }
	| OPERATOR_NOT
	{
        $$.node_type = NODE_TYPE_UNARY_OPERATOR;
        $$.node_sub_type = NODE_TYPE_UNARY_OPERATOR_SUBTYPE_OPERATOR_NOT;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }
	;

cast_expression
	: unary_expression
	{
        $$.node_type = NODE_TYPE_CAST_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_CAST_EXPRESSION_SUBTYPE_UNARY_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }
	| LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS cast_expression
	{
        $$.node_type = NODE_TYPE_CAST_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_CAST_EXPRESSION_SUBTYPE_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS_CAST_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_PARENTHESIS
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //type_name
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_PARENTHESIS
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //cast_expression
    }
	;

multiplicative_expression
	: cast_expression
	{
        $$.node_type = NODE_TYPE_MULTIPLICATIVE_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_CAST_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
    }
	| multiplicative_expression MUL cast_expression
	{
        $$.node_type = NODE_TYPE_MULTIPLICATIVE_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION_MUL_CAST_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //multiplicative_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //MUL
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //cast_expression
    }
	| multiplicative_expression DIV cast_expression
	{
        $$.node_type = NODE_TYPE_MULTIPLICATIVE_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION_DIV_CAST_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //multiplicative_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //DIV
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //cast_expression
    }
	| multiplicative_expression MOD cast_expression
	{
        $$.node_type = NODE_TYPE_MULTIPLICATIVE_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION_MOD_CAST_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //multiplicative_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //MOD
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //cast_expression
    }
	;

additive_expression
	: multiplicative_expression
	{
        $$.node_type = NODE_TYPE_ADDITIVE_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //multiplicative_expression
    }
	| additive_expression ADD multiplicative_expression
	{
        $$.node_type = NODE_TYPE_ADDITIVE_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION_ADD_MULTIPLICATIVE_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //additive_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //ADD
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //multiplicative_expression
    }
	| additive_expression SUB multiplicative_expression
	{
        $$.node_type = NODE_TYPE_ADDITIVE_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION_SUB_MULTIPLICATIVE_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //additive_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //SUB
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //multiplicative_expression
    }
	;

shift_expression
	: additive_expression
	{
        $$.node_type = NODE_TYPE_SHIFT_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_SHIFT_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //additive_expression
    }
	| shift_expression LEFT_SHIFT additive_expression
	{
        $$.node_type = NODE_TYPE_SHIFT_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_SHIFT_EXPRESSION_SUBTYPE_SHIFT_EXPRESSION_LEFT_SHIFT_ADDITIVE_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //shift_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_SHIFT
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //additive_expression
    }
	| shift_expression RIGHT_SHIFT additive_expression
	{
        $$.node_type = NODE_TYPE_SHIFT_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_SHIFT_EXPRESSION_SUBTYPE_SHIFT_EXPRESSION_RIGHT_SHIFT_ADDITIVE_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //shift_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //RIGHT_SHIFT
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //additive_expression
    }
	;

relational_expression
	: shift_expression
	{
        $$.node_type = NODE_TYPE_RELATIONAL_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_SHIFT_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //shift_expression
    }
	| relational_expression LESS_THAN shift_expression
	{
        $$.node_type = NODE_TYPE_RELATIONAL_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_LESS_THAN_SHIFT_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //relational_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LESS_THAN
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //shift_expression
    }
	| relational_expression GREATER_THAN shift_expression
	{
        $$.node_type = NODE_TYPE_RELATIONAL_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_GREATER_THAN_SHIFT_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //relational_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //GREATER_THAN
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //shift_expression
    }
	| relational_expression LE_OP shift_expression
	{
        $$.node_type = NODE_TYPE_RELATIONAL_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_LE_OP_SHIFT_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //relational_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LE_OP
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //shift_expression
    }
	| relational_expression GE_OP shift_expression
	{
        $$.node_type = NODE_TYPE_RELATIONAL_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_GE_OP_SHIFT_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //relational_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //GE_OP
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //shift_expression
    }
	;

equality_expression
	: relational_expression
	{
        $$.node_type = NODE_TYPE_EQUALITY_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_EQUALITY_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //relational_expression
    }
	| equality_expression EQ_OP relational_expression
	{
        $$.node_type = NODE_TYPE_EQUALITY_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_EQUALITY_EXPRESSION_SUBTYPE_EQUALITY_EXPRESSION_EQ_OP_RELATIONAL_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //equality_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //EQ_OP
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //relational_expression
    }
	| equality_expression NE_OP relational_expression
	{
        $$.node_type = NODE_TYPE_EQUALITY_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_EQUALITY_EXPRESSION_SUBTYPE_EQUALITY_EXPRESSION_NE_OP_RELATIONAL_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //equality_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //NE_OP
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //relational_expression
    }
	;

and_expression
	: equality_expression
	{
        $$.node_type = NODE_TYPE_AND_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_AND_EXPRESSION_SUBTYPE_EQUALITY_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //equality_expression
    }
	| and_expression BITAND equality_expression
	{
        $$.node_type = NODE_TYPE_AND_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_AND_EXPRESSION_SUBTYPE_AND_EXPRESSION_BITAND_RELATIONAL_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //and_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //BITAND
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //equality_expression
    }
	;

exclusive_or_expression
	: and_expression
	{
        $$.node_type = NODE_TYPE_EXCLUSIVE_OR_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_EXCLUSIVE_OR_EXPRESSION_SUBTYPE_AND_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //and_expression
    }
	| exclusive_or_expression BITXOR and_expression
	{
        $$.node_type = NODE_TYPE_EXCLUSIVE_OR_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_EXCLUSIVE_OR_EXPRESSION_SUBTYPE_EXCLUSIVE_OR_EXPRESSION_BITXOR_AND_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //exclusive_or_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //BITXOR
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //and_expression
    }
	;

inclusive_or_expression
	: exclusive_or_expression
	{
        $$.node_type = NODE_TYPE_INCLUSIVE_OR_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_INCLUSIVE_OR_EXPRESSION_SUBTYPE_EXCLUSIVE_OR_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //exclusive_or_expression
    }
	| inclusive_or_expression BITOR exclusive_or_expression
	{
        $$.node_type = NODE_TYPE_INCLUSIVE_OR_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_INCLUSIVE_OR_EXPRESSION_SUBTYPE_INCLUSIVE_OR_EXPRESSION_BITOR_EXCLUSIVE_OR_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //inclusive_or_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //BITOR
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //exclusive_or_expression
    }
	;

logical_and_expression
	: inclusive_or_expression
	{
        $$.node_type = NODE_TYPE_LOGICAL_AND_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_LOGICAL_AND_EXPRESSION_SUBTYPE_INCLUSIVE_OR_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //inclusive_or_expression
    }
	| logical_and_expression AND_OP inclusive_or_expression
	{
        $$.node_type = NODE_TYPE_LOGICAL_AND_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_LOGICAL_AND_EXPRESSION_SUBTYPE_LOGICAL_AND_EXPRESSION_AND_OP_INCLUSIVE_OR_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //logical_and_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //AND_OP
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //inclusive_or_expression
    }
	;

logical_or_expression
	: logical_and_expression
	{
        $$.node_type = NODE_TYPE_LOGICAL_OR_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_LOGICAL_OR_EXPRESSION_SUBTYPE_LOGICAL_AND_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //logical_and_expression
    }
	| logical_or_expression OR_OP logical_and_expression
	{
        $$.node_type = NODE_TYPE_LOGICAL_OR_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_LOGICAL_OR_EXPRESSION_SUBTYPE_LOGICAL_OR_EXPRESSION_OR_OP_LOGICAL_AND_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //logical_or_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //OR_OP
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //NODE_TYPE_LOGICAL_OR_EXPRESSION
    }
	;

conditional_expression
	: logical_or_expression
	{
        $$.node_type = NODE_TYPE_CONDITIONAL_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_CONDITIONAL_EXPRESSION_SUBTYPE_LOGICAL_OR_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //logical_or_expression
    }
	| logical_or_expression QUESTION expression COLON conditional_expression
	{
        $$.node_type = NODE_TYPE_CONDITIONAL_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_CONDITIONAL_EXPRESSION_SUBTYPE_LOGICAL_OR_EXPRESSION_QUESTION_EXPRESSION_COLON_CONDITIONAL_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //logical_or_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //OR_OP
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //NODE_TYPE_LOGICAL_OR_EXPRESSION
    }
	;

assignment_expression
	: conditional_expression
	{
        $$.node_type = NODE_TYPE_ASSIGNMENT_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_ASSIGNMENT_EXPRESSION_SUBTYPE_CONDITIONAL_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //conditional_expression
    }
	| unary_expression assignment_operator assignment_expression
	{
        $$.node_type = NODE_TYPE_ASSIGNMENT_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_ASSIGNMENT_EXPRESSION_SUBTYPE_UNARY_EXPRESSION_ASSIGNMENT_OPERATOR_ASSIGNMENT_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //unary_expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //assignment_operator
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //assignment_expression
    }
	;

assignment_operator
	: ASSIGN
	{
        $$.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        $$.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_ASSIGN;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //ASSIGN
    }
	| MUL_ASSIGN
	{
        $$.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        $$.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_MUL_ASSIGN;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //MUL_ASSIGN
    }
	| DIV_ASSIGN
	{
        $$.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        $$.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_DIV_ASSIGN;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //DIV_ASSIGN
    }
	| MOD_ASSIGN
	{
        $$.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        $$.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_MOD_ASSIGN;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //MOD_ASSIGN
    }
	| ADD_ASSIGN
	{
        $$.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        $$.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_ADD_ASSIGN;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //ADD_ASSIGN
    }
	| SUB_ASSIGN
	{
        $$.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        $$.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_SUB_ASSIGN;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //SUB_ASSIGN
    }
	| LEFT_SHIFT_ASSIGN
	{
        $$.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        $$.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_LEFT_SHIFT_ASSIGN;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_SHIFT_ASSIGN
    }
	| RIGHT_SHIFT_ASSIGN
	{
        $$.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        $$.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_RIGHT_SHIFT_ASSIGN;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //RIGHT_SHIFT_ASSIGN
    }
	| AND_ASSIGN
	{
        $$.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        $$.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_AND_ASSIGN;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //AND_ASSIGN
    }
	| XOR_ASSIGN
	{
        $$.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        $$.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_XOR_ASSIGN;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //XOR_ASSIGN
    }
	| OR_ASSIGN
	{
        $$.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        $$.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_OR_ASSIGN;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //OR_ASSIGN
    }
	;

expression
	: assignment_expression
	{
        $$.node_type = NODE_TYPE_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_EXPRESSION_SUBTYPE_ASSIGNMENT_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //assignment_expression
    }
	| expression COMMA assignment_expression
	{
        $$.node_type = NODE_TYPE_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_EXPRESSION_SUBTYPE_EXPRESSION_COMMA_ASSIGNMENT_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //expression
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COMMA
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //assignment_expression
    }
	;

constant_expression
	: conditional_expression
	{
		/* with constraints */
        $$.node_type = NODE_TYPE_CONSTANT_EXPRESSION;
        $$.node_sub_type = NODE_TYPE_CONSTANT_EXPRESSION_SUBTYPE_CONDITIONAL_EXPRESSION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //conditional_expression
    }	
	;

declaration
	: declaration_specifiers SEMI_COLON
	{
        $$.node_type = NODE_TYPE_DECLARATION;
        $$.node_sub_type = NODE_TYPE_DECLARATION_SUBTYPE_DECLARATION_SPECIFIERS_SEMI_COLON;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declaration_specifiers
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //SEMI_COLON
    }
	| declaration_specifiers init_declarator_list SEMI_COLON
	{
        $$.node_type = NODE_TYPE_DECLARATION;
        $$.node_sub_type = NODE_TYPE_DECLARATION_SUBTYPE_DECLARATION_SPECIFIERS_INIT_DECLARATOR_LIST_SEMI_COLON;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declaration_specifiers
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //init_declarator_list
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //SEMI_COLON
    }
	| static_assert_declaration
	{
        $$.node_type = NODE_TYPE_DECLARATION;
        $$.node_sub_type = NODE_TYPE_DECLARATION_SUBTYPE_STATIC_ASSERT_DECLARATION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //static_assert_declaration
    }
	;

declaration_specifiers
	: storage_class_specifier declaration_specifiers
	{
        $$.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        $$.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_STORAGE_CLASS_SPECIFIER_DECLARATION_SPECIFIERS;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //storage_class_specifier
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //declaration_specifiers
    }
	| storage_class_specifier
	{
        $$.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        $$.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_STORAGE_CLASS_SPECIFIER;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //storage_class_specifier
    }
	| type_specifier declaration_specifiers
	{
        $$.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        $$.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_TYPE_SPECIFIER_DECLARATION_SPECIFIERS;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_specifier
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //declaration_specifiers
    }
	| type_specifier
	{
        $$.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        $$.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_TYPE_SPECIFIER;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_specifier
    }
	| type_qualifier declaration_specifiers
	{
        $$.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        $$.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_TYPE_QUALIFIER_DECLARATION_SPECIFIERS;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_qualifier
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //declaration_specifiers
    }
	| type_qualifier
	{
        $$.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        $$.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_TYPE_QUALIFIER;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_qualifier
    }
	| function_specifier declaration_specifiers
	{
        $$.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        $$.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_FUNCTION_SPECIFIER_DECLARATION_SPECIFIERS;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //function_specifier
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //declaration_specifiers
    }
	| function_specifier
	{
        $$.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        $$.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_FUNCTION_SPECIFIER;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //function_specifier
    }
	| alignment_specifier declaration_specifiers
	{
        $$.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        $$.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_ALIGNMENT_SPECIFIER_DECLARATION_SPECIFIERS;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //alignment_specifier
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //declaration_specifiers
    }
	| alignment_specifier
	{
        $$.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        $$.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_ALIGNMENT_SPECIFIER;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //alignment_specifier
    }
	;

init_declarator_list
	: init_declarator
	{
        $$.node_type = NODE_TYPE_INIT_DECLARATOR_LIST;
        $$.node_sub_type = NODE_TYPE_INIT_DECLARATOR_LIST_SUBTYPE_INIT_DECLARATOR;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //init_declarator
    }
	| init_declarator_list COMMA init_declarator
	{
        $$.node_type = NODE_TYPE_INIT_DECLARATOR_LIST;
        $$.node_sub_type = NODE_TYPE_INIT_DECLARATOR_LIST_SUBTYPE_INIT_DECLARATOR_LIST_INIT_DECLARATOR;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //init_declarator_list
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //COMMA
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //init_declarator
    }
	;

init_declarator
	: declarator ASSIGN initializer
	{
        $$.node_type = NODE_TYPE_INIT_DECLARATOR;
        $$.node_sub_type = NODE_TYPE_INIT_DECLARATOR_SUBTYPE_DECLARATOR_ASSIGN_INITIALIZER;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declarator
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //ASSIGN
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //initializer
    }
	| declarator
	{
        $$.node_type = NODE_TYPE_INIT_DECLARATOR;
        $$.node_sub_type = NODE_TYPE_INIT_DECLARATOR_SUBTYPE_DECLARATOR;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declarator
    }
	;

storage_class_specifier
	: TYPEDEF	
	{
		/* identifiers must be flagged as TYPEDEF_NAME */
        $$.node_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_TYPEDEF;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //TYPEDEF
    }
	| EXTERN
	{
        $$.node_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_EXTERN;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //EXTERN
    }
	| STATIC
	{
        $$.node_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_STATIC;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //STATIC
    }
	| THREAD_LOCAL
	{
        $$.node_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_THREAD_LOCAL;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //THREAD_LOCAL
    }
	| AUTO
	{
        $$.node_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_AUTO;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //AUTO
    }
	| REGISTER
	{
        $$.node_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_REGISTER;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //REGISTER
    }
	;

type_specifier
	: VOID
	{
        $$.node_type = NODE_TYPE_TYPE_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_VOID;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //VOID
    }
	| CHAR
	{
        $$.node_type = NODE_TYPE_TYPE_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_CHAR;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //CHAR
    }
	| SHORT
	{
        $$.node_type = NODE_TYPE_TYPE_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_SHORT;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //SHORT
    }
	| INT
	{
        $$.node_type = NODE_TYPE_TYPE_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_INT;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //INT
    }
	| LONG
	{
        $$.node_type = NODE_TYPE_TYPE_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_LONG;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LONG
    }
	| FLOAT
	{
        $$.node_type = NODE_TYPE_TYPE_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_FLOAT;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //FLOAT
    }
	| DOUBLE
	{
        $$.node_type = NODE_TYPE_TYPE_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_DOUBLE;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //DOUBLE
    }
	| SIGNED
	{
        $$.node_type = NODE_TYPE_TYPE_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_SIGNED;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //SIGNED
    }
	| UNSIGNED
	{
        $$.node_type = NODE_TYPE_TYPE_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_UNSIGNED;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //UNSIGNED
    }
	| BOOL
	{
        $$.node_type = NODE_TYPE_TYPE_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_BOOL;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //BOOL
    }
	| COMPLEX
	{
        $$.node_type = NODE_TYPE_TYPE_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_COMPLEX;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //COMPLEX
    }
	| IMAGINARY	  
	{
		/* non-mandated extension */
        $$.node_type = NODE_TYPE_TYPE_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_IMAGINARY;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //IMAGINARY
    }	
	| atomic_type_specifier
	{
        $$.node_type = NODE_TYPE_TYPE_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_ATOMIC_TYPE_SPECIFIER;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //atomic_type_specifier
    }
	| struct_or_union_specifier
	{
        $$.node_type = NODE_TYPE_TYPE_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_STRUCT_OR_UNION_SPECIFIER;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //struct_or_union_specifier
    }
	| enum_specifier
	{
        $$.node_type = NODE_TYPE_TYPE_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_ENUM_SPECIFIER;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //enum_specifier
    }
	| TYPEDEF_NAME	
	{
		/* after it has been defined as such */
        $$.node_type = NODE_TYPE_TYPE_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_TYPEDEF_NAME;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //TYPEDEF_NAME
    }	
	;

struct_or_union_specifier
	: struct_or_union LEFT_BRACE struct_declaration_list RIGHT_BRACE
	{
        $$.node_type = NODE_TYPE_STRUCT_OR_UNION_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_STRUCT_OR_UNION_SPECIFIER_SUBTYPE_STRUCT_OR_UNION_LEFT_BRACE_STRUCT_DECLARATION_LIST_RIGHT_BRACE;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //struct_or_union
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACE
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //struct_declaration_list
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_BRACE
    }
	| struct_or_union IDENTIFIER LEFT_BRACE struct_declaration_list RIGHT_BRACE
	{
        $$.node_type = NODE_TYPE_STRUCT_OR_UNION_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_STRUCT_OR_UNION_SPECIFIER_SUBTYPE_STRUCT_OR_UNION_IDENTIFIER_LEFT_BRACE_STRUCT_DECLARATION_LIST_RIGHT_BRACE;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //struct_or_union
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //IDENTIFIER
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //LEFT_BRACE
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //struct_declaration_list
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //RIGHT_BRACE
    }
	| struct_or_union IDENTIFIER
	{
        $$.node_type = NODE_TYPE_STRUCT_OR_UNION_SPECIFIER;
        $$.node_sub_type = NODE_TYPE_STRUCT_OR_UNION_SPECIFIER_SUBTYPE_IDENTIFIER;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //struct_or_union
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //IDENTIFIER
    }
	;

struct_or_union
	: STRUCT
	{
        $$.node_type = NODE_TYPE_STRUCT_OR_UNION;
        $$.node_sub_type = NODE_TYPE_STRUCT_OR_UNION_SUBTYPE_STRUCT;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //STRUCT
    }
	| UNION
	{
        $$.node_type = NODE_TYPE_STRUCT_OR_UNION;
        $$.node_sub_type = NODE_TYPE_STRUCT_OR_UNION_SUBTYPE_UNION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //UNION
    }
	;

struct_declaration_list
	: struct_declaration
	{
        $$.node_type = NODE_TYPE_STRUCT_DECLARATION_LIST;
        $$.node_sub_type = NODE_TYPE_STRUCT_DECLARATION_LIST_SUBTYPE_STRUCT_DECLARATION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //struct_declaration
    }
	| struct_declaration_list struct_declaration
	{
        $$.node_type = NODE_TYPE_STRUCT_DECLARATION_LIST;
        $$.node_sub_type = NODE_TYPE_STRUCT_DECLARATION_LIST_SUBTYPE_STRUCT_DECLARATION_LIST_STRUCT_DECLARATION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //struct_declaration_list
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //struct_declaration
    }
	;

struct_declaration
	: specifier_qualifier_list SEMI_COLON
	{
		/* for anonymous struct/union */
        $$.node_type = NODE_TYPE_STRUCT_DECLARATION;
        $$.node_sub_type = NODE_TYPE_STRUCT_DECLARATION_SUBTYPE_SPECIFIER_QUALIFIER_LIST_SEMI_COLON;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //specifier_qualifier_list
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //SEMI_COLON
    }	
	| specifier_qualifier_list struct_declarator_list SEMI_COLON
	{
        $$.node_type = NODE_TYPE_STRUCT_DECLARATION;
        $$.node_sub_type = NODE_TYPE_STRUCT_DECLARATION_SUBTYPE_SPECIFIER_QUALIFIER_LIST_STRUT_DECLARATOR_LIST_SEMI_COLON;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //specifier_qualifier_list
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //struct_declarator_list
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //SEMI_COLON
    }
	| static_assert_declaration
	{
        $$.node_type = NODE_TYPE_STRUCT_DECLARATION;
        $$.node_sub_type = NODE_TYPE_STRUCT_DECLARATION_SUBTYPE_STATIC_ASSERT_DECLARATION;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //static_assert_declaration
    }
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	{
        $$.node_type = NODE_TYPE_SPECIFIER_QUALIFIER_LIST;
        $$.node_sub_type = NODE_TYPE_SPECIFIER_QUALIFIER_LIST_SUBTYPE_TYPE_SPECIFIER_SPECIFIER_QUALIFIER_LIST;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_specifier
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //specifier_qualifier_list
    }
	| type_specifier
	{
        $$.node_type = NODE_TYPE_SPECIFIER_QUALIFIER_LIST;
        $$.node_sub_type = NODE_TYPE_SPECIFIER_QUALIFIER_LIST_SUBTYPE_TYPE_SPECIFIER;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_specifier
    }
	| type_qualifier specifier_qualifier_list
	{
        $$.node_type = NODE_TYPE_SPECIFIER_QUALIFIER_LIST;
        $$.node_sub_type = NODE_TYPE_SPECIFIER_QUALIFIER_LIST_SUBTYPE_TYPE_QUALIFIER_TYPE_SPECIFIER_QUALIFIER_LIST;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_qualifier
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //specifier_qualifier_list
    }
	| type_qualifier
	{
        $$.node_type = NODE_TYPE_SPECIFIER_QUALIFIER_LIST;
        $$.node_sub_type = NODE_TYPE_SPECIFIER_QUALIFIER_LIST_SUBTYPE_TYPE_QUALIFIER;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_qualifier
    }
	;

struct_declarator_list
	: struct_declarator
	{
        $$.node_type = NODE_TYPE_STRUCT_DECLARATOR_LIST;
        $$.node_sub_type = NODE_TYPE_STRUCT_DECLARATOR_LIST_SUBTYPE_STRUCT_DECLARATOR;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //struct_declarator
    }
	| struct_declarator_list COMMA struct_declarator
	{
        $$.node_type = NODE_TYPE_STRUCT_DECLARATOR_LIST;
        $$.node_sub_type = NODE_TYPE_STRUCT_DECLARATOR_LIST_SUBTYPE_STRUCT_DECLARATOR_LIST_COMMA_STRUCT_DECLARATOR;
        $$.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //struct_declarator_list
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COMMA
		$$.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //struct_declarator
    }
	;

struct_declarator
	: COLON constant_expression
	| declarator COLON constant_expression
	| declarator
	;

enum_specifier
	: ENUM LEFT_BRACE enumerator_list RIGHT_BRACE
	| ENUM LEFT_BRACE enumerator_list COMMA RIGHT_BRACE
	| ENUM IDENTIFIER LEFT_BRACE enumerator_list RIGHT_BRACE
	| ENUM IDENTIFIER LEFT_BRACE enumerator_list COMMA RIGHT_BRACE
	| ENUM IDENTIFIER
	;

enumerator_list
	: enumerator
	| enumerator_list COMMA enumerator
	;

enumerator	/* identifiers must be flagged as ENUMERATION_CONSTANT */
	: enumeration_constant ASSIGN constant_expression
	| enumeration_constant
	;

atomic_type_specifier
	: ATOMIC LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS
	;

type_qualifier
	: CONST
	| RESTRICT
	| VOLATILE
	| ATOMIC
	;

function_specifier
	: INLINE
	| NORETURN
	;

alignment_specifier
	: ALIGNAS LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS
	| ALIGNAS LEFT_PARENTHESIS constant_expression RIGHT_PARENTHESIS
	;

declarator
	: pointer direct_declarator
	| direct_declarator
	;

direct_declarator
	: IDENTIFIER
	| LEFT_PARENTHESIS declarator RIGHT_PARENTHESIS
	| direct_declarator LEFT_BRACKET RIGHT_BRACKET
	| direct_declarator LEFT_BRACKET MUL RIGHT_BRACKET
	| direct_declarator LEFT_BRACKET STATIC type_qualifier_list assignment_expression RIGHT_BRACKET
	| direct_declarator LEFT_BRACKET STATIC assignment_expression RIGHT_BRACKET
	| direct_declarator LEFT_BRACKET type_qualifier_list MUL RIGHT_BRACKET
	| direct_declarator LEFT_BRACKET type_qualifier_list STATIC assignment_expression RIGHT_BRACKET
	| direct_declarator LEFT_BRACKET type_qualifier_list assignment_expression RIGHT_BRACKET
	| direct_declarator LEFT_BRACKET type_qualifier_list RIGHT_BRACKET
	| direct_declarator LEFT_BRACKET assignment_expression RIGHT_BRACKET
	| direct_declarator LEFT_PARENTHESIS parameter_type_list RIGHT_PARENTHESIS
	| direct_declarator LEFT_PARENTHESIS RIGHT_PARENTHESIS
	| direct_declarator LEFT_PARENTHESIS identifier_list RIGHT_PARENTHESIS
	;

pointer
	: MUL type_qualifier_list pointer
	| MUL type_qualifier_list
	| MUL pointer
	| MUL
	;

type_qualifier_list
	: type_qualifier
	| type_qualifier_list type_qualifier
	;


parameter_type_list
	: parameter_list COMMA ELLIPSIS
	| parameter_list
	;

parameter_list
	: parameter_declaration
	| parameter_list COMMA parameter_declaration
	;

parameter_declaration
	: declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

identifier_list
	: IDENTIFIER
	| identifier_list COMMA IDENTIFIER
	;

type_name
	: specifier_qualifier_list abstract_declarator
	| specifier_qualifier_list
	;

abstract_declarator
	: pointer direct_abstract_declarator
	| pointer
	| direct_abstract_declarator
	;

direct_abstract_declarator
	: LEFT_PARENTHESIS abstract_declarator RIGHT_PARENTHESIS
	| LEFT_BRACKET RIGHT_BRACKET
	| LEFT_BRACKET MUL RIGHT_BRACKET
	| LEFT_BRACKET STATIC type_qualifier_list assignment_expression RIGHT_BRACKET
	| LEFT_BRACKET STATIC assignment_expression RIGHT_BRACKET
	| LEFT_BRACKET type_qualifier_list STATIC assignment_expression RIGHT_BRACKET
	| LEFT_BRACKET type_qualifier_list assignment_expression RIGHT_BRACKET
	| LEFT_BRACKET type_qualifier_list RIGHT_BRACKET
	| LEFT_BRACKET assignment_expression RIGHT_BRACKET
	| direct_abstract_declarator LEFT_BRACKET RIGHT_BRACKET
	| direct_abstract_declarator LEFT_BRACKET MUL RIGHT_BRACKET
	| direct_abstract_declarator LEFT_BRACKET STATIC type_qualifier_list assignment_expression RIGHT_BRACKET
	| direct_abstract_declarator LEFT_BRACKET STATIC assignment_expression RIGHT_BRACKET
	| direct_abstract_declarator LEFT_BRACKET type_qualifier_list assignment_expression RIGHT_BRACKET
	| direct_abstract_declarator LEFT_BRACKET type_qualifier_list STATIC assignment_expression RIGHT_BRACKET
	| direct_abstract_declarator LEFT_BRACKET type_qualifier_list RIGHT_BRACKET
	| direct_abstract_declarator LEFT_BRACKET assignment_expression RIGHT_BRACKET
	| LEFT_PARENTHESIS RIGHT_PARENTHESIS
	| LEFT_PARENTHESIS parameter_type_list RIGHT_PARENTHESIS
	| direct_abstract_declarator LEFT_PARENTHESIS RIGHT_PARENTHESIS
	| direct_abstract_declarator LEFT_PARENTHESIS parameter_type_list RIGHT_PARENTHESIS
	;

initializer
	: LEFT_BRACE initializer_list RIGHT_BRACE
	| LEFT_BRACE initializer_list COMMA RIGHT_BRACE
	| assignment_expression
	;

initializer_list
	: designation initializer
	| initializer
	| initializer_list COMMA designation initializer
	| initializer_list COMMA initializer
	;

designation
	: designator_list ASSIGN
	;

designator_list
	: designator
	| designator_list designator
	;

designator
	: LEFT_BRACKET constant_expression RIGHT_BRACKET
	| DOT IDENTIFIER
	;

static_assert_declaration
	: STATIC_ASSERT LEFT_PARENTHESIS constant_expression COMMA STRING_LITERAL RIGHT_PARENTHESIS SEMI_COLON
	;

statement
	: labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;

labeled_statement
	: IDENTIFIER COLON statement
	| CASE constant_expression COLON statement
	| DEFAULT COLON statement
	;

compound_statement
	: LEFT_BRACE RIGHT_BRACE
	| LEFT_BRACE  block_item_list RIGHT_BRACE
	;

block_item_list
	: block_item
	| block_item_list block_item
	;

block_item
	: declaration
	| statement
	;

expression_statement
	: SEMI_COLON
	| expression SEMI_COLON
	;

selection_statement
	: IF LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement ELSE statement
	| IF LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement
	| SWITCH LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement
	;

iteration_statement
	: WHILE LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement
	| DO statement WHILE LEFT_PARENTHESIS expression RIGHT_PARENTHESIS SEMI_COLON
	| FOR LEFT_PARENTHESIS expression_statement expression_statement RIGHT_PARENTHESIS statement
	| FOR LEFT_PARENTHESIS expression_statement expression_statement expression RIGHT_PARENTHESIS statement
	| FOR LEFT_PARENTHESIS declaration expression_statement RIGHT_PARENTHESIS statement
	| FOR LEFT_PARENTHESIS declaration expression_statement expression RIGHT_PARENTHESIS statement
	;

jump_statement
	: GOTO IDENTIFIER SEMI_COLON
	| CONTINUE SEMI_COLON
	| BREAK SEMI_COLON
	| RETURN SEMI_COLON
	| RETURN expression SEMI_COLON
	;

translation_unit
	: external_declaration
	| translation_unit external_declaration
	;

external_declaration
	: function_definition
	| declaration
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement
	;

declaration_list
	: declaration
	| declaration_list declaration
	;

%%
