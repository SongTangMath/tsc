#include <algorithm>
#include "parser.h"

std::shared_ptr<ast_node> translation_unit;
std::string input_file_name;

std::shared_ptr<tsc_type> global_types::primitive_type_void=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_char=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_unsigned_char=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_short=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_unsigned_short=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_int=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_unsigned_int=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_long=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_unsigned_long=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_long_long=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_unsigned_long_long=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_float=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_double=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_long_double=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::composite_type_const_char_star=std::make_shared<tsc_type>();

std::shared_ptr<tsc_type> global_types::primitive_type_const_void=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_const_char=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_const_unsigned_char=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_const_short=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_const_unsigned_short=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_const_int=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_const_unsigned_int=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_const_long=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_const_unsigned_long=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_const_long_long=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_const_unsigned_long_long=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_const_float=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_const_double=std::make_shared<tsc_type>();
std::shared_ptr<tsc_type> global_types::primitive_type_const_long_double=std::make_shared<tsc_type>();


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
// \"abc\"  \"def\" -> abcdef
std::string extract_string(std::string input) {
  std::string ret;
  bool in_quote = false;
  bool find_back_slash = false;
  for (size_t i = 0; i < input.length(); i++) {
    char ch = input[i];
    switch (ch) {
    case '\\':
      if (!in_quote)
        break;
      if (find_back_slash)
        ret += '\\';
      else
        find_back_slash = true;
      break;
    case '"':
      if (find_back_slash)
        ret += '\\';
      else
        in_quote = !in_quote;
      break;

    case 't':
      if (find_back_slash) {
        ret += '\t';
        find_back_slash = false;
      } else {
        ret += 't';
      }
      break;
    case 'r':
      if (find_back_slash) {
        ret += '\r';
        find_back_slash = false;
      } else {
        ret += 'r';
      }
      break;
    case 'n':
      if (find_back_slash) {
        ret += '\n';
        find_back_slash = false;
      } else {
        ret += 'n';
      }
      break;
    default:
      if (!in_quote)
        break;
      ret += ch;
    }
  }
  return ret;
}

std::shared_ptr<tsc_type> construct_pointer_to( std::shared_ptr<tsc_type> type){
    std::shared_ptr<tsc_type> pointer=std::make_shared<tsc_type>();
    pointer->is_pointer=true;
    pointer->underlying_type=type;
    return pointer;
}

