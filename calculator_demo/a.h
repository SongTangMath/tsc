#ifndef a_h
#define a_h


#include <string>
#include <memory>
#include <vector>
#define YYSTYPE expression_node

#define NODE_TYPE_EXPRESSION 1
#define NODE_TYPE_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION 1
#define NODE_TYPE_EXPRESSION_SUBTYPE_EXPRESSION_ADDMINUS_ADDITIVE_EXPRESSION 2

#define NODE_TYPE_ADDITIVE_EXPRESSION 2
#define NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION 1
#define NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION_ADDMINUS_MULTIPLICATIVE_EXPRESSION 2

#define NODE_TYPE_MULTIPLICATIVE_EXPRESSION 3
#define NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_ICONSTANT 1
#define NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_FCONSTANT 2
#define NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_QUOTED_EXPRESSION 3

#define EVALUATION_TYPE_INT 1
#define EVALUATION_TYPE_DOUBLE 2
int yyparse (void);
struct expression_node {
  int node_type;
  int node_sub_type;
  std::shared_ptr<std::string> lexeme;
  std::vector<std::shared_ptr<expression_node>> items;

  std::string get_expression();
};

struct evaluation_value {
  int type;
  bool is_valid;
  int int_value;
  double double_value;

  void print_value();
};

extern std::shared_ptr<expression_node> root;

int evaluate(std::string operator_token, int left, int right);
double evaluate(std::string operator_token, int left, double right);
double evaluate(std::string operator_token, double left, double right);
double evaluate(std::string operator_token, double left, int right);
evaluation_value evaluate(std::shared_ptr<expression_node> node);
#endif