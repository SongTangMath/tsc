%{
#include <stdio.h>
#include "a.h"
#include "tsc.tab.hpp"
#define YYERROR_VERBOSE 1  
int yylex ();
extern char yytext[];
void yyerror(const char *s)
{
	printf("%s\n", s);
}  
%}

%token I_CONSTANT F_CONSTANT Add_minus Multiply_divide_mod
%token LEFT_PARENTHESIS RIGHT_PARENTHESIS
%start expression

%%




expression:
    additive_expression {
        $$.node_type=NODE_TYPE_EXPRESSION;
        $$.node_sub_type=NODE_TYPE_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION;

        $$.left=std::shared_ptr<expression_node>(new expression_node($1));
        $$.expression=$1.expression;
        root=std::shared_ptr<expression_node>(new expression_node($$));
        #ifdef TSC_DEBUG 
        printf("%d %d\n",$$.node_type,$$.node_sub_type); 
        #endif
    }
    |  expression Add_minus additive_expression {
        $$.node_type=NODE_TYPE_EXPRESSION;
        $$.node_sub_type=NODE_TYPE_EXPRESSION_SUBTYPE_EXPRESSION_ADDMINUS_ADDITIVE_EXPRESSION;

        $$.left=std::shared_ptr<expression_node>(new expression_node($1));
        $$.operator_token=std::shared_ptr<expression_node>(new expression_node($2));
        $$.right=std::shared_ptr<expression_node>(new expression_node($3));

        $$.expression=$1.expression+$2.expression+$3.expression;
        
        root=std::shared_ptr<expression_node>(new expression_node($$));
        #ifdef TSC_DEBUG 
        printf("%d %d\n",$$.node_type,$$.node_sub_type); 
        #endif 
    }
    ;

additive_expression:
    multiplicative_expression
    {
        $$.node_type=NODE_TYPE_ADDITIVE_EXPRESSION;
        $$.node_sub_type=NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION;

        $$.left=std::shared_ptr<expression_node>(new expression_node($1));  
        $$.expression=$1.expression;      
        #ifdef TSC_DEBUG 
        printf("%d %d\n",$$.node_type,$$.node_sub_type); 
        #endif
    }
    | additive_expression Multiply_divide_mod multiplicative_expression
    {
        $$.node_type=NODE_TYPE_ADDITIVE_EXPRESSION;
        $$.node_sub_type=NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION_ADDMINUS_MULTIPLICATIVE_EXPRESSION;

        $$.left=std::shared_ptr<expression_node>(new expression_node($1));  
        $$.operator_token=std::shared_ptr<expression_node>(new expression_node($2));
        $$.right=std::shared_ptr<expression_node>(new expression_node($3));   

        $$.expression=$1.expression+$2.expression+$3.expression;  
        #ifdef TSC_DEBUG 
        printf("%d %d\n",$$.node_type,$$.node_sub_type); 
        #endif
    }
    ;

multiplicative_expression:
    I_CONSTANT
    {
        $$.node_type=NODE_TYPE_MULTIPLICATIVE_EXPRESSION;
        $$.node_sub_type=NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_ICONSTANT;

        #ifdef TSC_DEBUG 
        printf("%d %d\n",$$.node_type,$$.node_sub_type); 
        #endif
    }
    | F_CONSTANT
    {
        $$.node_type=NODE_TYPE_MULTIPLICATIVE_EXPRESSION;
        $$.node_sub_type=NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_FCONSTANT;
        #ifdef TSC_DEBUG 
        printf("%d %d\n",$$.node_type,$$.node_sub_type); 
        #endif
    }
    | LEFT_PARENTHESIS expression RIGHT_PARENTHESIS
    {
        $$.node_type=NODE_TYPE_MULTIPLICATIVE_EXPRESSION;
        $$.node_sub_type=NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_QUOTED_EXPRESSION;
        $$.left=std::shared_ptr<expression_node>(new expression_node($1));  
        $$.expression=$1.expression+$2.expression+$3.expression; 
        
        #ifdef TSC_DEBUG 
        printf("%d %d\n",$$.node_type,$$.node_sub_type); 
        #endif
    }
    ;


%%

