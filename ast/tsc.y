
%{
#include <stdio.h>
#include "parser.h"
#include "lex.yy.h"
#include "tsc.tab.hh"

#define YYERROR_VERBOSE 1  
int yylex ();
void yyerror(const char *s)
{
	printf("%s:%d\n\t%s\n",input_file_name.c_str(), yyget_lineno(), s);
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
        ast_node node;
        node.node_type = NODE_TYPE_PRIMARY_EXPRESSION;
        node.node_sub_type = NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_IDENTIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }
	| constant 
	{
		/* 这里有二义性.constant可以推出 IDENTIFIER constant实际上要在语义分析才能确定 */
        ast_node node;
        node.node_type = NODE_TYPE_PRIMARY_EXPRESSION;
        node.node_sub_type = NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_CONSTANT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }
	| string
	{
        ast_node node;
        node.node_type = NODE_TYPE_PRIMARY_EXPRESSION;
        node.node_sub_type = NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_STRING;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }
	| LEFT_PARENTHESIS expression RIGHT_PARENTHESIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_PRIMARY_EXPRESSION;
        node.node_sub_type = NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_LEFT_PARENTHESIS_EXPRESSION_RIGHT_PARENTHESIS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); // LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); // expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); // RIGHT_PARENTHESIS
        $$=node;
    }
	| generic_selection /* C11 generic_selection */
	{
        ast_node node;
        node.node_type = NODE_TYPE_PRIMARY_EXPRESSION;
        node.node_sub_type = NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_GENERIC_SELECTION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }
	;

constant
	: I_CONSTANT
	{
		/* includes character_constant */
        ast_node node;
        node.node_type = NODE_TYPE_CONSTANT;
        node.node_sub_type = NODE_TYPE_CONSTANT_SUBTYPE_ICONSTANT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }		
	| F_CONSTANT
	{
        ast_node node;
        node.node_type = NODE_TYPE_CONSTANT;
        node.node_sub_type = NODE_TYPE_CONSTANT_SUBTYPE_FCONSTANT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }	
	| ENUMERATION_CONSTANT	
	{
		/* after it has been defined as such */
        ast_node node;
        node.node_type = NODE_TYPE_CONSTANT;
        node.node_sub_type = NODE_TYPE_CONSTANT_SUBTYPE_ENUMERATION_CONSTANT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }	
	;

enumeration_constant
	: IDENTIFIER
	{
	/* before it has been defined as such */
        ast_node node;
        node.node_type = NODE_TYPE_ENUMERATION_CONSTANT;
        node.node_sub_type = NODE_TYPE_ENUMERATION_CONSTANT_SUBTYPE_IDENTIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }	
	;

string
	: STRING_LITERAL 
	{
		/* 支持多个字符串连起来如 "a" "b"等价于 "ab" */
        ast_node node;
        node.node_type = NODE_TYPE_STRING;
        node.node_sub_type = NODE_TYPE_STRING_SUBTYPE_STRING_LITERAL;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }
	| FUNC_NAME 
	{
		/* 在任何函数内部 "__func__" 都隐式表示当前函数名字 */
        ast_node node;
        node.node_type = NODE_TYPE_STRING;
        node.node_sub_type = NODE_TYPE_STRING_SUBTYPE_FUNC_NAME;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }	
	;

generic_selection
	: GENERIC LEFT_PARENTHESIS assignment_expression COMMA generic_assoc_list RIGHT_PARENTHESIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_GENERIC_SELECTION;
        node.node_sub_type = NODE_TYPE_GENERIC_SELECTION_SUBTYPE_GENERIC_LEFT_PARENTHESIS_ASSIGNMENT_EXPRESSION_COMMA_GENERIC_ASSOC_LIST_RIGHT_PARENTHESIS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //GENERIC
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //assignment_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //COMMA
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //generic_assoc_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($6))); //RIGHT_PARENTHESIS		
        $$=node;
    }	
	;

generic_assoc_list
	: generic_association
	{
        ast_node node;
        node.node_type = NODE_TYPE_GENERIC_ASSOC_LIST;
        node.node_sub_type = NODE_TYPE_GENERIC_ASSOC_LIST_SUBTYPE_GENERIC_ASSOCIATION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }
	| generic_assoc_list COMMA generic_association
	{
        ast_node node;
        node.node_type = NODE_TYPE_GENERIC_ASSOC_LIST;
        node.node_sub_type = NODE_TYPE_GENERIC_ASSOC_LIST_SUBTYPE_GENERIC_ASSOC_LIST_COMMA_ASSOCIATION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //generic_assoc_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COMMA
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //generic_association
        $$=node;
    }
	;

generic_association
	: type_name COLON assignment_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_GENERIC_ASSOCIATION;
        node.node_sub_type = NODE_TYPE_GENERIC_ASSOCIATION_SUBTYPE_TYPE_NAME_COLON_ASSIGNMENT_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_name
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COLON
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //assignment_expression
        $$=node;
    }
	| DEFAULT COLON assignment_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_GENERIC_ASSOCIATION;
        node.node_sub_type = NODE_TYPE_GENERIC_ASSOCIATION_SUBTYPE_DEFAULT_COLON_ASSIGNMENT_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //DEFAULT
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COLON
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //assignment_expression
        $$=node;
    }
	;

postfix_expression
	: primary_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        node.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_PRIMARY_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //primary_expression	
        $$=node;
    }
	| postfix_expression LEFT_BRACKET expression RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        node.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_LEFT_BRACKET_EXPRESSION_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //postfix_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_BRACKET
        $$=node;
    }
	| postfix_expression LEFT_PARENTHESIS RIGHT_PARENTHESIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        node.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_LEFT_PARENTHESIS_RIGHT_PARENTHESIS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //postfix_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_PARENTHESIS
        $$=node;
    }
	| postfix_expression LEFT_PARENTHESIS argument_expression_list RIGHT_PARENTHESIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        node.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_LEFT_PARENTHESIS_ARGUMENT_EXPRESSION_LIST_RIGHT_PARENTHESIS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //postfix_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //argument_expression_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_PARENTHESIS
        $$=node;
    }
	| postfix_expression DOT IDENTIFIER
	{
        ast_node node;
        node.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        node.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_DOT_IDENTIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //postfix_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //DOT
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //IDENTIFIER
        $$=node;
    }
	| postfix_expression PTR_OP IDENTIFIER
	{
        ast_node node;
        node.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        node.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_PTR_OP_IDENTIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //DEFAULT
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //PTR_OP
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //IDENTIFIER
        $$=node;
    }
	| postfix_expression INC_OP
	{
        ast_node node;
        node.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        node.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_INC_OP;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //postfix_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //INC_OP
        $$=node;
    }
	| postfix_expression DEC_OP
	{
        ast_node node;
        node.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        node.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_DEC_OP;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //postfix_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //DEC_OP
        $$=node;
    }
	| LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS LEFT_BRACE initializer_list RIGHT_BRACE
	{
        ast_node node;
        node.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        node.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS_LEFT_BRACE_INITIALIZER_LIST_RIGHT_BRACE;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //type_name
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //LEFT_BRACE
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //initializer_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($6))); //RIGHT_BRACE
        $$=node;
    }
	| LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS LEFT_BRACE initializer_list COMMA RIGHT_BRACE
	{
        ast_node node;
        node.node_type = NODE_TYPE_POSTFIX_EXPRESSION;
        node.node_sub_type = NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS_LEFT_BRACE_INITIALIZER_LIST_COMMA_RIGHT_BRACE;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //type_name
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //LEFT_BRACE
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //initializer_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($6))); //COMMA
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($7))); //RIGHT_BRACE
        $$=node;
    }
	;

argument_expression_list
	: assignment_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_ARGUMENT_EXPRESSION_LIST;
        node.node_sub_type = NODE_TYPE_ARGUMENT_EXPRESSION_LIST_SUBTYPE_ASSIGNMENT_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //assignment_expression
        $$=node;
    }
	| argument_expression_list COMMA assignment_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_ARGUMENT_EXPRESSION_LIST;
        node.node_sub_type = NODE_TYPE_ARGUMENT_EXPRESSION_LIST_SUBTYPE_ARGUMENT_EXPRESSION_LIST_COMMA_ASSIGNMENT_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //argument_expression_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COMMA
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //assignment_expression
        $$=node;
    }
	;

unary_expression
	: postfix_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_UNARY_EXPRESSION;
        node.node_sub_type = NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //postfix_expression
        $$=node;
    }
	| INC_OP unary_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_UNARY_EXPRESSION;
        node.node_sub_type = NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_INC_OP_UNARY_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //INC_OP
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //unary_expression
        $$=node;
    }
	| DEC_OP unary_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_UNARY_EXPRESSION;
        node.node_sub_type = NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_DEC_OP_UNARY_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //DEC_OP
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //unary_expression
        $$=node;
    }
	| unary_operator cast_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_UNARY_EXPRESSION;
        node.node_sub_type = NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_UNARY_OPERATOR_CAST_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //unary_operator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //cast_expression
        $$=node;
    }
	| SIZEOF unary_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_UNARY_EXPRESSION;
        node.node_sub_type = NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_SIZEOF_UNARY_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //SIZEOF
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //unary_expression
        $$=node;
    }
	| SIZEOF LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_UNARY_EXPRESSION;
        node.node_sub_type = NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_SIZEOF_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //SIZEOF
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //type_name
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_PARENTHESIS
        $$=node;
    }
	| ALIGNOF LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_UNARY_EXPRESSION;
        node.node_sub_type = NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_ALIGNOF_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //ALIGNOF
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //type_name
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_PARENTHESIS
        $$=node;
    }
	;

