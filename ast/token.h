#ifndef token_h
#define token_h

#include <string>
#include <vector>
#include <map>

#define TOKEN_TYPE_KEYWORD_AUTO 1      //auto
#define TOKEN_TYPE_KEYWORD_EXTERN 2    //extern
#define TOKEN_TYPE_KEYWORD_REGISTER 3  //register
#define TOKEN_TYPE_KEYWORD_STATIC 4    //static
#define TOKEN_TYPE_KEYWORD_TYPEDEF 5   //typedef
#define TOKEN_TYPE_KEYWORD_CONST 6     //const
#define TOKEN_TYPE_KEYWORD_VOLATILE 7  //volatile
#define TOKEN_TYPE_KEYWORD_SIGNED 8    //signed
#define TOKEN_TYPE_KEYWORD_UNSIGNED 9  //unsigned
#define TOKEN_TYPE_KEYWORD_SHORT 10    //short
#define TOKEN_TYPE_KEYWORD_LONG 11     //long
#define TOKEN_TYPE_KEYWORD_CHAR 12     //char
#define TOKEN_TYPE_KEYWORD_INT 13      //int
#define TOKEN_TYPE_KEYWORD_FLOAT 14    //float
#define TOKEN_TYPE_KEYWORD_DOUBLE 15   //double
#define TOKEN_TYPE_KEYWORD_ENUM 16     //enum
#define TOKEN_TYPE_KEYWORD_STRUCT 17   //struct
#define TOKEN_TYPE_KEYWORD_UNION 18    //union
#define TOKEN_TYPE_KEYWORD_VOID 19     //void
#define TOKEN_TYPE_KEYWORD_BREAK 20    //break
#define TOKEN_TYPE_KEYWORD_CASE 21     //case
#define TOKEN_TYPE_KEYWORD_CONTINUE 22 //continue
#define TOKEN_TYPE_KEYWORD_DEAFULT 23  //default
#define TOKEN_TYPE_KEYWORD_DO 24       //do
#define TOKEN_TYPE_KEYWORD_ELSE 25     //else
#define TOKEN_TYPE_KEYWORD_FOR 26      //for
#define TOKEN_TYPE_KEYWORD_GOTO 27     //goto
#define TOKEN_TYPE_KEYWORD_IF 28       //if
#define TOKEN_TYPE_KEYWORD_RETURN 29   //return
#define TOKEN_TYPE_KEYWORD_SWITCH 30   //switch
#define TOKEN_TYPE_KEYWORD_WHILE 31    //while
#define TOKEN_TYPE_KEYWORD_SIZEOF 32   //sizeof

#define TOKEN_TYPE_IDENTIFIER 33 //示例:int a;中的a int foo(int,int);中的foo

#define TOKEN_TYPE_CONSTANT_INT 34                //int型字面量如int a=123;中的123 char或者wchar_t的字面量视为int wchar_t w=L'abc';在gcc中会给一个warning
#define TOKEN_TYPE_CONSTANT_UNSIGNED_INT 35       //后缀为u,U unsigned int a=123u;
#define TOKEN_TYPE_CONSTANT_LONG 36               //后缀为l,L long a=123L;
#define TOKEN_TYPE_CONSTANT_UNSIGNED_LONG 37      //后缀为ul,UL等4种组合 unsigned long a=123UL;
#define TOKEN_TYPE_CONSTANT_LONG_LONG 38          //后缀为ll,LL long long a=123LL;
#define TOKEN_TYPE_CONSTANT_UNSIGNED_LONG_LONG 39 //后缀为ull,ULL long a=123L;
#define TOKEN_TYPE_CONSTANT_FLOAT 40              //f,F float a=0.123f;
#define TOKEN_TYPE_CONSTANT_DOUBLE 41             //double a=0.123; double b=1.2e7;
#define TOKEN_TYPE_CONSTANT_LONG_DOUBLE 42 //一个long double至少与double有相同精度.在x86架构上通常是80bit扩展精度
#define TOKEN_TYPE_CONSTANT_STRING 43      //const chat* str="hello"; 中的"hello"
#define TOKEN_TYPE_CONSTANT_WIDE_STRING 44 //const wchar_t* str=L"hello"; 中的L"hello"

