#include "token.h"
#include <vector>
#include <map>
#include <memory>
#include <ctype.h>

std::vector<std::string> keywords = {"auto",     "extern",   "register", "static", "typedef", "const", "volatile",
                                     "signed",   "unsigned", "short",    "long",   "char",    "int",   "float",
                                     "double",   "enum",     "struct",   "union",  "void",    "break", "case",
                                     "continue", "default",  "do",       "else",   "for",     "goto",  "if",
                                     "return",   "switch",   "while",    "sizeof"};

std::map<std::string, int> keyword_to_token_type_id = {

    {"auto", TOKEN_TYPE_KEYWORD_AUTO},         {"extern", TOKEN_TYPE_KEYWORD_EXTERN},
    {"register", TOKEN_TYPE_KEYWORD_REGISTER}, {"static", TOKEN_TYPE_KEYWORD_STATIC},
    {"typedef", TOKEN_TYPE_KEYWORD_TYPEDEF},   {"const", TOKEN_TYPE_KEYWORD_CONST},
    {"volatile", TOKEN_TYPE_KEYWORD_VOLATILE}, {"signed", TOKEN_TYPE_KEYWORD_SIGNED},
    {"unsigned", TOKEN_TYPE_KEYWORD_UNSIGNED}, {"short", TOKEN_TYPE_KEYWORD_SHORT},
    {"long", TOKEN_TYPE_KEYWORD_LONG},         {"char", TOKEN_TYPE_KEYWORD_CHAR},
    {"int", TOKEN_TYPE_KEYWORD_INT},           {"float", TOKEN_TYPE_KEYWORD_FLOAT},
    {"double", TOKEN_TYPE_KEYWORD_DOUBLE},     {"enum", TOKEN_TYPE_KEYWORD_ENUM},
    {"struct", TOKEN_TYPE_KEYWORD_STRUCT},     {"union", TOKEN_TYPE_KEYWORD_UNION},
    {"void", TOKEN_TYPE_KEYWORD_VOID},         {"break", TOKEN_TYPE_KEYWORD_BREAK},
    {"case", TOKEN_TYPE_KEYWORD_CASE},         {"continue", TOKEN_TYPE_KEYWORD_CONTINUE},
    {"default", TOKEN_TYPE_KEYWORD_DEAFULT},   {"do", TOKEN_TYPE_KEYWORD_DO},
    {"else", TOKEN_TYPE_KEYWORD_ELSE},         {"for", TOKEN_TYPE_KEYWORD_FOR},
    {"goto", TOKEN_TYPE_KEYWORD_GOTO},         {"if", TOKEN_TYPE_KEYWORD_IF},
    {"return", TOKEN_TYPE_KEYWORD_RETURN},     {"switch", TOKEN_TYPE_KEYWORD_SWITCH},
    {"while", TOKEN_TYPE_KEYWORD_WHILE},       {"sizeof", TOKEN_TYPE_KEYWORD_SIZEOF}};

//expected_terminate_char='\'' or '"'
std::string get_string_literal(ast_parse_context &parse_context, char expected_terminate_char) {

  std::string token_value;
  int original_parsed_char_count = parse_context.parsed_char_count;
  while (parse_context.parsed_char_count + 1 < parse_context.file_length &&
         parse_context.file_buffer[parse_context.parsed_char_count + 1] != expected_terminate_char) {
    //还有下一个字符.先读进来(parsed_char_count++)再判断是什么字符
    parse_context.parsed_char_count++;
    //事实上ascii转义应该支持数字表示的转义如'\123'(1-3位的8进制数字),'\xABC'(16进制数字),'\u0012'(必须是4位的16进制数字)
    if (parse_context.file_buffer[parse_context.parsed_char_count] == '\\') {

      //如果转义符是文件的最后一个字符,error
      if (parse_context.parsed_char_count >= parse_context.file_length) {
        parse_context.errors.push_back(
            ast_error(parse_context.line_number, parse_context.column_number, "unexpected EOF"));
        return token_value;
      }
      parse_context.parsed_char_count++;
      switch (parse_context.file_buffer[parse_context.parsed_char_count]) {
      case 'a':
        token_value += '\a';
        break;

      case 'b':
        token_value += '\b';
        break;

      case 'v':
        token_value += '\v';
        break;

      case 't':
        token_value += '\t';
        break;

      case 'r':
        token_value += '\r';
        break;

      case 'n':
        token_value += '\n';
        break;

      case '\r':
        if (parse_context.parsed_char_count + 1 < parse_context.file_length &&
            parse_context.file_buffer[parse_context.parsed_char_count + 1] == '\n') {
          parse_context.parsed_char_count++;
        }
        parse_context.line_number++;
        parse_context.column_number = 0;
        break;
      case '\n':
        parse_context.line_number++;
        parse_context.column_number = 0;
        break;
      }
    }

    else if (parse_context.file_buffer[parse_context.parsed_char_count] == '\r' ||
             parse_context.file_buffer[parse_context.parsed_char_count] == '\n') {
      parse_context.column_number += parse_context.parsed_char_count - original_parsed_char_count;
      parse_context.errors.push_back(
          ast_error(parse_context.line_number, parse_context.column_number, "unexpected new line"));
      //正常解析(容错处理)
      return token_value;
    }
  }

  //搜索到文件末尾都没有匹配的'\'' error
  if (parse_context.parsed_char_count == parse_context.file_length) {
    parse_context.errors.push_back(ast_error(parse_context.line_number, parse_context.column_number,
                                             "unexpected EOF, expecting a correspongding' or \" "));
  }
  return token_value;
}