unary_operator
	: BITAND
	{
        ast_node node;
        node.node_type = NODE_TYPE_UNARY_OPERATOR;
        node.node_sub_type = NODE_TYPE_UNARY_OPERATOR_SUBTYPE_BITAND;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }
	| MUL
	{
        ast_node node;
        node.node_type = NODE_TYPE_UNARY_OPERATOR;
        node.node_sub_type = NODE_TYPE_UNARY_OPERATOR_SUBTYPE_MUL;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }
	| ADD
	{
        ast_node node;
        node.node_type = NODE_TYPE_UNARY_OPERATOR;
        node.node_sub_type = NODE_TYPE_UNARY_OPERATOR_SUBTYPE_ADD;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }
	| SUB
	{
        ast_node node;
        node.node_type = NODE_TYPE_UNARY_OPERATOR;
        node.node_sub_type = NODE_TYPE_UNARY_OPERATOR_SUBTYPE_SUB;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }
	| BITNOT
	{
        ast_node node;
        node.node_type = NODE_TYPE_UNARY_OPERATOR;
        node.node_sub_type = NODE_TYPE_UNARY_OPERATOR_SUBTYPE_BITNOT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }
	| OPERATOR_NOT
	{
        ast_node node;
        node.node_type = NODE_TYPE_UNARY_OPERATOR;
        node.node_sub_type = NODE_TYPE_UNARY_OPERATOR_SUBTYPE_OPERATOR_NOT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }
	;

cast_expression
	: unary_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_CAST_EXPRESSION;
        node.node_sub_type = NODE_TYPE_CAST_EXPRESSION_SUBTYPE_UNARY_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }
	| LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS cast_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_CAST_EXPRESSION;
        node.node_sub_type = NODE_TYPE_CAST_EXPRESSION_SUBTYPE_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS_CAST_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //type_name
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //cast_expression
        $$=node;
    }
	;

multiplicative_expression
	: cast_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_MULTIPLICATIVE_EXPRESSION;
        node.node_sub_type = NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_CAST_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1)));
        $$=node;
    }
	| multiplicative_expression MUL cast_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_MULTIPLICATIVE_EXPRESSION;
        node.node_sub_type = NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION_MUL_CAST_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //multiplicative_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //MUL
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //cast_expression
        $$=node;
    }
	| multiplicative_expression DIV cast_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_MULTIPLICATIVE_EXPRESSION;
        node.node_sub_type = NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION_DIV_CAST_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //multiplicative_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //DIV
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //cast_expression
        $$=node;
    }
	| multiplicative_expression MOD cast_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_MULTIPLICATIVE_EXPRESSION;
        node.node_sub_type = NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION_MOD_CAST_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //multiplicative_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //MOD
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //cast_expression
        $$=node;
    }
	;

additive_expression
	: multiplicative_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_ADDITIVE_EXPRESSION;
        node.node_sub_type = NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //multiplicative_expression
        $$=node;
    }
	| additive_expression ADD multiplicative_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_ADDITIVE_EXPRESSION;
        node.node_sub_type = NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION_ADD_MULTIPLICATIVE_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //additive_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //ADD
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //multiplicative_expression
        $$=node;
    }
	| additive_expression SUB multiplicative_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_ADDITIVE_EXPRESSION;
        node.node_sub_type = NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION_SUB_MULTIPLICATIVE_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //additive_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //SUB
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //multiplicative_expression
        $$=node;
    }
	;

shift_expression
	: additive_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_SHIFT_EXPRESSION;
        node.node_sub_type = NODE_TYPE_SHIFT_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //additive_expression
        $$=node;
    }
	| shift_expression LEFT_SHIFT additive_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_SHIFT_EXPRESSION;
        node.node_sub_type = NODE_TYPE_SHIFT_EXPRESSION_SUBTYPE_SHIFT_EXPRESSION_LEFT_SHIFT_ADDITIVE_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //shift_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_SHIFT
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //additive_expression
        $$=node;
    }
	| shift_expression RIGHT_SHIFT additive_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_SHIFT_EXPRESSION;
        node.node_sub_type = NODE_TYPE_SHIFT_EXPRESSION_SUBTYPE_SHIFT_EXPRESSION_RIGHT_SHIFT_ADDITIVE_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //shift_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //RIGHT_SHIFT
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //additive_expression
        $$=node;
    }
	;

relational_expression
	: shift_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_RELATIONAL_EXPRESSION;
        node.node_sub_type = NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_SHIFT_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //shift_expression
        $$=node;
    }
	| relational_expression LESS_THAN shift_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_RELATIONAL_EXPRESSION;
        node.node_sub_type = NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_LESS_THAN_SHIFT_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //relational_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LESS_THAN
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //shift_expression
        $$=node;
    }
	| relational_expression GREATER_THAN shift_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_RELATIONAL_EXPRESSION;
        node.node_sub_type = NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_GREATER_THAN_SHIFT_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //relational_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //GREATER_THAN
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //shift_expression
        $$=node;
    }
	| relational_expression LE_OP shift_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_RELATIONAL_EXPRESSION;
        node.node_sub_type = NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_LE_OP_SHIFT_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //relational_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LE_OP
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //shift_expression
        $$=node;
    }
	| relational_expression GE_OP shift_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_RELATIONAL_EXPRESSION;
        node.node_sub_type = NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_GE_OP_SHIFT_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //relational_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //GE_OP
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //shift_expression
        $$=node;
    }
	;

equality_expression
	: relational_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_EQUALITY_EXPRESSION;
        node.node_sub_type = NODE_TYPE_EQUALITY_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //relational_expression
        $$=node;
    }
	| equality_expression EQ_OP relational_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_EQUALITY_EXPRESSION;
        node.node_sub_type = NODE_TYPE_EQUALITY_EXPRESSION_SUBTYPE_EQUALITY_EXPRESSION_EQ_OP_RELATIONAL_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //equality_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //EQ_OP
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //relational_expression
        $$=node;
    }
	| equality_expression NE_OP relational_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_EQUALITY_EXPRESSION;
        node.node_sub_type = NODE_TYPE_EQUALITY_EXPRESSION_SUBTYPE_EQUALITY_EXPRESSION_NE_OP_RELATIONAL_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //equality_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //NE_OP
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //relational_expression
        $$=node;
    }
	;

and_expression
	: equality_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_AND_EXPRESSION;
        node.node_sub_type = NODE_TYPE_AND_EXPRESSION_SUBTYPE_EQUALITY_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //equality_expression
        $$=node;
    }
	| and_expression BITAND equality_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_AND_EXPRESSION;
        node.node_sub_type = NODE_TYPE_AND_EXPRESSION_SUBTYPE_AND_EXPRESSION_BITAND_EQUALITY_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //and_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //BITAND
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //equality_expression
        $$=node;
    }
	;

exclusive_or_expression
	: and_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_EXCLUSIVE_OR_EXPRESSION;
        node.node_sub_type = NODE_TYPE_EXCLUSIVE_OR_EXPRESSION_SUBTYPE_AND_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //and_expression
        $$=node;
    }
	| exclusive_or_expression BITXOR and_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_EXCLUSIVE_OR_EXPRESSION;
        node.node_sub_type = NODE_TYPE_EXCLUSIVE_OR_EXPRESSION_SUBTYPE_EXCLUSIVE_OR_EXPRESSION_BITXOR_AND_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //exclusive_or_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //BITXOR
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //and_expression
        $$=node;
    }
	;

inclusive_or_expression
	: exclusive_or_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_INCLUSIVE_OR_EXPRESSION;
        node.node_sub_type = NODE_TYPE_INCLUSIVE_OR_EXPRESSION_SUBTYPE_EXCLUSIVE_OR_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //exclusive_or_expression
        $$=node;
    }
	| inclusive_or_expression BITOR exclusive_or_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_INCLUSIVE_OR_EXPRESSION;
        node.node_sub_type = NODE_TYPE_INCLUSIVE_OR_EXPRESSION_SUBTYPE_INCLUSIVE_OR_EXPRESSION_BITOR_EXCLUSIVE_OR_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //inclusive_or_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //BITOR
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //exclusive_or_expression
        $$=node;
    }
	;

logical_and_expression
	: inclusive_or_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_LOGICAL_AND_EXPRESSION;
        node.node_sub_type = NODE_TYPE_LOGICAL_AND_EXPRESSION_SUBTYPE_INCLUSIVE_OR_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //inclusive_or_expression
        $$=node;
    }
	| logical_and_expression AND_OP inclusive_or_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_LOGICAL_AND_EXPRESSION;
        node.node_sub_type = NODE_TYPE_LOGICAL_AND_EXPRESSION_SUBTYPE_LOGICAL_AND_EXPRESSION_AND_OP_INCLUSIVE_OR_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //logical_and_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //AND_OP
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //inclusive_or_expression
        $$=node;
    }
	;

logical_or_expression
	: logical_and_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_LOGICAL_OR_EXPRESSION;
        node.node_sub_type = NODE_TYPE_LOGICAL_OR_EXPRESSION_SUBTYPE_LOGICAL_AND_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //logical_and_expression
        $$=node;
    }
	| logical_or_expression OR_OP logical_and_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_LOGICAL_OR_EXPRESSION;
        node.node_sub_type = NODE_TYPE_LOGICAL_OR_EXPRESSION_SUBTYPE_LOGICAL_OR_EXPRESSION_OR_OP_LOGICAL_AND_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //logical_or_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //OR_OP
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //NODE_TYPE_LOGICAL_OR_EXPRESSION
        $$=node;
    }
	;

