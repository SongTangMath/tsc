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

        expression_node node;
        node.node_type=NODE_TYPE_EXPRESSION;
        node.node_sub_type=NODE_TYPE_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION;

        node.items.push_back(std::shared_ptr<expression_node>(new expression_node($1)));

        root=std::shared_ptr<expression_node>(new expression_node(node));
        $$=node;
        #ifdef TSC_DEBUG 
        printf("%d %d\n",node.node_type,node.node_sub_type); 
        #endif
    }
    |  expression Add_minus additive_expression {
        expression_node node;
        node.node_type=NODE_TYPE_EXPRESSION;
        node.node_sub_type=NODE_TYPE_EXPRESSION_SUBTYPE_EXPRESSION_ADDMINUS_ADDITIVE_EXPRESSION;

        node.items.push_back(std::shared_ptr<expression_node>(new expression_node($1)));
        node.items.push_back(std::shared_ptr<expression_node>(new expression_node($2)));
        node.items.push_back(std::shared_ptr<expression_node>(new expression_node($3)));
        $$=node;

        root=std::shared_ptr<expression_node>(new expression_node(node));
        #ifdef TSC_DEBUG 
        printf("%d %d\n",node.node_type,node.node_sub_type); 
        #endif 
    }
    ;

additive_expression:
    multiplicative_expression
    {
        expression_node node;
        node.node_type=NODE_TYPE_ADDITIVE_EXPRESSION;
        node.node_sub_type=NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION;

        node.items.push_back(std::shared_ptr<expression_node>(new expression_node($1)));
        $$=node;

        #ifdef TSC_DEBUG 
        printf("%d %d\n",node.node_type,node.node_sub_type); 
        #endif
    }
    | additive_expression Multiply_divide_mod multiplicative_expression
    {
        expression_node node;
        node.node_type=NODE_TYPE_ADDITIVE_EXPRESSION;
        node.node_sub_type=NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION_ADDMINUS_MULTIPLICATIVE_EXPRESSION;

        node.items.push_back(std::shared_ptr<expression_node>(new expression_node($1)));
        node.items.push_back(std::shared_ptr<expression_node>(new expression_node($2)));
        node.items.push_back(std::shared_ptr<expression_node>(new expression_node($3)));
        $$=node;

        #ifdef TSC_DEBUG 
        printf("%d %d\n",node.node_type,node.node_sub_type); 
        #endif
    }
    ;

multiplicative_expression:
    I_CONSTANT
    {
        expression_node node;
        node.node_type=NODE_TYPE_MULTIPLICATIVE_EXPRESSION;
        node.node_sub_type=NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_ICONSTANT;
        node.items.push_back(std::shared_ptr<expression_node>(new expression_node($1)));
        $$=node;
        #ifdef TSC_DEBUG 
        printf("%d %d\n",node.node_type,node.node_sub_type); 
        #endif
    }
    | F_CONSTANT
    {
        expression_node node;
        node.node_type=NODE_TYPE_MULTIPLICATIVE_EXPRESSION;
        node.node_sub_type=NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_FCONSTANT;
        node.items.push_back(std::shared_ptr<expression_node>(new expression_node($1)));
        #ifdef TSC_DEBUG 
        printf("%d %d\n",node.node_type,node.node_sub_type); 
        #endif
    }
    | LEFT_PARENTHESIS expression RIGHT_PARENTHESIS
    {
        expression_node node;
        node.node_type=NODE_TYPE_MULTIPLICATIVE_EXPRESSION;
        node.node_sub_type=NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_QUOTED_EXPRESSION;
        node.items.push_back(std::shared_ptr<expression_node>(new expression_node($1)));
        node.items.push_back(std::shared_ptr<expression_node>(new expression_node($2)));
        node.items.push_back(std::shared_ptr<expression_node>(new expression_node($3)));

        $$=node;
          
        #ifdef TSC_DEBUG 
        printf("%d %d\n",node.node_type,node.node_sub_type); 
        #endif
    }
    ;


%%