std::shared_ptr<ast_token> get_next_token(ast_parse_context &parse_context) {
  //可能找到一个token
  std::shared_ptr<ast_token> optional_token;
  //处理所有的空白字符
  while (parse_context.parsed_char_count < parse_context.file_length) {
    char next_char = parse_context.file_buffer[parse_context.parsed_char_count];
    if (next_char == ' ' || next_char == '\t') {
      parse_context.parsed_char_count++;
      parse_context.column_number++;
    }

    // darwin(mac)上\r换行,unix上\n换行windows\r\n换行
    else if (next_char == '\r') {
      parse_context.parsed_char_count++;
      parse_context.line_number++;
      parse_context.column_number = 0;
      //如果下一个字符是\n则也将它读入但是line_number,column_number不变
      if (parse_context.parsed_char_count + 1 < parse_context.file_length &&
          parse_context.file_buffer[parse_context.parsed_char_count + 1] == '\n') {
        parse_context.parsed_char_count++;
      }

    } else if (next_char == '\n') {
      parse_context.parsed_char_count++;
      parse_context.line_number++;
      parse_context.column_number = 0;
    }
    //读到了一个非空白字符
    else {
      break;
    }
  }

  char next_char = parse_context.file_buffer[parse_context.parsed_char_count];
  //identifier允许以_开头.以L开头还可能是宽字符or宽字符串
  if (isalpha(next_char) || next_char == '_') {
    if (next_char == 'L') {
      //可能是以L开头的宽字符或者宽字符串.注意宽字符串L与后面的\"中间不能有空格,所以可以直接判断下个字符
      if (parse_context.parsed_char_count + 1 == parse_context.file_length) {
        //文件的最后一个字符为L,非法(应该是';'或者'}'或者空白字符)
        parse_context.errors.push_back(
            ast_error(parse_context.line_number, parse_context.column_number, "unexpected EOF"));
      } else if (parse_context.file_buffer[parse_context.parsed_char_count + 1] == '\'') {
        //是一个宽字符.wchar_t ch=L'abc'会报一个warning
        //寻找匹配的'\''

        ast_integer_value_token *integer_value_token = new ast_integer_value_token;
        integer_value_token->token_type_id = TOKEN_TYPE_CONSTANT_INT;
        integer_value_token->value = get_string_literal(parse_context, '\'');
        if (integer_value_token->value.length() == 0) {
          parse_context.errors.push_back(
              ast_error(parse_context.line_number, parse_context.column_number, "empty char literal"));
        }
        optional_token.reset(integer_value_token);
        return optional_token;
      }

      else if (parse_context.file_buffer[parse_context.parsed_char_count + 1] == '"') {
        //是一个宽字符串.wchar_t* wstr=L"abc"
        //寻找匹配的'"'

        ast_wstring_token *wstring_value_token = new ast_wstring_token;
        wstring_value_token->token_type_id = TOKEN_TYPE_CONSTANT_INT;
        wstring_value_token->value = get_string_literal(parse_context, '"');

        optional_token.reset(wstring_value_token);
        return optional_token;
      }
    }
    //不是以L开头,是identifier.可能是关键字
    else {
      std::string identifier;
      int original_parsed_char_count = parse_context.parsed_char_count;
      while (parse_context.parsed_char_count < parse_context.file_length) {
        while (isalpha(parse_context.file_buffer[parse_context.parsed_char_count]) ||
               parse_context.file_buffer[parse_context.parsed_char_count] == '_') {
          identifier += parse_context.file_buffer[parse_context.parsed_char_count];
          parse_context.parsed_char_count++;
        }

        //identifier结束.空白字符不作处理,等待下一次读取token时处理.正常来说identifier后面不应该是换行符.
        parse_context.column_number += parse_context.parsed_char_count - original_parsed_char_count;
        ast_identifier_token *identifier_token = new ast_identifier_token;
        identifier_token->value = identifier;

        std::map<std::string, int>::iterator it = keyword_to_token_type_id.find(identifier);
        if (it != keyword_to_token_type_id.end()) {
          //找到了一个关键字
          identifier_token->token_type_id = it->second;
        } else {
          identifier_token->token_type_id = TOKEN_TYPE_IDENTIFIER;
        }

        optional_token.reset(identifier_token);
        return optional_token;
      }
    }
  } else {
    //不是字母或者下划线
    return optional_token;
  }

  return optional_token;
}