conditional_expression
	: logical_or_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_CONDITIONAL_EXPRESSION;
        node.node_sub_type = NODE_TYPE_CONDITIONAL_EXPRESSION_SUBTYPE_LOGICAL_OR_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //logical_or_expression
        $$=node;
    }
	| logical_or_expression QUESTION expression COLON conditional_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_CONDITIONAL_EXPRESSION;
        node.node_sub_type = NODE_TYPE_CONDITIONAL_EXPRESSION_SUBTYPE_LOGICAL_OR_EXPRESSION_QUESTION_EXPRESSION_COLON_CONDITIONAL_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //logical_or_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //OR_OP
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //NODE_TYPE_LOGICAL_OR_EXPRESSION
        $$=node;
    }
	;

assignment_expression
	: conditional_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_ASSIGNMENT_EXPRESSION;
        node.node_sub_type = NODE_TYPE_ASSIGNMENT_EXPRESSION_SUBTYPE_CONDITIONAL_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //conditional_expression
        $$=node;
    }
	| unary_expression assignment_operator assignment_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_ASSIGNMENT_EXPRESSION;
        node.node_sub_type = NODE_TYPE_ASSIGNMENT_EXPRESSION_SUBTYPE_UNARY_EXPRESSION_ASSIGNMENT_OPERATOR_ASSIGNMENT_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //unary_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //assignment_operator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //assignment_expression
        $$=node;
    }
	;

assignment_operator
	: ASSIGN
	{
        ast_node node;
        node.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        node.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_ASSIGN;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //ASSIGN
        $$=node;
    }
	| MUL_ASSIGN
	{
        ast_node node;
        node.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        node.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_MUL_ASSIGN;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //MUL_ASSIGN
        $$=node;
    }
	| DIV_ASSIGN
	{
        ast_node node;
        node.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        node.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_DIV_ASSIGN;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //DIV_ASSIGN
        $$=node;
    }
	| MOD_ASSIGN
	{
        ast_node node;
        node.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        node.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_MOD_ASSIGN;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //MOD_ASSIGN
        $$=node;
    }
	| ADD_ASSIGN
	{
        ast_node node;
        node.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        node.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_ADD_ASSIGN;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //ADD_ASSIGN
        $$=node;
    }
	| SUB_ASSIGN
	{
        ast_node node;
        node.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        node.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_SUB_ASSIGN;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //SUB_ASSIGN
        $$=node;
    }
	| LEFT_SHIFT_ASSIGN
	{
        ast_node node;
        node.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        node.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_LEFT_SHIFT_ASSIGN;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_SHIFT_ASSIGN
        $$=node;
    }
	| RIGHT_SHIFT_ASSIGN
	{
        ast_node node;
        node.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        node.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_RIGHT_SHIFT_ASSIGN;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //RIGHT_SHIFT_ASSIGN
        $$=node;
    }
	| AND_ASSIGN
	{
        ast_node node;
        node.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        node.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_BITAND_ASSIGN;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //AND_ASSIGN
        $$=node;
    }
	| XOR_ASSIGN
	{
        ast_node node;
        node.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        node.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_BITXOR_ASSIGN;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //XOR_ASSIGN
        $$=node;
    }
	| OR_ASSIGN
	{
        ast_node node;
        node.node_type = NODE_TYPE_ASSIGNMENT_OPERATOR;
        node.node_sub_type = NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_BITOR_ASSIGN;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //OR_ASSIGN
        $$=node;
    }
	;

expression
	: assignment_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_EXPRESSION;
        node.node_sub_type = NODE_TYPE_EXPRESSION_SUBTYPE_ASSIGNMENT_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //assignment_expression
        $$=node;
    }
	| expression COMMA assignment_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_EXPRESSION;
        node.node_sub_type = NODE_TYPE_EXPRESSION_SUBTYPE_EXPRESSION_COMMA_ASSIGNMENT_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COMMA
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //assignment_expression
        $$=node;
    }
	;

constant_expression
	: conditional_expression
	{
		/* with constraints */
        ast_node node;
        node.node_type = NODE_TYPE_CONSTANT_EXPRESSION;
        node.node_sub_type = NODE_TYPE_CONSTANT_EXPRESSION_SUBTYPE_CONDITIONAL_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //conditional_expression
        $$=node;
    }	
	;

declaration
	: declaration_specifiers SEMI_COLON
	{
        ast_node node;
        node.node_type = NODE_TYPE_DECLARATION;
        node.node_sub_type = NODE_TYPE_DECLARATION_SUBTYPE_DECLARATION_SPECIFIERS_SEMI_COLON;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declaration_specifiers
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //SEMI_COLON
        $$=node;
    }
	| declaration_specifiers init_declarator_list SEMI_COLON
	{
        ast_node node;
        node.node_type = NODE_TYPE_DECLARATION;
        node.node_sub_type = NODE_TYPE_DECLARATION_SUBTYPE_DECLARATION_SPECIFIERS_INIT_DECLARATOR_LIST_SEMI_COLON;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declaration_specifiers
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //init_declarator_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //SEMI_COLON
        $$=node;
    }
	| static_assert_declaration
	{
        ast_node node;
        node.node_type = NODE_TYPE_DECLARATION;
        node.node_sub_type = NODE_TYPE_DECLARATION_SUBTYPE_STATIC_ASSERT_DECLARATION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //static_assert_declaration
        $$=node;
    }
	;

declaration_specifiers
	: storage_class_specifier declaration_specifiers
	{
        ast_node node;
        node.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        node.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_STORAGE_CLASS_SPECIFIER_DECLARATION_SPECIFIERS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //storage_class_specifier
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //declaration_specifiers
        $$=node;
    }
	| storage_class_specifier
	{
        ast_node node;
        node.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        node.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_STORAGE_CLASS_SPECIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //storage_class_specifier
        $$=node;
    }
	| type_specifier declaration_specifiers
	{
        ast_node node;
        node.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        node.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_TYPE_SPECIFIER_DECLARATION_SPECIFIERS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_specifier
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //declaration_specifiers
        $$=node;
    }
	| type_specifier
	{
        ast_node node;
        node.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        node.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_TYPE_SPECIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_specifier
        $$=node;
    }
	| type_qualifier declaration_specifiers
	{
        ast_node node;
        node.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        node.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_TYPE_QUALIFIER_DECLARATION_SPECIFIERS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_qualifier
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //declaration_specifiers
        $$=node;
    }
	| type_qualifier
	{
        ast_node node;
        node.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        node.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_TYPE_QUALIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_qualifier
        $$=node;
    }
	| function_specifier declaration_specifiers
	{
        ast_node node;
        node.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        node.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_FUNCTION_SPECIFIER_DECLARATION_SPECIFIERS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //function_specifier
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //declaration_specifiers
        $$=node;
    }
	| function_specifier
	{
        ast_node node;
        node.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        node.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_FUNCTION_SPECIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //function_specifier
        $$=node;
    }
	| alignment_specifier declaration_specifiers
	{
        ast_node node;
        node.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        node.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_ALIGNMENT_SPECIFIER_DECLARATION_SPECIFIERS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //alignment_specifier
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //declaration_specifiers
        $$=node;
    }
	| alignment_specifier
	{
        ast_node node;
        node.node_type = NODE_TYPE_DECLARATION_SPECIFIERS;
        node.node_sub_type = NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_ALIGNMENT_SPECIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //alignment_specifier
        $$=node;
    }
	;

init_declarator_list
	: init_declarator
	{
        ast_node node;
        node.node_type = NODE_TYPE_INIT_DECLARATOR_LIST;
        node.node_sub_type = NODE_TYPE_INIT_DECLARATOR_LIST_SUBTYPE_INIT_DECLARATOR;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //init_declarator
        $$=node;
    }
	| init_declarator_list COMMA init_declarator
	{
        ast_node node;
        node.node_type = NODE_TYPE_INIT_DECLARATOR_LIST;
        node.node_sub_type = NODE_TYPE_INIT_DECLARATOR_LIST_SUBTYPE_INIT_DECLARATOR_LIST_INIT_DECLARATOR;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //init_declarator_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COMMA
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //init_declarator
        $$=node;
    }
	;

init_declarator
	: declarator ASSIGN initializer
	{
        ast_node node;
        node.node_type = NODE_TYPE_INIT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_INIT_DECLARATOR_SUBTYPE_DECLARATOR_ASSIGN_INITIALIZER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //ASSIGN
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //initializer
        $$=node;
    }
	| declarator
	{
        ast_node node;
        node.node_type = NODE_TYPE_INIT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_INIT_DECLARATOR_SUBTYPE_DECLARATOR;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declarator
        $$=node;
    }
	;

storage_class_specifier
	: TYPEDEF	
	{
		/* identifiers must be flagged as TYPEDEF_NAME */
        ast_node node;
        node.node_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER;
        node.node_sub_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_TYPEDEF;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //TYPEDEF
        $$=node;
    }
	| EXTERN
	{
        ast_node node;
        node.node_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER;
        node.node_sub_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_EXTERN;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //EXTERN
        $$=node;
    }
	| STATIC
	{
        ast_node node;
        node.node_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER;
        node.node_sub_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_STATIC;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //STATIC
        $$=node;
    }
	| THREAD_LOCAL
	{
        ast_node node;
        node.node_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER;
        node.node_sub_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_THREAD_LOCAL;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //THREAD_LOCAL
        $$=node;
    }
	| AUTO
	{
        ast_node node;
        node.node_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER;
        node.node_sub_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_AUTO;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //AUTO
        $$=node;
    }
	| REGISTER
	{
        ast_node node;
        node.node_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER;
        node.node_sub_type = NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_REGISTER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //REGISTER
        $$=node;
    }
	;

