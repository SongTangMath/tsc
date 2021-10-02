
%{
#include <stdio.h>
#include "parser.h"
#include "tsc.tab.hpp"
#define YYERROR_VERBOSE 1  
int yylex ();
extern char yytext[];
void yyerror(const char *s)
{
	printf("%s\n", s);
}  
%}


%token	IDENTIFIER I_CONSTANT F_CONSTANT STRING_LITERAL FUNC_NAME SIZEOF
%token	PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token	AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token	SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
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
	| constant
	| string
	| LEFT_PARENTHESIS expression RIGHT_PARENTHESIS
	| generic_selection
	;

constant
	: I_CONSTANT		/* includes character_constant */
	| F_CONSTANT
	| ENUMERATION_CONSTANT	/* after it has been defined as such */
	;

enumeration_constant		/* before it has been defined as such */
	: IDENTIFIER
	;

string
	: STRING_LITERAL
	| FUNC_NAME
	;

generic_selection
	: GENERIC LEFT_PARENTHESIS assignment_expression COMMA generic_assoc_list RIGHT_PARENTHESIS
	;

generic_assoc_list
	: generic_association
	| generic_assoc_list COMMA generic_association
	;

generic_association
	: type_name COLON assignment_expression
	| DEFAULT COLON assignment_expression
	;

postfix_expression
	: primary_expression
	| postfix_expression LEFT_BRACKET expression RIGHT_BRACKET
	| postfix_expression LEFT_PARENTHESIS RIGHT_PARENTHESIS
	| postfix_expression LEFT_PARENTHESIS argument_expression_list RIGHT_PARENTHESIS
	| postfix_expression DOT IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP
	| postfix_expression DEC_OP
	| LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS LEFT_BRACE initializer_list RIGHT_BRACE
	| LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS LEFT_BRACE initializer_list COMMA RIGHT_BRACE
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list COMMA assignment_expression
	;

unary_expression
	: postfix_expression
	| INC_OP unary_expression
	| DEC_OP unary_expression
	| unary_operator cast_expression
	| SIZEOF unary_expression
	| SIZEOF LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS
	| ALIGNOF LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS
	;

unary_operator
	: BITAND
	| MUL
	| ADD
	| SUB
	| BITNOT
	| OPERATOR_NOT
	;

cast_expression
	: unary_expression
	| LEFT_PARENTHESIS type_name RIGHT_PARENTHESIS cast_expression
	;

multiplicative_expression
	: cast_expression
	| multiplicative_expression MUL cast_expression
	| multiplicative_expression DIV cast_expression
	| multiplicative_expression MOD cast_expression
	;

additive_expression
	: multiplicative_expression
	| additive_expression ADD multiplicative_expression
	| additive_expression SUB multiplicative_expression
	;

shift_expression
	: additive_expression
	| shift_expression LEFT_OP additive_expression
	| shift_expression RIGHT_OP additive_expression
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression
	| relational_expression '>' shift_expression
	| relational_expression LE_OP shift_expression
	| relational_expression GE_OP shift_expression
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression
	| equality_expression NE_OP relational_expression
	;

and_expression
	: equality_expression
	| and_expression BITAND equality_expression
	;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression BITXOR and_expression
	;

inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression BITOR exclusive_or_expression
	;

logical_and_expression
	: inclusive_or_expression
	| logical_and_expression AND_OP inclusive_or_expression
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression OR_OP logical_and_expression
	;

conditional_expression
	: logical_or_expression
	| logical_or_expression QUESTION expression COLON conditional_expression
	;

assignment_expression
	: conditional_expression
	| unary_expression assignment_operator assignment_expression
	;

assignment_operator
	: ASSIGN
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
	| AND_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
	;

expression
	: assignment_expression
	| expression COMMA assignment_expression
	;

constant_expression
	: conditional_expression	/* with constraints */
	;

declaration
	: declaration_specifiers SEMI_COLON
	| declaration_specifiers init_declarator_list SEMI_COLON
	| static_assert_declaration
	;

declaration_specifiers
	: storage_class_specifier declaration_specifiers
	| storage_class_specifier
	| type_specifier declaration_specifiers
	| type_specifier
	| type_qualifier declaration_specifiers
	| type_qualifier
	| function_specifier declaration_specifiers
	| function_specifier
	| alignment_specifier declaration_specifiers
	| alignment_specifier
	;

init_declarator_list
	: init_declarator
	| init_declarator_list COMMA init_declarator
	;

init_declarator
	: declarator ASSIGN initializer
	| declarator
	;

storage_class_specifier
	: TYPEDEF	/* identifiers must be flagged as TYPEDEF_NAME */
	| EXTERN
	| STATIC
	| THREAD_LOCAL
	| AUTO
	| REGISTER
	;

type_specifier
	: VOID
	| CHAR
	| SHORT
	| INT
	| LONG
	| FLOAT
	| DOUBLE
	| SIGNED
	| UNSIGNED
	| BOOL
	| COMPLEX
	| IMAGINARY	  	/* non-mandated extension */
	| atomic_type_specifier
	| struct_or_union_specifier
	| enum_specifier
	| TYPEDEF_NAME		/* after it has been defined as such */
	;

struct_or_union_specifier
	: struct_or_union LEFT_BRACE struct_declaration_list RIGHT_BRACE
	| struct_or_union IDENTIFIER LEFT_BRACE struct_declaration_list RIGHT_BRACE
	| struct_or_union IDENTIFIER
	;

struct_or_union
	: STRUCT
	| UNION
	;

struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;

struct_declaration
	: specifier_qualifier_list SEMI_COLON	/* for anonymous struct/union */
	| specifier_qualifier_list struct_declarator_list SEMI_COLON
	| static_assert_declaration
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	| type_qualifier specifier_qualifier_list
	| type_qualifier
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list COMMA struct_declarator
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
