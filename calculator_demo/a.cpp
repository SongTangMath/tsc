#include "a.h"
#include "lex.yy.h"
#include "tsc.tab.hpp"

std::shared_ptr<expression_node> root;

std::string expression_node::get_expression(){
  if(lexeme)return *lexeme;
  std::string expr;
  for(size_t i=0;i<items.size()-1;i++ ){
    expr+=items[i]->get_expression();
    expr+=" ";
  }
  expr+=items[items.size()-1]->get_expression();
  return expr;
  
}

void evaluation_value::print_value() {
  if (!is_valid) {
    printf("invalid value\n");
    return;
  }
  switch (type) {
  case EVALUATION_TYPE_INT:
    printf("%d\n", int_value);
    break;
  case EVALUATION_TYPE_DOUBLE:
    printf("%lf\n", double_value);
    break;

  default:
    break;
  }
}

// error: invalid operands of types 'double' and 'double' to binary 'operator%'
evaluation_value evaluate(std::string operator_token, evaluation_value left, evaluation_value right) {
  evaluation_value return_value;
  return_value.is_valid = true;
  if (!left.is_valid || !right.is_valid) {
    return_value.is_valid = false;
    return return_value;
  }
  // % 运算要求左右都是int
  if (left.type == EVALUATION_TYPE_INT && right.type == EVALUATION_TYPE_INT) {
    return_value.type = EVALUATION_TYPE_INT;
    return_value.int_value = evaluate(operator_token, left.int_value, right.int_value);
  } else if (left.type == EVALUATION_TYPE_INT && right.type == EVALUATION_TYPE_DOUBLE) {
    return_value.type = EVALUATION_TYPE_DOUBLE;

    if (operator_token == "%") {
      printf("error: invalid operands of types 'int' and 'double' to binary 'operator%%'\n");
      return_value.is_valid = false;
    } else
      return_value.double_value = evaluate(operator_token, left.int_value, right.double_value);
  }

  else if (left.type == EVALUATION_TYPE_DOUBLE && right.type == EVALUATION_TYPE_INT) {
    return_value.type = EVALUATION_TYPE_DOUBLE;

    if (operator_token == "%") {
      printf("error: invalid operands of types 'double' and 'int' to binary 'operator%%'\n");
      return_value.is_valid = false;
    } else
      return_value.double_value = evaluate(operator_token, left.double_value, right.int_value);
  }

  else if (left.type == EVALUATION_TYPE_DOUBLE && right.type == EVALUATION_TYPE_DOUBLE) {
    return_value.type = EVALUATION_TYPE_DOUBLE;

    if (operator_token == "%") {
      printf("error: invalid operands of types 'double' and 'double' to binary 'operator%%'\n");
      return_value.is_valid = false;
    } else
      return_value.double_value = evaluate(operator_token, left.double_value, right.double_value);
  }
  return return_value;
}

int evaluate(std::string operator_token, int left, int right) {
  if (operator_token == "+")
    return left + right;
  else if (operator_token == "-")
    return left - right;
  else if (operator_token == "*")
    return left * right;
  else if (operator_token == "/")
    return left / right;
  else if (operator_token == "%")
    return left % right;
  else
    return 0;
}

double evaluate(std::string operator_token, int left, double right) {
  if (operator_token == "+")
    return left + right;
  else if (operator_token == "-")
    return left - right;
  else if (operator_token == "*")
    return left * right;
  else if (operator_token == "/")
    return left / right;
  else
    return 0.0;
}

double evaluate(std::string operator_token, double left, double right) {
  if (operator_token == "+")
    return left + right;
  else if (operator_token == "-")
    return left - right;
  else if (operator_token == "*")
    return left * right;
  else if (operator_token == "/")
    return left / right;
  else
    return 0.0;
}

double evaluate(std::string operator_token, double left, int right) {
  if (operator_token == "+")
    return left + right;
  else if (operator_token == "-")
    return left - right;
  else if (operator_token == "*")
    return left * right;
  else if (operator_token == "/")
    return left / right;
  else
    return 0.0;
}

evaluation_value evaluate(std::shared_ptr<expression_node> node) {
  evaluation_value return_value;
  return_value.is_valid = true;
  if (!node) {
    return_value.is_valid = false;
    return return_value;
  }

  switch (node->node_type) {
  case NODE_TYPE_EXPRESSION: {
    switch (node->node_sub_type) {
    case NODE_TYPE_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION:
      return evaluate(node->items[0]);
    case NODE_TYPE_EXPRESSION_SUBTYPE_EXPRESSION_ADDMINUS_ADDITIVE_EXPRESSION: {
      evaluation_value left = evaluate(node->items[0]);
      evaluation_value right = evaluate(node->items[2]);
      return_value = evaluate(node->items[1]->get_expression(), left, right);
      return return_value;
    }
    }
    break;
  }
  case NODE_TYPE_ADDITIVE_EXPRESSION: {
    switch (node->node_sub_type) {
    case NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION:
      return evaluate(node->items[0]);
    case NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION_ADDMINUS_MULTIPLICATIVE_EXPRESSION: {
      evaluation_value left = evaluate(node->items[0]);
      evaluation_value right = evaluate(node->items[2]);
      return_value = evaluate(node->items[1]->get_expression(), left, right);
      return return_value;
    }
    }
    break;
  }

  case NODE_TYPE_MULTIPLICATIVE_EXPRESSION: {
    switch (node->node_sub_type) {
    case NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_ICONSTANT: {
      return_value.type = EVALUATION_TYPE_INT;
      return_value.int_value = atoi(node->items[0]->get_expression().c_str());
      return return_value;
    }

    case NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_FCONSTANT: {
      return_value.type = EVALUATION_TYPE_DOUBLE;
      return_value.int_value = atof(node->items[0]->get_expression().c_str());
      return return_value;
    }
    case NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_QUOTED_EXPRESSION: {
      // 0,2位置分别是左右括号
      return_value = evaluate(node->items[1]);
      return return_value;
    }
    }
    break;
  }
  default:
    break;
  }

  return_value.is_valid = false;
  return return_value;
}
int main() {
  FILE *file = fopen("../a.txt", "r+");
  if(!file){
      printf("file not found\n");
      return 1;
  }
  yyset_in(file);
  yyset_out(stdout);
  yyparse();
  for(size_t i=0;i<root->items.size();i++)
  printf("%s\n",root->items[i]->get_expression().c_str());
  evaluation_value value = evaluate(root);
  value.print_value();
  fclose(file);
  return 0;
}