type_specifier
	: VOID
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_SPECIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_VOID;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //VOID
        $$=node;
    }
	| CHAR
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_SPECIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_CHAR;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //CHAR
        $$=node;
    }
	| SHORT
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_SPECIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_SHORT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //SHORT
        $$=node;
    }
	| INT
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_SPECIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_INT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //INT
        $$=node;
    }
	| LONG
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_SPECIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_LONG;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LONG
        $$=node;
    }
	| FLOAT
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_SPECIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_FLOAT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //FLOAT
        $$=node;
    }
	| DOUBLE
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_SPECIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_DOUBLE;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //DOUBLE
        $$=node;
    }
	| SIGNED
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_SPECIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_SIGNED;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //SIGNED
        $$=node;
    }
	| UNSIGNED
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_SPECIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_UNSIGNED;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //UNSIGNED
        $$=node;
    }
	| BOOL
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_SPECIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_BOOL;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //BOOL
        $$=node;
    }
	| COMPLEX
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_SPECIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_COMPLEX;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //COMPLEX
        $$=node;
    }
	| IMAGINARY	  
	{
		/* non-mandated extension */
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_SPECIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_IMAGINARY;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //IMAGINARY
        $$=node;
    }	
	| atomic_type_specifier
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_SPECIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_ATOMIC_TYPE_SPECIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //atomic_type_specifier
        $$=node;
    }
	| struct_or_union_specifier
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_SPECIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_STRUCT_OR_UNION_SPECIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //struct_or_union_specifier
        $$=node;
    }
	| enum_specifier
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_SPECIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_ENUM_SPECIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //enum_specifier
        $$=node;
    }
	| TYPEDEF_NAME
	{
	/* 没有符号表,词法分析器无法返回 TYPEDEF_NAME 参考 https://github.com/antlr/grammars-v4/blob/master/c/C.g4
	多加一层.定义typedef_name 为identifier 注意我们的 declaration_specifiers 中允许有多个 type_specifier 以处理long int这样的type_specifier
	但是type_specifier可以推导出identifier会造成另一个问题:int a;中的a也被认为是 declaration_specifiers
	这里会造成很多奇怪问题.如果无法正确判断是否是TYPEDEF_NAME 会导致无法递归typedef即typedef int a; typedef a b;语法错误
	*/

        ast_node node;
        node.node_type = NODE_TYPE_TYPE_SPECIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_TYPEDEF_NAME;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //TYPEDEF_NAME
        $$=node;
    }
	;

typedef_name:
	IDENTIFIER

struct_or_union_specifier
	: struct_or_union LEFT_BRACE struct_declaration_list RIGHT_BRACE
	{
        ast_node node;
        node.node_type = NODE_TYPE_STRUCT_OR_UNION_SPECIFIER;
        node.node_sub_type = NODE_TYPE_STRUCT_OR_UNION_SPECIFIER_SUBTYPE_STRUCT_OR_UNION_LEFT_BRACE_STRUCT_DECLARATION_LIST_RIGHT_BRACE;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //struct_or_union
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACE
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //struct_declaration_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_BRACE
        $$=node;
    }
	| struct_or_union IDENTIFIER LEFT_BRACE struct_declaration_list RIGHT_BRACE
	{
        ast_node node;
        node.node_type = NODE_TYPE_STRUCT_OR_UNION_SPECIFIER;
        node.node_sub_type = NODE_TYPE_STRUCT_OR_UNION_SPECIFIER_SUBTYPE_STRUCT_OR_UNION_IDENTIFIER_LEFT_BRACE_STRUCT_DECLARATION_LIST_RIGHT_BRACE;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //struct_or_union
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //IDENTIFIER
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //LEFT_BRACE
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //struct_declaration_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //RIGHT_BRACE
        $$=node;
    }
	| struct_or_union IDENTIFIER
	{
        ast_node node;
        node.node_type = NODE_TYPE_STRUCT_OR_UNION_SPECIFIER;
        node.node_sub_type = NODE_TYPE_STRUCT_OR_UNION_SPECIFIER_SUBTYPE_IDENTIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //struct_or_union
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //IDENTIFIER
        $$=node;
    }
	;

struct_or_union
	: STRUCT
	{
        ast_node node;
        node.node_type = NODE_TYPE_STRUCT_OR_UNION;
        node.node_sub_type = NODE_TYPE_STRUCT_OR_UNION_SUBTYPE_STRUCT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //STRUCT
        $$=node;
    }
	| UNION
	{
        ast_node node;
        node.node_type = NODE_TYPE_STRUCT_OR_UNION;
        node.node_sub_type = NODE_TYPE_STRUCT_OR_UNION_SUBTYPE_UNION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //UNION
        $$=node;
    }
	;

struct_declaration_list
	: struct_declaration
	{
        ast_node node;
        node.node_type = NODE_TYPE_STRUCT_DECLARATION_LIST;
        node.node_sub_type = NODE_TYPE_STRUCT_DECLARATION_LIST_SUBTYPE_STRUCT_DECLARATION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //struct_declaration
        $$=node;
    }
	| struct_declaration_list struct_declaration
	{
        ast_node node;
        node.node_type = NODE_TYPE_STRUCT_DECLARATION_LIST;
        node.node_sub_type = NODE_TYPE_STRUCT_DECLARATION_LIST_SUBTYPE_STRUCT_DECLARATION_LIST_STRUCT_DECLARATION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //struct_declaration_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //struct_declaration
        $$=node;
    }
	;

struct_declaration
	: specifier_qualifier_list SEMI_COLON
	{
		/* for anonymous struct/union */
        ast_node node;
        node.node_type = NODE_TYPE_STRUCT_DECLARATION;
        node.node_sub_type = NODE_TYPE_STRUCT_DECLARATION_SUBTYPE_SPECIFIER_QUALIFIER_LIST_SEMI_COLON;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //specifier_qualifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //SEMI_COLON
        $$=node;
    }	
	| specifier_qualifier_list struct_declarator_list SEMI_COLON
	{
        ast_node node;
        node.node_type = NODE_TYPE_STRUCT_DECLARATION;
        node.node_sub_type = NODE_TYPE_STRUCT_DECLARATION_SUBTYPE_SPECIFIER_QUALIFIER_LIST_STRUT_DECLARATOR_LIST_SEMI_COLON;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //specifier_qualifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //struct_declarator_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //SEMI_COLON
        $$=node;
    }
	| static_assert_declaration
	{
        ast_node node;
        node.node_type = NODE_TYPE_STRUCT_DECLARATION;
        node.node_sub_type = NODE_TYPE_STRUCT_DECLARATION_SUBTYPE_STATIC_ASSERT_DECLARATION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //static_assert_declaration
        $$=node;
    }
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	{
        ast_node node;
        node.node_type = NODE_TYPE_SPECIFIER_QUALIFIER_LIST;
        node.node_sub_type = NODE_TYPE_SPECIFIER_QUALIFIER_LIST_SUBTYPE_TYPE_SPECIFIER_SPECIFIER_QUALIFIER_LIST;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_specifier
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //specifier_qualifier_list
        $$=node;
    }
	| type_specifier
	{
        ast_node node;
        node.node_type = NODE_TYPE_SPECIFIER_QUALIFIER_LIST;
        node.node_sub_type = NODE_TYPE_SPECIFIER_QUALIFIER_LIST_SUBTYPE_TYPE_SPECIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_specifier
        $$=node;
    }
	| type_qualifier specifier_qualifier_list
	{
        ast_node node;
        node.node_type = NODE_TYPE_SPECIFIER_QUALIFIER_LIST;
        node.node_sub_type = NODE_TYPE_SPECIFIER_QUALIFIER_LIST_SUBTYPE_TYPE_QUALIFIER_TYPE_SPECIFIER_QUALIFIER_LIST;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_qualifier
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //specifier_qualifier_list
        $$=node;
    }
	| type_qualifier
	{
        ast_node node;
        node.node_type = NODE_TYPE_SPECIFIER_QUALIFIER_LIST;
        node.node_sub_type = NODE_TYPE_SPECIFIER_QUALIFIER_LIST_SUBTYPE_TYPE_QUALIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_qualifier
        $$=node;
    }
	;

struct_declarator_list
	: struct_declarator
	{
        ast_node node;
        node.node_type = NODE_TYPE_STRUCT_DECLARATOR_LIST;
        node.node_sub_type = NODE_TYPE_STRUCT_DECLARATOR_LIST_SUBTYPE_STRUCT_DECLARATOR;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //struct_declarator
        $$=node;
    }
	| struct_declarator_list COMMA struct_declarator
	{
        ast_node node;
        node.node_type = NODE_TYPE_STRUCT_DECLARATOR_LIST;
        node.node_sub_type = NODE_TYPE_STRUCT_DECLARATOR_LIST_SUBTYPE_STRUCT_DECLARATOR_LIST_COMMA_STRUCT_DECLARATOR;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //struct_declarator_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COMMA
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //struct_declarator
        $$=node;
    }
	;

struct_declarator
	: COLON constant_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_STRUCT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_STRUCT_DECLARATOR_SUBTYPE_COLON_CONSTANT_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //COLON
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //constant_expression
        $$=node;
    }
	| declarator COLON constant_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_STRUCT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_STRUCT_DECLARATOR_SUBTYPE_DECLARATOR_COLON_CONSTANT_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COLON
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //constant_expression
        $$=node;
    }
	| declarator
	{
        ast_node node;
        node.node_type = NODE_TYPE_STRUCT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_STRUCT_DECLARATOR_SUBTYPE_DECLARATOR;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declarator
        $$=node;
    }
	;