#define TOKEN_TYPE_OPERATOR_COMMA 45                 // ","
#define TOKEN_TYPE_OPERATOR_QUESTION 46              // "?"
#define TOKEN_TYPE_OPERATOR_COLON 47                 // ":"
#define TOKEN_TYPE_OPERATOR_ASSIGN 48                // "="
#define TOKEN_TYPE_OPERATOR_BITOR_ASSIGN 49          // "|="
#define TOKEN_TYPE_OPERATOR_BITXOR_ASSIGN 50         // "^="
#define TOKEN_TYPE_OPERATOR_BITAND_ASSIGN 51         // "&="
#define TOKEN_TYPE_OPERATOR_LEFT_SHIFT_ASSIGN 52     // "<<="
#define TOKEN_TYPE_OPERATOR_RIGHT_SHIFT_ASSIGN 53    // ">>="
#define TOKEN_TYPE_OPERATOR_ADD_ASSIGN 54            // "+="
#define TOKEN_TYPE_OPERATOR_SUB_ASSIGN 55            // "-="
#define TOKEN_TYPE_OPERATOR_MUL_ASSIGN 56            // "*="
#define TOKEN_TYPE_OPERATOR_DIV_ASSIGN 57            // "/="
#define TOKEN_TYPE_OPERATOR_MOD_ASSIGN 58            // "%="
#define TOKEN_TYPE_OPERATOR_OR 59                    // "||"
#define TOKEN_TYPE_OPERATOR_AND 60                   // "&&"
#define TOKEN_TYPE_OPERATOR_BITOR 61                 // "|"
#define TOKEN_TYPE_OPERATOR_BITAND 62                // "&"
#define TOKEN_TYPE_OPERATOR_EQUAL 63                 // "=="
#define TOKEN_TYPE_OPERATOR_UNEQUAL 64               // "!="
#define TOKEN_TYPE_OPERATOR_GREATER_THAN 65          // ">"
#define TOKEN_TYPE_OPERATOR_LESS_THAN 66             // "<"
#define TOKEN_TYPE_OPERATOR_GREATER_THAN_OR_EQUAL 67 // ">="
#define TOKEN_TYPE_OPERATOR_LESS_THAN_OR_EQUAL 68    // "<="
#define TOKEN_TYPE_OPERATOR_LEFT_SHIFT 69            // "<<"
#define TOKEN_TYPE_OPERATOR_RIGHT_SHIFT 70           // ">>"
#define TOKEN_TYPE_OPERATOR_ADD 71                   // "+"
#define TOKEN_TYPE_OPERATOR_SUB 72                   // "-"
#define TOKEN_TYPE_OPERATOR_MUL 73                   // "*" 也用于指针变量取值
#define TOKEN_TYPE_OPERATOR_DIV 74                   // "/"
#define TOKEN_TYPE_OPERATOR_MOD 75                   // "%"
#define TOKEN_TYPE_OPERATOR_INC 76                   // "++"
#define TOKEN_TYPE_OPERATOR_DEC 77                   // "--"
#define TOKEN_TYPE_OPERATOR_NOT 78                   // "!"
#define TOKEN_TYPE_OPERATOR_BIT_NOT 79               // "~"
#define TOKEN_TYPE_OPERATOR_DOT 80                   // "."
#define TOKEN_TYPE_OPERATOR_POINTER 81               // "->"

#define TOKEN_TYPE_PUNCTUATOR_LEFT_PARENTHESIS 82  // "("
#define TOKEN_TYPE_PUNCTUATOR_RIGHT_PARENTHESIS 83 // ")"
#define TOKEN_TYPE_PUNCTUATOR_LEFT_BREACKET 84     // "["
#define TOKEN_TYPE_PUNCTUATOR_RIGHT_BREACKET 85    // "]"
#define TOKEN_TYPE_PUNCTUATOR_LEFT_BRACE 86        // "{"
#define TOKEN_TYPE_PUNCTUATOR_RIGHT_BRACE 87       // "}"
#define TOKEN_TYPE_PUNCTUATOR_SEMI_COLON 88        // ";"
#define TOKEN_TYPE_PUNCTUATOR_ELLIPSIS 89          // "..."
#define TOKEN_TYPE_PUNCTUATOR_POUND 90             // "#"
#define TOKEN_TYPE_PUNCTUATOR_NEWLINE 91           // "\r,\n,\r\n"

#define TOKEN_TYPE_EOF 94 // "eof"

struct ast_token {
  int token_type_id;                 // TOKEN_TYPE_KEYWORD_INT
  std::string token_type_identifier; // "int"
  std::string value;                 // "int", "variable_name"
};

struct ast_identifier_token: ast_token{
    
};

struct ast_wstring_token: ast_token{
    std::wstring wstring_value;
};

struct ast_integer_value_token: ast_token{
    union{
        long long_value;
        unsigned long ulong_value;
    }int_value;
    bool overflow;
    
};

struct ast_error {
  int line_number;
  int column_number;
  std::string message;
  ast_error() = default;
  ast_error(int line_number, int column_number, std::string message)
      : line_number(line_number), column_number(column_number), message(message) {}
};

struct ast_parse_context {
  const char *file_buffer;
  int file_length;
  int line_number;
  int column_number;
  int parsed_char_count;
  std::vector<ast_error> errors;
  std::vector<ast_error> warnings;
};

extern std::vector<std::string> keywords;
extern std::map<std::string, int> keyword_to_token_type_id;
std::string get_string_literal(ast_parse_context &parse_context, char expected_terminate_char);
std::shared_ptr<ast_token> get_next_token(ast_parse_context &parse_context);

#endif // #ifndef token_h
