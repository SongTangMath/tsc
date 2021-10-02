#include <algorithm>
#include "parser.h"

std::shared_ptr<ast_node> translation_unit;
std::string input_file_name;

std::shared_ptr<tsc_type> global_types::primitive_type_void;
std::shared_ptr<tsc_type> global_types::primitive_type_char;
std::shared_ptr<tsc_type> global_types::primitive_type_unsigned_char;
std::shared_ptr<tsc_type> global_types::primitive_type_short;
std::shared_ptr<tsc_type> global_types::primitive_type_unsigned_short;
std::shared_ptr<tsc_type> global_types::primitive_type_int;
std::shared_ptr<tsc_type> global_types::primitive_type_unsigned_int;
std::shared_ptr<tsc_type> global_types::primitive_type_long;
std::shared_ptr<tsc_type> global_types::primitive_type_unsigned_long;
std::shared_ptr<tsc_type> global_types::primitive_type_long_long;
std::shared_ptr<tsc_type> global_types::primitive_type_unsigned_long_long;
std::shared_ptr<tsc_type> global_types::primitive_type_float;
std::shared_ptr<tsc_type> global_types::primitive_type_double;
std::shared_ptr<tsc_type> global_types::primitive_type_long_double;

std::shared_ptr<tsc_type> global_types::primitive_type_const_void;
std::shared_ptr<tsc_type> global_types::primitive_type_const_char;
std::shared_ptr<tsc_type> global_types::primitive_type_const_unsigned_char;
std::shared_ptr<tsc_type> global_types::primitive_type_const_short;
std::shared_ptr<tsc_type> global_types::primitive_type_const_unsigned_short;
std::shared_ptr<tsc_type> global_types::primitive_type_const_int;
std::shared_ptr<tsc_type> global_types::primitive_type_const_unsigned_int;
std::shared_ptr<tsc_type> global_types::primitive_type_const_long;
std::shared_ptr<tsc_type> global_types::primitive_type_const_unsigned_long;
std::shared_ptr<tsc_type> global_types::primitive_type_const_long_long;
std::shared_ptr<tsc_type> global_types::primitive_type_const_unsigned_long_long;
std::shared_ptr<tsc_type> global_types::primitive_type_const_float;
std::shared_ptr<tsc_type> global_types::primitive_type_const_double;
std::shared_ptr<tsc_type> global_types::primitive_type_const_long_double;

std::shared_ptr<tsc_type> global_types::primitive_type_sizeof;
std::shared_ptr<tsc_type> global_types::primitive_type_ptrdiff_t;

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

// \"abc\"  \"def\" -> abcdef "abc\1234cd"->"abcS4cd"
// "abc\94cd"; -> warning: unknown escape sequence: '\9' -> "abc94cd"
// todo 实现数字转义字符
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

            case 'a':
            case 'b':
            case 'f':
            case 'v':
            case 't':
            case 'r':
            case 'n':
                if (find_back_slash) {
                    ret += escape_char(ch);
                    find_back_slash = false;
                } else {
                    ret += ch;
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

bool is_unsigned_suffix(const std::string &suffix) {
    return suffix.find('u') != std::string::npos || suffix.find('U') != std::string::npos;
}

bool is_long_suffix(const std::string &suffix) {
    return (suffix.find('l') != std::string::npos || suffix.find('L') != std::string::npos) &&
           (suffix.find("ll") == std::string::npos || suffix.find("LL") == std::string::npos);
}

bool is_long_long_suffix(const std::string &suffix) {
    return suffix.find("ll") != std::string::npos || suffix.find("LL") != std::string::npos;
}

bool is_long_double_suffix(const std::string &suffix) { return suffix == "l" || suffix == "L"; }

bool is_float_suffix(const std::string &suffix) { return suffix == "f" || suffix == "F"; }

char escape_char(char ch) {
    switch (ch) {
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 'f':
            return '\f';
        case 'v':
            return '\v';
        case 't':
            return '\t';
        case 'r':
            return '\r';
        case 'n':
            return '\n';
        default:
            return ch;
    }
}