enum_specifier
	: ENUM LEFT_BRACE enumerator_list RIGHT_BRACE
	{
        ast_node node;
        node.node_type = NODE_TYPE_ENUM_SPECIFIER;
        node.node_sub_type = NODE_TYPE_ENUM_SPECIFIER_SUBTYPE_ENUM_LEFT_BRACE_ENUMATOR_LIST_RIGHT_BRACE;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //ENUM
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACE
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //enumerator_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_BRACE
        $$=node;
    }
	| ENUM LEFT_BRACE enumerator_list COMMA RIGHT_BRACE
	{
        ast_node node;
        node.node_type = NODE_TYPE_ENUM_SPECIFIER;
        node.node_sub_type = NODE_TYPE_ENUM_SPECIFIER_SUBTYPE_ENUM_LEFT_BRACE_ENUMATOR_LIST_COMMA_RIGHT_BRACE;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //ENUM
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACE
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //enumerator_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //COMMA
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //RIGHT_BRACE
        $$=node;
    }
	| ENUM IDENTIFIER LEFT_BRACE enumerator_list RIGHT_BRACE
	{
        ast_node node;
        node.node_type = NODE_TYPE_ENUM_SPECIFIER;
        node.node_sub_type = NODE_TYPE_ENUM_SPECIFIER_SUBTYPE_ENUM_IDENTIFER_LEFT_BRACE_ENUMATOR_LIST_RIGHT_BRACE;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //ENUM
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //IDENTIFIER
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //LEFT_BRACE
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //enumerator_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //RIGHT_BRACE
        $$=node;
    }
	| ENUM IDENTIFIER LEFT_BRACE enumerator_list COMMA RIGHT_BRACE
	{
        ast_node node;
        node.node_type = NODE_TYPE_ENUM_SPECIFIER;
        node.node_sub_type = NODE_TYPE_ENUM_SPECIFIER_SUBTYPE_ENUM_IDENTIFER_LEFT_BRACE_ENUMATOR_LIST_COMMA_RIGHT_BRACE;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //ENUM
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //IDENTIFIER
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //LEFT_BRACE
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //enumerator_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //COMMA
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($6))); //RIGHT_BRACE
        $$=node;
    }
	| ENUM IDENTIFIER
	{
        ast_node node;
        node.node_type = NODE_TYPE_ENUM_SPECIFIER;
        node.node_sub_type = NODE_TYPE_ENUM_SPECIFIER_SUBTYPE_ENUM_IDENTIFER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //ENUM
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //IDENTIFIER
        $$=node;
    }
	;

enumerator_list
	: enumerator
	{
        ast_node node;
        node.node_type = NODE_TYPE_ENUMERATOR_LIST;
        node.node_sub_type = NODE_TYPE_ENUMERATOR_LIST_SUBTYPE_ENUMERATOR;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //enumerator
        $$=node;
    }
	| enumerator_list COMMA enumerator
	{
        ast_node node;
        node.node_type = NODE_TYPE_ENUMERATOR_LIST;
        node.node_sub_type = NODE_TYPE_ENUMERATOR_LIST_SUBTYPE_ENUMERATOR_LIST_COMMA_ENUMERATOR;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //enumerator_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COMMA
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //enumerator
        $$=node;
    }
	;

enumerator
	: enumeration_constant ASSIGN constant_expression
	{
	/* identifiers must be flagged as ENUMERATION_CONSTANT */
        ast_node node;
        node.node_type = NODE_TYPE_ENUMERATOR;
        node.node_sub_type = NODE_TYPE_ENUMERATOR_SUBTYPE_ENUMERATION_CONSTANT_ASSIGN_CONSTANT_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //enumeration_constant
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //ASSIGN
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //constant_expression
        $$=node;
    }
	| enumeration_constant
	{
        ast_node node;
        node.node_type = NODE_TYPE_ENUMERATOR;
        node.node_sub_type = NODE_TYPE_ENUMERATOR_SUBTYPE_ENUMERATION_CONSTANT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //enumeration_constant
        $$=node;
    }
	;

atomic_type_specifier
	: ATOMIC LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_ATOMIC_TYPE_SPECIFIER;
        node.node_sub_type = NODE_TYPE_ATOMIC_TYPE_SPECIFIER_SUBTYPE_ATOMIC_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //ATOMIC
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //type_name
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_PARENTHESIS
        $$=node;
    }
	;

type_qualifier
	: CONST
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_QUALIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_QUALIFIER_SUBTYPE_CONST;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //CONST
        $$=node;
    }
	| RESTRICT
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_QUALIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_QUALIFIER_SUBTYPE_RESTRICT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //RESTRICT
        $$=node;
    }
	| VOLATILE
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_QUALIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_QUALIFIER_SUBTYPE_VOLATILE;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //VOLATILE
        $$=node;
    }
	| ATOMIC
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_QUALIFIER;
        node.node_sub_type = NODE_TYPE_TYPE_QUALIFIER_SUBTYPE_ATOMIC;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //ATOMIC
        $$=node;
    }
	;

function_specifier
	: INLINE
	{
        ast_node node;
        node.node_type = NODE_TYPE_FUNCTION_SPECIFIER;
        node.node_sub_type = NODE_TYPE_FUNCTION_SPECIFIER_SUBTYPE_INLINE;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //INLINE
        $$=node;
    }
	| NORETURN
	{
        ast_node node;
        node.node_type = NODE_TYPE_FUNCTION_SPECIFIER;
        node.node_sub_type = NODE_TYPE_FUNCTION_SPECIFIER_SUBTYPE_NORETURN;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //NORETURN
        $$=node;
    }
	;

alignment_specifier
	: ALIGNAS LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_ALIGNMENT_SPECIFIER;
        node.node_sub_type = NODE_TYPE_ALIGNMENT_SPECIFIER_SUBTYPE_LEFT_PARENTHESIS_CONSTANT_EXPRESSION_RIGHT_PARENTHESIS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //ALIGNAS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //type_name
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_PARENTHESIS
        $$=node;
    }
	| ALIGNAS LEFT_PARENTHESIS constant_expression RIGHT_PARENTHESIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_ALIGNMENT_SPECIFIER;
        node.node_sub_type = NODE_TYPE_ALIGNMENT_SPECIFIER_SUBTYPE_ALIGNAS_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //ALIGNAS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //constant_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_PARENTHESIS
        $$=node;
    }
	;

declarator
	: pointer direct_declarator
	{
        ast_node node;
        node.node_type = NODE_TYPE_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DECLARATOR_SUBTYPE_POINTER_DIRECT_DECLARATOR;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //pointer
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //direct_declarator
        $$=node;
    }
	| direct_declarator
	{
        ast_node node;
        node.node_type = NODE_TYPE_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_declarator
        $$=node;
    }
	;

direct_declarator
	: IDENTIFIER
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_IDENTIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //IDENTIFIER
        $$=node;
    }
	| LEFT_PARENTHESIS declarator RIGHT_PARENTHESIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_LEFT_PARENTHESIS_DECLARATOR_RIGHT_PARENTHESIS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_PARENTHESIS
        $$=node;
    }
	| direct_declarator LEFT_BRACKET RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_BRACKET_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_BRACKET
        $$=node;
    }
	| direct_declarator LEFT_BRACKET MUL RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_BRACKET_MUL_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //MUL
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_BRACKET
        $$=node;
    }
	| direct_declarator LEFT_BRACKET STATIC type_qualifier_list assignment_expression RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_BRACKET_STATIC_TYPE_QUALIFIER_LIST_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //STATIC
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //type_qualifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //assignment_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($6))); //RIGHT_BRACKET
        $$=node;
    }
	| direct_declarator LEFT_BRACKET STATIC assignment_expression RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_BRACKET_STATIC_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //STATIC
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //assignment_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //RIGHT_BRACKET
        $$=node;
    }
	| direct_declarator LEFT_BRACKET type_qualifier_list MUL RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_BRACKET_TYPE_QUALIFIER_LIST_MUL_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //type_qualifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //MUL
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //RIGHT_BRACKET
        $$=node;
    }
	| direct_declarator LEFT_BRACKET type_qualifier_list STATIC assignment_expression RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_BRACKET_TYPE_QUALIFIER_LIST_STATIC_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //type_qualifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //STATIC
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //assignment_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($6))); //RIGHT_BRACKET
        $$=node;
    }
	| direct_declarator LEFT_BRACKET type_qualifier_list assignment_expression RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_BRACKET_TYPE_QUALIFIER_LIST_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //type_qualifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //assignment_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //RIGHT_BRACKET
        $$=node;
    }
	| direct_declarator LEFT_BRACKET type_qualifier_list RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_BRACKET_TYPE_QUALIFIER_LIST_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //type_qualifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_BRACKET
        $$=node;
    }
	| direct_declarator LEFT_BRACKET assignment_expression RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_BRACKET_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //assignment_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_BRACKET
        $$=node;
    }
	| direct_declarator LEFT_PARENTHESIS parameter_type_list RIGHT_PARENTHESIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_PARENTHESIS_PARAMETER_TYPE_LIST_RIGHT_PARENTHESIS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //parameter_type_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_PARENTHESIS
        $$=node;
    }
	| direct_declarator LEFT_PARENTHESIS RIGHT_PARENTHESIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_PARENTHESIS_RIGHT_PARENTHESIS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_PARENTHESIS
        $$=node;
    }
	| direct_declarator LEFT_PARENTHESIS identifier_list RIGHT_PARENTHESIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_PARENTHESIS_IDENTIFIER_LIST_RIGHT_PARENTHESIS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //identifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_PARENTHESIS
        $$=node;
    }
	;

