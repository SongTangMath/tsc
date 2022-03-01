#include <algorithm>
#include "parser.h"
#include "lex.yy.h"

std::shared_ptr<ast_node> translation_unit;

std::string ast_node::get_expression() {
  if (lexeme)
    return *lexeme;
  std::string expr;
  for (size_t i = 0; i < items.size() - 1; i++) {
    expr += items[i]->get_expression();
    expr += " ";
  }
  expr += items[items.size() - 1]->get_expression();
  return expr;
}

int ast_node::get_first_terminal_line_no() {
  if (lexeme)
    return line_no;

  return items[0]->get_first_terminal_line_no();
}