pointer
	: MUL type_qualifier_list pointer
	{
        ast_node node;
        node.node_type = NODE_TYPE_POINTER;
        node.node_sub_type = NODE_TYPE_POINTER_SUBTYPE_MUL_TYPE_QUALIFIER_LIST_POINTER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //MUL
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //type_qualifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //pointer
        $$=node;
    }
	| MUL type_qualifier_list
	{
        ast_node node;
        node.node_type = NODE_TYPE_POINTER;
        node.node_sub_type = NODE_TYPE_POINTER_SUBTYPE_MUL_TYPE_QUALIFIER_LIST;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //MUL
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //type_qualifier_list
        $$=node;
    }
	| MUL pointer
	{
        ast_node node;
        node.node_type = NODE_TYPE_POINTER;
        node.node_sub_type = NODE_TYPE_POINTER_SUBTYPE_MUL_POINTER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //MUL
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //pointer
        $$=node;
    }
	| MUL
	{
        ast_node node;
        node.node_type = NODE_TYPE_POINTER;
        node.node_sub_type = NODE_TYPE_POINTER_SUBTYPE_MUL;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //MUL
        $$=node;
    }
	;

type_qualifier_list
	: type_qualifier
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_QUALIFIER_LIST;
        node.node_sub_type = NODE_TYPE_TYPE_QUALIFIER_LIST_SUBTYPE_TYPE_QUALIFIER_LIST_TYPE_QUALIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_qualifier
        $$=node;
    }
	| type_qualifier_list type_qualifier
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_QUALIFIER_LIST;
        node.node_sub_type = NODE_TYPE_TYPE_QUALIFIER_LIST_SUBTYPE_TYPE_QUALIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_qualifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //type_qualifier
        $$=node;
    }
	;


parameter_type_list
	: parameter_list COMMA ELLIPSIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_PARAMETER_TYPE_LIST;
        node.node_sub_type = NODE_TYPE_PARAMETER_TYPE_LIST_SUBTYPE_PARAMETER_LIST_COMMA_ELLIPSE;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //parameter_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COMMA
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //ELLIPSIS
        $$=node;
    }
	| parameter_list
	{
        ast_node node;
        node.node_type = NODE_TYPE_PARAMETER_TYPE_LIST;
        node.node_sub_type = NODE_TYPE_PARAMETER_TYPE_LIST_SUBTYPE_PARAMETER_LIST;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //type_qualifier
        $$=node;
    }
	;

parameter_list
	: parameter_declaration
	{
        ast_node node;
        node.node_type = NODE_TYPE_PARAMETER_LIST;
        node.node_sub_type = NODE_TYPE_PARAMETER_LIST_SUBTYPE_PARAMETER_DECLARATION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //parameter_declaration
        $$=node;
    }
	| parameter_list COMMA parameter_declaration
	{
        ast_node node;
        node.node_type = NODE_TYPE_PARAMETER_LIST;
        node.node_sub_type = NODE_TYPE_PARAMETER_LIST_SUBTYPE_PARAMETER_LIST_COMMA_PARAMETER_DECLARATION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //parameter_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COMMA
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //parameter_declaration
        $$=node;
    }
	;

parameter_declaration
	: declaration_specifiers declarator
	{
        ast_node node;
        node.node_type = NODE_TYPE_PARAMETER_DECLARATION;
        node.node_sub_type = NODE_TYPE_PARAMETER_DECLARATION_SUBTYPE_DECLARATION_SPECIFIERS_DECLARATOR;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declaration_specifiers
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //declarator
        $$=node;
    }
	| declaration_specifiers abstract_declarator
	{
        ast_node node;
        node.node_type = NODE_TYPE_PARAMETER_DECLARATION;
        node.node_sub_type = NODE_TYPE_PARAMETER_DECLARATION_SUBTYPE_DECLARATION_SPECIFIERS_ABSTRACT_DECLARATOR;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declaration_specifiers
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //abstract_declarator
        $$=node;
    }
	| declaration_specifiers
	{
        ast_node node;
        node.node_type = NODE_TYPE_PARAMETER_DECLARATION;
        node.node_sub_type = NODE_TYPE_PARAMETER_DECLARATION_SUBTYPE_DECLARATION_SPECIFIERS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declaration_specifiers
        $$=node;
    }
	;

identifier_list
	: IDENTIFIER
	{
        ast_node node;
        node.node_type = NODE_TYPE_IDENTIFIER_LIST;
        node.node_sub_type = NODE_TYPE_IDENTIFIER_LIST_SUBTYPE_IDENTIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //IDENTIFIER
        $$=node;
    }
	| identifier_list COMMA IDENTIFIER
	{
        ast_node node;
        node.node_type = NODE_TYPE_IDENTIFIER_LIST;
        node.node_sub_type = NODE_TYPE_IDENTIFIER_LIST_SUBTYPE_IDENTIFIER_LIST_COMMA_IDENTIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //identifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COMMA
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //IDENTIFIER
        $$=node;
    }
	;

type_name
	: specifier_qualifier_list abstract_declarator
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_NAME;
        node.node_sub_type = NODE_TYPE_TYPE_NAME_SUBTYPE_SPECIFIER_QUALIFIER_LIST_ABSTRACT_DECLARATOR;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //specifier_qualifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //abstract_declarator
        $$=node;
    }
	| specifier_qualifier_list
	{
        ast_node node;
        node.node_type = NODE_TYPE_TYPE_NAME;
        node.node_sub_type = NODE_TYPE_TYPE_NAME_SUBTYPE_SPECIFIER_QUALIFIER_LIST;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //specifier_qualifier_list
        $$=node;
    }
	;

abstract_declarator
	: pointer direct_abstract_declarator
	{
        ast_node node;
        node.node_type = NODE_TYPE_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_ABSTRACT_DECLARATOR_SUBTYPE_POINTER_DIRECT_ABSTRACT_DECLARATOR;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //pointer
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //direct_abstract_declarator
        $$=node;
    }
	| pointer
	{
        ast_node node;
        node.node_type = NODE_TYPE_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_ABSTRACT_DECLARATOR_SUBTYPE_POINTER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //pointer
        $$=node;
    }
	| direct_abstract_declarator
	{
        ast_node node;
        node.node_type = NODE_TYPE_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_abstract_declarator
        $$=node;
    }
	;

direct_abstract_declarator
	: LEFT_PARENTHESIS abstract_declarator RIGHT_PARENTHESIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_PARENTHESIS_ABSTRACT_DECLARATOR_RIGHT_PARENTHESIS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //abstract_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_PARENTHESIS
        $$=node;
    }
	| LEFT_BRACKET RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_BRACKET_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //RIGHT_PARENTHESIS
        $$=node;
    }
	| LEFT_BRACKET MUL RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_BRACKET_MUL_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //MUL
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_BRACKET
        $$=node;
    }
	| LEFT_BRACKET STATIC type_qualifier_list assignment_expression RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_BRACKET_STATIC_TYPE_QUALIFIER_LIST_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //STATIC
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //type_qualifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //assignment_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //RIGHT_BRACKET
        $$=node;
    }
	| LEFT_BRACKET STATIC assignment_expression RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_BRACKET_STATIC_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //STATIC
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //assignment_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_BRACKET
        $$=node;
    }
	| LEFT_BRACKET type_qualifier_list STATIC assignment_expression RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_BRACKET_TYPE_QUALIFIER_LIST_STATIC_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //type_qualifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //STATIC
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //assignment_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //RIGHT_BRACKET
        $$=node;
    }
	| LEFT_BRACKET type_qualifier_list assignment_expression RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_BRACKET_TYPE_QUALIFIER_LIST_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //type_qualifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //assignment_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_BRACKET
        $$=node;
    }
	| LEFT_BRACKET type_qualifier_list RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_BRACKET_TYPE_QUALIFIER_LIST_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //type_qualifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_BRACKET
        $$=node;
    }
	| LEFT_BRACKET assignment_expression RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_BRACKET_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //assignment_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_BRACKET
        $$=node;
    }
	| direct_abstract_declarator LEFT_BRACKET RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_BRACKET_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_abstract_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_BRACKET
        $$=node;
    }
	| direct_abstract_declarator LEFT_BRACKET MUL RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_BRACKET_MUL_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_abstract_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //MUL
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_BRACKET
        $$=node;
    }
	| direct_abstract_declarator LEFT_BRACKET STATIC type_qualifier_list assignment_expression RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_BRACKET_STATIC_TYPE_QUALIFIER_LIST_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_abstract_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //STATIC
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //type_qualifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //assignment_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($6))); //RIGHT_BRACKET
        $$=node;
    }
	| direct_abstract_declarator LEFT_BRACKET STATIC assignment_expression RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_BRACKET_STATIC_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_abstract_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //STATIC
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //assignment_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //RIGHT_BRACKET
        $$=node;
    }
	| direct_abstract_declarator LEFT_BRACKET type_qualifier_list assignment_expression RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_BRACKET_TYPE_QUALIFIER_LIST_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_abstract_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //type_qualifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //assignment_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //RIGHT_BRACKET
        $$=node;
    }
	| direct_abstract_declarator LEFT_BRACKET type_qualifier_list STATIC assignment_expression RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_BRACKET_TYPE_QUALIFIER_LIST_STATIC_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_abstract_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //type_qualifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //STATIC
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //assignment_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($6))); //RIGHT_BRACKET
        $$=node;
    }
	| direct_abstract_declarator LEFT_BRACKET type_qualifier_list RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_BRACKET_TYPE_QUALIFIER_LIST_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_abstract_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //type_qualifier_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_BRACKET
        $$=node;
    }
	| direct_abstract_declarator LEFT_BRACKET assignment_expression RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_BRACKET_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_abstract_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //assignment_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_BRACKET
        $$=node;
    }
	| LEFT_PARENTHESIS RIGHT_PARENTHESIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_PARENTHESIS_RIGHT_PARENTHESIS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //RIGHT_PARENTHESIS
        $$=node;
    }
	| LEFT_PARENTHESIS parameter_type_list RIGHT_PARENTHESIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_PARENTHESIS_PARAMETER_TYPE_LIST_RIGHT_PARENTHESIS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //parameter_type_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_PARENTHESIS
        $$=node;
    }
	| direct_abstract_declarator LEFT_PARENTHESIS RIGHT_PARENTHESIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_PARENTHESIS_RIGHT_PARENTHESIS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_abstract_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_PARENTHESIS
        $$=node;
    }
	| direct_abstract_declarator LEFT_PARENTHESIS parameter_type_list RIGHT_PARENTHESIS
	{
        ast_node node;
        node.node_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR;
        node.node_sub_type = NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_PARENTHESIS_PARAMETER_TYPE_LIST_RIGHT_PARENTHESIS;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //direct_abstract_declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //parameter_type_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_PARENTHESIS
        $$=node;
    }
	;

initializer
	: LEFT_BRACE initializer_list RIGHT_BRACE
	{
        ast_node node;
        node.node_type = NODE_TYPE_INITIALIZER;
        node.node_sub_type = NODE_TYPE_INITIALIZER_SUBTYPE_LEFT_BRACE_INITIALIZER__LIST_RIGHT_BRACE;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_BRACE
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //initializer_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_BRACE
        $$=node;
    }
	| LEFT_BRACE initializer_list COMMA RIGHT_BRACE
	{
        ast_node node;
        node.node_type = NODE_TYPE_INITIALIZER;
        node.node_sub_type = NODE_TYPE_INITIALIZER_SUBTYPE_LEFT_BRACE_INITIALIZER__LIST_COMMA_RIGHT_BRACE;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_BRACE
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //initializer_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //COMMA
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_BRACE
        $$=node;
    }
	| assignment_expression
	{
        ast_node node;
        node.node_type = NODE_TYPE_INITIALIZER;
        node.node_sub_type = NODE_TYPE_INITIALIZER_SUBTYPE_ASSIGNMENT_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //assignment_expression
        $$=node;
    }
	;

initializer_list
	: designation initializer
	{
        ast_node node;
        node.node_type = NODE_TYPE_INITIALIZER_LIST;
        node.node_sub_type = NODE_TYPE_INITIALIZER_LIST_SUBTYPE_DESIGNATION_INITIALIZER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //designation
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //initializer
        $$=node;
    }
	| initializer
	{
        ast_node node;
        node.node_type = NODE_TYPE_INITIALIZER_LIST;
        node.node_sub_type = NODE_TYPE_INITIALIZER_LIST_SUBTYPE_INITIALIZER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //initializer
        $$=node;
    }
	| initializer_list COMMA designation initializer
	{
        ast_node node;
        node.node_type = NODE_TYPE_INITIALIZER_LIST;
        node.node_sub_type = NODE_TYPE_INITIALIZER_LIST_SUBTYPE_INITIALIZER_LIST_COMMA_DESIGNATION_INITIALIZER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //initializer_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COMMA
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //designation
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //initializer
        $$=node;
    }
	| initializer_list COMMA initializer
	{
        ast_node node;
        node.node_type = NODE_TYPE_INITIALIZER_LIST;
        node.node_sub_type = NODE_TYPE_INITIALIZER_LIST_SUBTYPE_INITIALIZER_LIST_COMMA_INITIALIZER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //initializer_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COMMA
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //initializer
        $$=node;
    }
	;

designation
	: designator_list ASSIGN
	{
        ast_node node;
        node.node_type = NODE_TYPE_DESIGNATION;
        node.node_sub_type = NODE_TYPE_DESIGNATION_SUBTYPE_DESIGNATOR_LIST_ASSIGN;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //designator_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //ASSIGN
        $$=node;
    }
	;

designator_list
	: designator
	{
        ast_node node;
        node.node_type = NODE_TYPE_DESIGNATOR_LIST;
        node.node_sub_type = NODE_TYPE_DESIGNATOR_LIST_SUBTYPE_DESIGNATOR;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //designator
        $$=node;
    }
	| designator_list designator
	{
        ast_node node;
        node.node_type = NODE_TYPE_DESIGNATOR_LIST;
        node.node_sub_type = NODE_TYPE_DESIGNATOR_LIST_SUBTYPE_DESIGNATOR_LIST_DESIGNATOR;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //designator_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //designator
        $$=node;
    }
	;

designator
	: LEFT_BRACKET constant_expression RIGHT_BRACKET
	{
        ast_node node;
        node.node_type = NODE_TYPE_DESIGNATOR;
        node.node_sub_type = NODE_TYPE_DESIGNATOR_SUBTYPE_LEFT_BRACKET_CONSTANT_EXPRESSION_RIGHT_BRACKET;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_BRACKET
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //constant_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_BRACKET
        $$=node;
    }
	| DOT IDENTIFIER
	{
        ast_node node;
        node.node_type = NODE_TYPE_DESIGNATOR;
        node.node_sub_type = NODE_TYPE_DESIGNATOR_SUBTYPE_DOT_IDENTIFIER;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //DOT
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //IDENTIFIER
        $$=node;
    }
	;

static_assert_declaration
	: STATIC_ASSERT LEFT_PARENTHESIS constant_expression COMMA STRING_LITERAL RIGHT_PARENTHESIS SEMI_COLON
	{
        ast_node node;
        node.node_type = NODE_TYPE_STATIC_ASSERT_DECLARATION;
        node.node_sub_type = NODE_TYPE_STATIC_ASSERT_DECLARATION_SUBTYPE_STATIC_ASSERT_LEFT_PARENTHESIS_CONSTANT_EXPRESSION_COMMA_STRING_LITERAL_RIGHT_PARENTHESIS_SEMI_COLON;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //STATIC_ASSERT
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //constant_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //COMMA
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //STRING_LITERAL
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($6))); //RIGHT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($7))); //SEMI_COLON
        $$=node;
    }
	;

statement
	: labeled_statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_STATEMENT;
        node.node_sub_type = NODE_TYPE_STATEMENT_SUBTYPE_LABELED_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //labeled_statement
        $$=node;
    }
	| compound_statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_STATEMENT;
        node.node_sub_type = NODE_TYPE_STATEMENT_SUBTYPE_COMPOUND_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //compound_statement
        $$=node;
    }
	| expression_statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_STATEMENT;
        node.node_sub_type = NODE_TYPE_STATEMENT_SUBTYPE_EXPRESSION_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //expression_statement
        $$=node;
    }
	| selection_statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_STATEMENT;
        node.node_sub_type = NODE_TYPE_STATEMENT_SUBTYPE_SELECTION_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //selection_statement
        $$=node;
    }
	| iteration_statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_STATEMENT;
        node.node_sub_type = NODE_TYPE_STATEMENT_SUBTYPE_ITERATION_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //iteration_statement
        $$=node;
    }
	| jump_statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_STATEMENT;
        node.node_sub_type = NODE_TYPE_STATEMENT_SUBTYPE_JUMP_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //jump_statement
        $$=node;
    }
	;

labeled_statement
	: IDENTIFIER COLON statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_LABELED_STATEMENT;
        node.node_sub_type = NODE_TYPE_LABELED_STATEMENT_SUBTYPE_IDENTIFIER_COLON_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //IDENTIFIER
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COLON
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //statement
        $$=node;
    }
	| CASE constant_expression COLON statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_LABELED_STATEMENT;
        node.node_sub_type = NODE_TYPE_LABELED_STATEMENT_SUBTYPE_CASE_CONSTANT_EXPRESSION_COLON_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //CASE
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //constant_expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //COLON
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //statement
        $$=node;
    }
	| DEFAULT COLON statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_LABELED_STATEMENT;
        node.node_sub_type = NODE_TYPE_LABELED_STATEMENT_SUBTYPE_DEFAULT_COLON_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //DEFAULT
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //COLON
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //statement
        $$=node;
    }
	;

compound_statement
	: LEFT_BRACE RIGHT_BRACE
	{
        ast_node node;
        node.node_type = NODE_TYPE_COMPOUND_STATEMENT;
        node.node_sub_type = NODE_TYPE_COMPOUND_STATEMENT_SUBTYPE_LEFT_BRACE_RIGHT_BRACE;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_BRACE
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //RIGHT_BRACE
        $$=node;
    }
	| LEFT_BRACE  block_item_list RIGHT_BRACE
	{
        ast_node node;
        node.node_type = NODE_TYPE_COMPOUND_STATEMENT;
        node.node_sub_type = NODE_TYPE_COMPOUND_STATEMENT_SUBTYPE_LEFT_BRACE_BLOCK_ITEM_LIST_RIGHT_BRACE;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //LEFT_BRACE
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //block_item_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //RIGHT_BRACE
        $$=node;
    }
	;

block_item_list
	: block_item
	{
        ast_node node;
        node.node_type = NODE_TYPE_BLOCK_ITEM_LIST;
        node.node_sub_type = NODE_TYPE_BLOCK_ITEM_LIST_SUBTYPE_BLOCK_ITEM;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //block_item
        $$=node;
    }
	| block_item_list block_item
	{
        ast_node node;
        node.node_type = NODE_TYPE_BLOCK_ITEM_LIST;
        node.node_sub_type = NODE_TYPE_BLOCK_ITEM_LIST_SUBTYPE_BLOCK_ITEM_LIST_BLOCK_ITEM;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //block_item_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //block_item
        $$=node;
    }
	;

block_item
	: declaration
	{
        ast_node node;
        node.node_type = NODE_TYPE_BLOCK_ITEM;
        node.node_sub_type = NODE_TYPE_BLOCK_ITEM_SUBTYPE_DECLARATION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declaration
        $$=node;
    }
	| statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_BLOCK_ITEM;
        node.node_sub_type = NODE_TYPE_BLOCK_ITEM_SUBTYPE_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //statement
        $$=node;
    }
	;

expression_statement
	: SEMI_COLON
	{
        ast_node node;
        node.node_type = NODE_TYPE_EXPRESSION_STATEMENT;
        node.node_sub_type = NODE_TYPE_EXPRESSION_STATEMENT_SUBTYPE_SEMI_COLON;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //SEMI_COLON
        $$=node;
    }
	| expression SEMI_COLON
	{
        ast_node node;
        node.node_type = NODE_TYPE_EXPRESSION_STATEMENT;
        node.node_sub_type = NODE_TYPE_EXPRESSION_STATEMENT_SUBTYPE_EXPRESSION_SEMI_COLON;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //SEMI_COLON
        $$=node;
    }
	;

selection_statement
	: IF LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement ELSE statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_SELECTION_STATEMENT;
        node.node_sub_type = NODE_TYPE_SELECTION_STATEMENT_SUBTYPE_IF_LEFT_PARENTHESIS_EXPRESSION_RIGHT_PARENTHESIS_STATEMENT_ELSE_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //IF
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //statement
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($6))); //ELSE
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($7))); //statement
        $$=node;
    }
	| IF LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_SELECTION_STATEMENT;
        node.node_sub_type = NODE_TYPE_SELECTION_STATEMENT_SUBTYPE_IF_LEFT_PARENTHESIS_EXPRESSION_RIGHT_PARENTHESIS_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //IF
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //statement
        $$=node;
    }
	| SWITCH LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_SELECTION_STATEMENT;
        node.node_sub_type = NODE_TYPE_SELECTION_STATEMENT_SUBTYPE_SWITCH_LEFT_PARENTHESIS_EXPRESSION_RIGHT_PARENTHESIS_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //SWITCH
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //statement
        $$=node;
    }
	;

iteration_statement
	: WHILE LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_ITERATION_STATEMENT;
        node.node_sub_type = NODE_TYPE_ITERATION_STATEMENT_SUBTYPE_WHILE_LEFT_PARENTHESIS_EXPRESSION_RIGHT_PARENTHESIS_EXPRESSION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //WHILE
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //RIGHT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //statement
        $$=node;
    }
	| DO statement WHILE LEFT_PARENTHESIS expression RIGHT_PARENTHESIS SEMI_COLON
	{
        ast_node node;
        node.node_type = NODE_TYPE_ITERATION_STATEMENT;
        node.node_sub_type = NODE_TYPE_ITERATION_STATEMENT_SUBTYPE_DO_STATEMENT_WHILE_LEFT_PARENTHESIS_EXPRESSION_RIGHT_PARENTHESIS_SEMI_COLON;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //DO
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //statement
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //WHILE
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($6))); //RIGHT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($7))); //SEMI_COLON
        $$=node;
    }
	| FOR LEFT_PARENTHESIS expression_statement expression_statement RIGHT_PARENTHESIS statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_ITERATION_STATEMENT;
        node.node_sub_type = NODE_TYPE_ITERATION_STATEMENT_SUBTYPE_FOR_LEFT_PARENTHESIS_EXPRESSION_STATEMENT_EXPRESSION_STATEMENT_RIGHT_PARENTHESIS_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //FOR
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //expression_statement
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //expression_statement
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //RIGHT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($6))); //statement
        $$=node;
    }
	| FOR LEFT_PARENTHESIS expression_statement expression_statement expression RIGHT_PARENTHESIS statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_ITERATION_STATEMENT;
        node.node_sub_type = NODE_TYPE_ITERATION_STATEMENT_SUBTYPE_FOR_LEFT_PARENTHESIS_EXPRESSION_STATEMENT_EXPRESSION_STATEMENT_EXPRESSION_RIGHT_PARENTHESIS_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //FOR
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //expression_statement
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //expression_statement
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($6))); //RIGHT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($7))); //statement
        $$=node;
    }
	| FOR LEFT_PARENTHESIS declaration expression_statement RIGHT_PARENTHESIS statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_ITERATION_STATEMENT;
        node.node_sub_type = NODE_TYPE_ITERATION_STATEMENT_SUBTYPE_FOR_LEFT_PARENTHESIS_DECLARATION_EXPRESSION_STATEMENT_RIGHT_PARENTHESIS_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //FOR
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //declaration
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //expression_statement
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //RIGHT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($6))); //statement
        $$=node;
    }
	| FOR LEFT_PARENTHESIS declaration expression_statement expression RIGHT_PARENTHESIS statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_ITERATION_STATEMENT;
        node.node_sub_type = NODE_TYPE_ITERATION_STATEMENT_SUBTYPE_FOR_LEFT_PARENTHESIS_DECLARATION_EXPRESSION_STATEMENT_EXPRESSION_RIGHT_PARENTHESIS_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //FOR
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //LEFT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //declaration
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //expression_statement
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($5))); //expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($6))); //RIGHT_PARENTHESIS
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($7))); //statement
        $$=node;
    }
	;

jump_statement
	: GOTO IDENTIFIER SEMI_COLON
	{
        ast_node node;
        node.node_type = NODE_TYPE_JUMP_STATEMENT;
        node.node_sub_type = NODE_TYPE_JUMP_STATEMENT_SUBTYPE_GOTO_IDENTIFIER_SEMI_COLON;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //GOTO
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //IDENTIFIER
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //SEMI_COLON
        $$=node;
    }
	| CONTINUE SEMI_COLON
	{
        ast_node node;
        node.node_type = NODE_TYPE_JUMP_STATEMENT;
        node.node_sub_type = NODE_TYPE_JUMP_STATEMENT_SUBTYPE_CONTINUE_SEMI_COLON;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //CONTINUE
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //SEMI_COLON
        $$=node;
    }
	| BREAK SEMI_COLON
	{
        ast_node node;
        node.node_type = NODE_TYPE_JUMP_STATEMENT;
        node.node_sub_type = NODE_TYPE_JUMP_STATEMENT_SUBTYPE_BREAK_SEMI_COLON;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //BREAK
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //SEMI_COLON
        $$=node;
    }
	| RETURN SEMI_COLON
	{
        ast_node node;
        node.node_type = NODE_TYPE_JUMP_STATEMENT;
        node.node_sub_type = NODE_TYPE_JUMP_STATEMENT_SUBTYPE_RETURN_SEMI_COLON;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //RETURN
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //SEMI_COLON
        $$=node;
    }
	| RETURN expression SEMI_COLON
	{
        ast_node node;
        node.node_type = NODE_TYPE_JUMP_STATEMENT;
        node.node_sub_type = NODE_TYPE_JUMP_STATEMENT_SUBTYPE_RETURN_EXPRESSION_SEMI_COLON;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //RETURN
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //expression
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //SEMI_COLON
        $$=node;
    }
	;

translation_unit
	: external_declaration
	{
        ast_node node;
        node.node_type = NODE_TYPE_TRANSLATION_UNIT;
        node.node_sub_type = NODE_TYPE_TRANSLATION_UNIT_SUBTYPE_EXTERNAL_DECLARATION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //external_declaration
		translation_unit=std::shared_ptr<ast_node>(new ast_node(node));
		$$=node;
    }
	| translation_unit external_declaration
	{
        ast_node node;
        node.node_type = NODE_TYPE_TRANSLATION_UNIT;
        node.node_sub_type = NODE_TYPE_TRANSLATION_UNIT_SUBTYPE_TRANSLATION_UNIT_EXTERNAL_DECLARATION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //translation_unit
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //external_declaration
		translation_unit=std::shared_ptr<ast_node>(new ast_node(node));
		$$=node;
    }
	;

external_declaration
	: function_definition
	{
        ast_node node;
        node.node_type = NODE_TYPE_EXTERNAL_DECLARATION;
        node.node_sub_type = NODE_TYPE_EXTERNAL_DECLARATION_SUBTYPE_FUNCTION_DEFINITION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //function_definition
        $$=node;
    }
	| declaration
	{
        ast_node node;
        node.node_type = NODE_TYPE_EXTERNAL_DECLARATION;
        node.node_sub_type = NODE_TYPE_EXTERNAL_DECLARATION_SUBTYPE_DECLARATION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declaration
        $$=node;
    }
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_FUNCTION_DEFINITION;
        node.node_sub_type = NODE_TYPE_FUNCTION_DEFINITION_SUBTYPE_DECLARATION_SPECIFIERS_DECLARATOR_DECLARATION_LIST_COMPOUND_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declaration_specifiers
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //declaration_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($4))); //compound_statement
        $$=node;
    }
	| declaration_specifiers declarator compound_statement
	{
        ast_node node;
        node.node_type = NODE_TYPE_FUNCTION_DEFINITION;
        node.node_sub_type = NODE_TYPE_FUNCTION_DEFINITION_SUBTYPE_DECLARATION_SPECIFIERS_DECLARATOR_COMPOUND_STATEMENT;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declaration_specifiers
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //declarator
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($3))); //compound_statement
        $$=node;
    }
	;

declaration_list
	: declaration
	{
        ast_node node;
        node.node_type = NODE_TYPE_DECLARATION_LIST;
        node.node_sub_type = NODE_TYPE_DECLARATION_LIST_SUBTYPE_DECLARATION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declaration
        $$=node;
    }
	| declaration_list declaration
	{
        ast_node node;
        node.node_type = NODE_TYPE_DECLARATION_LIST;
        node.node_sub_type = NODE_TYPE_DECLARATION_LIST_SUBTYPE_DECLARATION_LIST_DECLARATION;
        node.items.push_back(std::shared_ptr<ast_node>(new ast_node($1))); //declaration_list
		node.items.push_back(std::shared_ptr<ast_node>(new ast_node($2))); //declaration
        $$=node;
    }
	;

%%
