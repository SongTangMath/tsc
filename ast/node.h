#ifndef node_h
#define node_h

#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

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

#define TOKEN_TYPE_CONSTANT_INT 34
// int型字面量如int a=123;中的123 char或者wchar_t的字面量视为int
// wchar_t w=L'abc';在gcc中会给一个warning
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
#define TOKEN_TYPE_OPERATOR_BITXOR 62                // "^"
#define TOKEN_TYPE_OPERATOR_BITAND 63                // "&"
#define TOKEN_TYPE_OPERATOR_EQUAL 64                 // "=="
#define TOKEN_TYPE_OPERATOR_UNEQUAL 65               // "!="
#define TOKEN_TYPE_OPERATOR_GREATER_THAN 66          // ">"
#define TOKEN_TYPE_OPERATOR_LESS_THAN 67             // "<"
#define TOKEN_TYPE_OPERATOR_GREATER_THAN_OR_EQUAL 68 // ">="
#define TOKEN_TYPE_OPERATOR_LESS_THAN_OR_EQUAL 69    // "<="
#define TOKEN_TYPE_OPERATOR_LEFT_SHIFT 70            // "<<"
#define TOKEN_TYPE_OPERATOR_RIGHT_SHIFT 71           // ">>"
#define TOKEN_TYPE_OPERATOR_ADD 72                   // "+"
#define TOKEN_TYPE_OPERATOR_SUB 73                   // "-"
#define TOKEN_TYPE_OPERATOR_MUL 74                   // "*" 也用于指针变量取值
#define TOKEN_TYPE_OPERATOR_DIV 75                   // "/"
#define TOKEN_TYPE_OPERATOR_MOD 76                   // "%"
#define TOKEN_TYPE_OPERATOR_INC 77                   // "++"
#define TOKEN_TYPE_OPERATOR_DEC 78                   // "--"
#define TOKEN_TYPE_OPERATOR_NOT 79                   // "!"
#define TOKEN_TYPE_OPERATOR_BIT_NOT 80               // "~"
#define TOKEN_TYPE_OPERATOR_DOT 81                   // "."
#define TOKEN_TYPE_OPERATOR_POINTER 82               // "->"

#define TOKEN_TYPE_PUNCTUATOR_LEFT_PARENTHESIS 83  // "("
#define TOKEN_TYPE_PUNCTUATOR_RIGHT_PARENTHESIS 84 // ")"
#define TOKEN_TYPE_PUNCTUATOR_LEFT_BRACKET 85      // "["
#define TOKEN_TYPE_PUNCTUATOR_RIGHT_BRACKET 86     // "]"
#define TOKEN_TYPE_PUNCTUATOR_LEFT_BRACE 87        // "{"
#define TOKEN_TYPE_PUNCTUATOR_RIGHT_BRACE 88       // "}"
#define TOKEN_TYPE_PUNCTUATOR_SEMI_COLON 89        // ";"
#define TOKEN_TYPE_PUNCTUATOR_ELLIPSIS 90          // "..."
#define TOKEN_TYPE_PUNCTUATOR_POUND 91             // "#"
#define TOKEN_TYPE_PUNCTUATOR_NEWLINE 92           // "\r,\n,\r\n"

#define TOKEN_TYPE_EOF 94 // "eof"

struct ast_token {
    int token_type_id; // TOKEN_TYPE_KEYWORD_INT
    std::string value; // "int", "variable_name"
};

struct ast_identifier_token : ast_token {
};

struct ast_wstring_token : ast_token {
    std::wstring wstring_value;
};

struct ast_integer_value_token : ast_token {
    union {
        long long_value;
        unsigned long ulong_value;
    } number_value;
    bool overflow;
};

struct ast_double_literal_token : ast_token {

    double double_value;
    int value_type;
    bool overflow;
};

struct ast_error {
    int line_number;
    int column_number;
    std::string message;

    ast_error() = default;

    ast_error(int line_number, int column_number, std::string message);
};

#define NODE_TYPE_LEAF 0
#define NODE_TYPE_TRANSLATION_UNIT 1
#define NODE_TYPE_EXTERNAL_DECLARATION 2
#define NODE_TYPE_DECLARATION 3
#define NODE_TYPE_FUNCTION_DEFINITION 4
#define NODE_TYPE_DECLARATION_SPECIFIER 5
#define NODE_TYPE_INIT_DECLARATOR 6
#define NODE_TYPE_STRUCT_OR_UNION_SPECIFIER 6
#define NODE_TYPE_ENUM_SPECIFIER 6

struct ast_translation_unit;
struct ast_external_declaration;
struct ast_declaration;
struct ast_function_definition;
struct ast_declaration_specifier;

struct ast_init_declarator;
struct ast_struct_or_union_specifier;
struct ast_struct_declaration;
struct ast_struct_declarator;
struct ast_declarator;

struct ast_constant_expression;
struct ast_initializer;
struct ast_pointer;
struct ast_direct_declarator;
struct ast_parameter_type_list;

struct ast_parameter_declaration;
struct ast_abstract_declarator;
struct ast_direct_abstract_declarator;
struct ast_enum_specifier;
struct ast_enumerator;

struct ast_conditional_expression;
struct ast_logical_or_expression;
struct ast_expression;
struct ast_logical_and_expression;
struct ast_inclusive_or_expression;

struct ast_exclusive_or_expression;
struct ast_and_expression;
struct ast_equality_expression;
struct ast_relational_expression;
struct ast_shift_expression;

struct ast_additive_expression;
struct ast_multiplicative_expression;
struct ast_cast_expression;
struct ast_unary_expression;
struct ast_postfix_expression;

struct ast_primary_expression;
struct ast_assignment_expression;
struct ast_type_name;
struct ast_initializer_list;
struct ast_designation;

struct ast_designator;
struct ast_optional_designation_and_initializer;
struct ast_identifier_list;

//语法树节点
struct ast_node {
    int node_type_id;
    std::shared_ptr<ast_token> token;

    ast_node();

    ast_node(int node_type_id, std::shared_ptr<ast_token> token);
};

struct ast_typedef_name : ast_node {
    std::string name;
    int level;
    int overload;
    int overload_level;

    ast_typedef_name() = default;

    ast_typedef_name(int node_type_id, std::shared_ptr<ast_token> token, std::string name, int level);
};

struct ast_parse_context {
    const char *file_buffer;
    int file_length;
    int line_number;
    int column_number;
    int parsed_char_count;
    std::vector<ast_error> errors;
    std::vector<ast_error> warnings;
    bool unexpected_eof;

    std::vector<ast_typedef_name> typedef_names;
    //当前在第几层作用域
    int level;

    std::vector<std::shared_ptr<ast_token>> tokens;
    //已经读取到但是还没有解析的token
    std::shared_ptr<ast_token> current_token;
};

/*
translation_unit可以认为就是一个C源文件
translation_unit:
  external_declaration
  | translation_unit external_declaration

所以一个translation_unit就是一个external_declaration的vector

external_declaration:
  function_definition
  | declaration


init_declaratior_list:
    init_declarator
    | init_declaratior_list , init_declarator


init_declarator:
    declarator
    | declarator = initializer

*/

struct ast_translation_unit : ast_node {
    std::vector<std::shared_ptr<ast_external_declaration>> external_declarations;
};

struct ast_external_declaration : ast_node {
};

/*

declaration:
    declaration_specifier [init_declarator_list] ;

typedef语句也是declaration.
虽然我们一般称int a;为定义extern int a;为声明,在文法中它们都称为declaration
*/
struct ast_declaration : ast_external_declaration {
    std::shared_ptr<ast_declaration_specifier> declaration_specifier;
    std::vector<std::shared_ptr<ast_init_declarator>> init_declarators;
};

/*

declaration_specifier:
  storage_class_specifier [declaration_specifier]
  | type_specifer [declaration_specifier]
  | type_qualifier [declaration_specifier]
storage_class_specifier是typedef extern static auto register

type_specifer是void char short int long float double signed unsigned struct union enum typedef_name
struct-or-union-specifier 
enum-specifier
typedef-name

type_qualifier是const volatile

*/

struct ast_declaration_specifier : ast_node {
    std::vector<std::shared_ptr<ast_token>> storage_class_specifiers;
    std::vector<std::shared_ptr<ast_node>> type_specifers;
    std::vector<std::shared_ptr<ast_token>> type_qualifiers;
};

/*
struct-or-union-specifier:
    struct-or-union [identifier] { struct-declaration-list }
    | struct-or-union identifier

struct-or-union:
    struct 
    | union

struct-declaration-list: 
    struct-declaration
    | struct-declaration-list struct-declaration
*/

struct ast_struct_or_union_specifier : ast_node {
    //1=struct,2=union
    int struct_or_union;
    int derive_formula_id;
    std::shared_ptr<ast_token> identifier;
    std::vector<std::shared_ptr<ast_struct_declaration>> declarations;
};

/*
struct-declaration:
    specifier-qualifier-list struct-declarator-list ;

struct-declarator-list: 
    struct-declarator
    | struct-declarator-list , struct-declarator

specifier-qualifier-list:
    type-specifier [specifier-qualifier-list]
    | type-qualifier [specifier-qualifier-list]
也就是std::vector<type-specifier or type-qualifier>
type-specifier是int double...出现左递归
type-qualifier是const volatile

specifier-qualifier-list
declaration_specifier 可以认为是std::vector<storage_class_specifier or type-specifier or type-qualifier>
所以可以调用 parse_declaration_specifier 然后检查storage_class_specifier是否为空.
*/

struct ast_struct_declaration : ast_node {

    int derive_formula_id;
    std::shared_ptr<ast_declaration_specifier> specifier_qualifier_list;
    std::vector<std::shared_ptr<ast_struct_declarator>> struct_declarator_list;
};

/*
struct-declarator:
    declarator
    | [declarator] : constant-expression  
*/
struct ast_struct_declarator : ast_node {

    int derive_formula_id;
    std::shared_ptr<ast_declarator> declarator;
    std::shared_ptr<ast_constant_expression> constant_expression;
};

/*
init_declarator:
    declarator
    | declarator = initializer
*/

struct ast_init_declarator : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_declarator> declarator;
    std::shared_ptr<ast_token> assignment_operator_token;
    std::shared_ptr<ast_initializer> initializer;
};

/*
declarator.简单理解就是所声明(定义)的变量如int a;中的a就是declarator.不过实际上int a[5];中declarator是 a[5]而不是a
declarator:
  [pointer] direct_declarator

*/

struct ast_declarator : ast_node {
    //pointer可以为nullptr
    std::shared_ptr<ast_pointer> pointer;
    std::shared_ptr<ast_direct_declarator> direct_declarator;
};

/*

initializer: 
  assignment-expression
  | { initializer-list }
  | { initializer-list , }

*/

struct ast_initializer : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_assignment_expression> assignment_expression;
    std::shared_ptr<ast_initializer_list> initializer_list;
};

/*
  pointer:
    * [type_qualifier_list]
    | * [type_qualifier_list] pointer

type_qualifier_list:
    type_qualifier
    | type_qualifier_list type_qualifier
 type_qualifier在declaration_specifier的注释中已经给出了,是const volatile
   
 */

struct ast_pointer : ast_node {

    int derive_formula_id;
    std::shared_ptr<ast_token> pointer_token;
    std::vector<std::shared_ptr<ast_token>> type_qualifiers;
    std::shared_ptr<ast_pointer> next_pointer;
};

/*
direct_declarator:
  identifier
  | (declarator)
  | direct_declarator '[' [ constant_expression ] ']'
  | direct_declarator (parameter_type_list)
  | direct_declarator ( [identifier_list] )

identifier_list:
  identifier
  | identifier_list, identifier

(declarator)可以替换为([pointer] direct_declarator)
*/

struct ast_direct_declarator : ast_node {

    int derive_formula_id;
    std::shared_ptr<ast_token> identifier;
    std::shared_ptr<ast_token> left_parenthesis;
    std::shared_ptr<ast_token> right_parenthesis;
    std::shared_ptr<ast_pointer> pointer;
    std::shared_ptr<ast_direct_declarator> next_direct_declarator;
    std::shared_ptr<ast_direct_declarator> previous_direct_declarator;
    std::shared_ptr<ast_identifier_list> identifier_list;
    std::shared_ptr<ast_constant_expression> constant_expression;
    std::shared_ptr<ast_parameter_type_list> parameter_type_list;
};

/*
parameter_type_list:
    parameter_list
    | parameter_list, ...

parameter_list:
    parameter_declaration
    | parameter_list, parameter_declaration

*/
struct ast_parameter_type_list : ast_node {
    // parameter_list就是一个用逗号分割的parameter_declaration的vector.然后可以再跟一个省略号
    std::vector<std::shared_ptr<ast_parameter_declaration>> parameter_declarations;
    std::shared_ptr<ast_token> ellipsis;
};

/*

parameter-declaration:
	declaration-specifier declarator
	| declaration-specifier [abstract-declarator]
*/
struct ast_parameter_declaration : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_declaration_specifier> declaration_specifier;
    std::shared_ptr<ast_declarator> declarator;
    std::shared_ptr<ast_abstract_declarator> abstract_declarator;
};
/*

abstract_declarator:
    pointer
    | [pointer] direct_abstract_delarator

*/

struct ast_abstract_declarator : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_pointer> pointer;
    std::shared_ptr<ast_direct_abstract_declarator> direct_abstract_declarator;
};

/*
direct_abstract_delarator:
   (abstract_declarator)
   | [direct_abstract_delarator] '[' [constant_expression] ']'
   | [direct_abstract_delarator] ( [parameter_type_list] )

*/

struct ast_direct_abstract_declarator : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_direct_abstract_declarator> previous_direct_abstract_declarator;
    std::shared_ptr<ast_direct_abstract_declarator> next_direct_abstract_declarator;
    std::shared_ptr<ast_constant_expression> constant_expression;
    std::shared_ptr<ast_parameter_type_list> parameter_type_list;
};

/*
enum-specifier:
  enum [identifier] { enumerator-list }
  | enum [identifier] { enumerator-list , } 
  | enum identifier

enumerator-list:
  enumerator
  | enumerator-list , enumerator

enumerator: 
  enumeration-constant
  | enumeration-constant = constant-expression
*/

struct ast_enum_specifier : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_token> identifier;
    std::vector<std::shared_ptr<ast_enumerator>> enumerator_list;
};

struct ast_enumerator : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_token> enumeration_constant;
    std::shared_ptr<ast_constant_expression> constant_expression;
};

/*
function_definition:
| [declaration_specifier] declarator [declaration_list] compound_statement

declaration-list: 
    declaration
    | declaration-list declaration
*/
struct ast_function_definition : ast_external_declaration {
};
/*
constant_expression:
    conditional_expression
文法上constant_expression就是conditional_expression但是还需要校验表达式结果却是是constexpr
*/
struct ast_constant_expression : ast_node {
    std::shared_ptr<ast_conditional_expression> conditional_expression;
};

/*
conditional_expression:
    logical_or_expression 
    | logical_or_expression ? expression : conditional_expression
*/

struct ast_conditional_expression : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_logical_or_expression> logical_or_expression;
    std::shared_ptr<ast_expression> expression;
    std::shared_ptr<ast_conditional_expression> next_conditional_expression;
};
/*
logical_or_expression:

    logical_and_expression
    | logical_or_expression || logical_and_expression
是若干个 logical_and_expression 用 || 连接
*/
struct ast_logical_or_expression : ast_node {
    std::vector<std::shared_ptr<ast_logical_and_expression>> logical_and_expressions;
};

/*
expression: 
  assignment-expression
  | expression , assignment-expression
*/
struct ast_expression : ast_node {
    std::vector<std::shared_ptr<ast_assignment_expression>> assignment_expressions;
};

/*
logical_and_expression:
    inclusive_or_expression
    | logical_and_expression && inclusive_or_expression
是若干个 inclusive_or_expression 用 && 连接
*/

struct ast_logical_and_expression : ast_node {
    std::vector<std::shared_ptr<ast_inclusive_or_expression>> inclusive_or_expressions;
};
/*
inclusive_or_expression:
    exclusive_or_expression
    | inclusive_or_expression | exclusive_or_expression
是若干个 exclusive_or_expression 用 | 连接
*/

struct ast_inclusive_or_expression : ast_node {
    std::vector<std::shared_ptr<ast_exclusive_or_expression>> exclusive_or_expressions;
};

/*
exclusive_or_expression:
    and_expression
    | exclusive_or_expression ^ and_expression
是若干个 and_expression 用 ^ 连接
*/

struct ast_exclusive_or_expression : ast_node {
    std::vector<std::shared_ptr<ast_and_expression>> and_expressions;
};

/*
and_expression:
    equality_expression
    | and_expression & equality_expression
是若干个 equality_expression 用 & 连接

*/

struct ast_and_expression : ast_node {
    std::vector<std::shared_ptr<ast_equality_expression>> equality_expressions;
};

/*
equality_expression:
    relational_expression
    | equality_expression == relational_expression
    | equality_expression != relational_expression
equality_expression 是若干个 relational_expression 用 == 或者 != 连接
*/

struct ast_equality_expression : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_relational_expression> relational_expression;
    std::shared_ptr<ast_token> operator_token;
    std::shared_ptr<ast_equality_expression> next_equality_expression;
};

/*
relational_expression:

    shift_expression
    | relational_expression < shift_expression
    | relational_expression <= shift_expression
    | relational_expression > shift_expression
    | relational_expression >= shift_expression

relational_expression 是若干个 shift_expression 用 < <= > >= 连接
*/

struct ast_relational_expression : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_shift_expression> shift_expression;
    std::shared_ptr<ast_token> operator_token;
    std::shared_ptr<ast_relational_expression> next_relational_expression;
};

/*
shift_expression:
    additive_expression
    | shift_expression << additive_expression
    | shift_expression >> additive_expression
*/

struct ast_shift_expression : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_additive_expression> additive_expression;
    std::shared_ptr<ast_token> operator_token;
    std::shared_ptr<ast_shift_expression> next_shift_expression;
};

/*
additive_expression:
    multiplicative_expression
    | additive_expression + multiplicative_expression
    | additive_expression - multiplicative_expression

*/
struct ast_additive_expression : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_multiplicative_expression> multiplicative_expression;
    std::shared_ptr<ast_token> operator_token;
    std::shared_ptr<ast_additive_expression> next_additive_expression;
};

/*
multiplicative_expression:

    cast_expression
    | multiplicative_expression * cast_expression
    | multiplicative_expression / cast_expression
    | multiplicative_expression % cast_expression

*/
struct ast_multiplicative_expression : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_cast_expression> cast_expression;
    std::shared_ptr<ast_token> operator_token;
    std::shared_ptr<ast_multiplicative_expression> next_multiplicative_expression;
};

/*
cast_expression:

    unary_expression
    | (type_name) cast_expression

*/
struct ast_cast_expression : ast_node {
    int derive_formula_id;
    // (long long)(int)a;这样type_names是long long next_ast_cast_expression是(int)a

    std::shared_ptr<ast_unary_expression> unary_expression;
    std::shared_ptr<ast_type_name> type_name;
    std::shared_ptr<ast_cast_expression> next_cast_expression;
};

/*
unary_expression:

    postfix_expression
    | ++ unary_expression
    | -- unary_expression
    | unary_operator cast_expression
    | sizeof unary_expression
    | sizeof (type_name)
后面2种以及++ --可以合并处理.
unary_operator:
    & * + - ~ !

*/
struct ast_unary_expression : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_postfix_expression> postfix_expression;
    std::shared_ptr<ast_token> operator_token;
    std::shared_ptr<ast_cast_expression> cast_expression;
    std::shared_ptr<ast_unary_expression> next_unary_expression;
};

/*
postfix_expression:
    primary_expression
    | postfix_expression '[' expression ']'
    | postfix_expression ([argument_expression_list])
    | postfix_expression . identifier
    | postfix_expression -> identifier
    | postfix_expression ++
    | postfix_expression --

*/
struct ast_postfix_expression : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_primary_expression> primary_expression;
    std::shared_ptr<ast_expression> expression;
    std::shared_ptr<ast_expression> argument_expression_list;
    std::shared_ptr<ast_postfix_expression> previous_postfix_expression;
    std::shared_ptr<ast_token> operator_token;
    std::shared_ptr<ast_token> identifier;
};

/*
primary_expression:

    identifier
    | constant
    | string_literal
    | (expression)

*/
struct ast_primary_expression : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_token> identifier_or_constant_or_string_literal;
    std::shared_ptr<ast_expression> expression;
};

/*
argument_expression_list:

    assignment_expression
    | argument_expression_list , assignment_expression
在文法上它与expression相同但是语义不同.

assignment_expression:

    conditional_expression
    | unary_expression assignment_operator assignment_expression

assignment_operator:

    = *= /= %= += -= <<= >>= &= ^= |=

*/
struct ast_assignment_expression : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_conditional_expression> conditional_expression;
    std::shared_ptr<ast_unary_expression> unary_expression;
    std::shared_ptr<ast_token> assignment_operator;
    std::shared_ptr<ast_assignment_expression> next_assignment_expression;
};

/*
type-name:
  specifier-qualifier-list [abstract-declarator]

abstract-declarator:
  pointer
  | [pointer] direct-abstract-declarator

direct-abstract-declarator:
  ( abstract-declarator )
  | [direct-abstract-declarator] '[' [type-qualifier-list] [assignment-expression] ']'
  | [direct-abstract-declarator] '[' static [type-qualifier-list] assignment-expression ']'
  | [direct-abstract-declarator] '[' type-qualifier-list static assignment-expression ']'
  | [direct-abstract-declarator] '[' '*' ']' 
  | [direct-abstract-declarator] '(' [parameter-type-list] ')'
*/

struct ast_type_name : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_declaration_specifier> specifier_qualifier_list;
    std::shared_ptr<ast_abstract_declarator> abstract_declarator;
};
/*
initializer-list:
  [designation] initializer
  | initializer-list , [designation]  initializer
*/
struct ast_initializer_list : ast_node {
    std::vector<std::shared_ptr<ast_optional_designation_and_initializer>> optional_designation_and_initializers;
};

struct ast_optional_designation_and_initializer : ast_node {
    std::shared_ptr<ast_designation> designation;
    std::shared_ptr<ast_initializer> initializer;
};

/*
designation:
  designator-list =

designator-list:
  designator
  | designator-list designator 
*/
struct ast_designation : ast_node {
    int derive_formula_id;
    std::vector<std::shared_ptr<ast_designator>> designators;
    std::shared_ptr<ast_token> assign_token;
};

/*
designator:
  [ constant-expression ]
  | . identifier
  
  下面的初始化合法
      int a[] = {[1] = 2};
      这里的designator就是[ constant-expression ]
*/

struct ast_designator : ast_node {
    int derive_formula_id;
    std::shared_ptr<ast_token> lbracket_token;
    std::shared_ptr<ast_constant_expression> constant_expression;
    std::shared_ptr<ast_token> rbracket_token;

    std::shared_ptr<ast_token> dot_token;
    std::shared_ptr<ast_token> identifier;
};

struct ast_identifier_list : ast_node {
    std::vector<std::shared_ptr<ast_token>> identifies;
};

extern std::vector<std::string> keywords;
extern std::map<std::string, int> keyword_to_token_type_id;

std::string get_number_literal(ast_parse_context &parse_context);

std::string get_string_literal(ast_parse_context &parse_context, char expected_terminate_char);

std::shared_ptr<ast_token> get_next_token(ast_parse_context &parse_context);

std::shared_ptr<ast_translation_unit> parse_translation_unit(ast_parse_context &parse_context);

std::shared_ptr<ast_external_declaration> parse_external_declaration(ast_parse_context &parse_context);

std::shared_ptr<ast_declaration> parse_declaration(ast_parse_context &parse_context);

std::shared_ptr<ast_function_definition> parse_function_definition(ast_parse_context &parse_context);

std::shared_ptr<ast_declaration_specifier> parse_declaration_specifier(ast_parse_context &parse_context);

std::shared_ptr<ast_init_declarator> parse_init_declarator(ast_parse_context &parse_context);

std::shared_ptr<ast_struct_or_union_specifier> parse_struct_or_union_specifier(ast_parse_context &parse_context);

std::shared_ptr<ast_struct_declaration> parse_struct_declaration(ast_parse_context &parse_context);

std::shared_ptr<ast_struct_declarator> parse_struct_declarator(ast_parse_context &parse_context);

std::shared_ptr<ast_declarator> parse_declarator(ast_parse_context &parse_context);

std::shared_ptr<ast_constant_expression> parse_constant_expression(ast_parse_context &parse_context);

std::shared_ptr<ast_initializer> parse_initializer(ast_parse_context &parse_context);

std::shared_ptr<ast_pointer> parse_pointer(ast_parse_context &parse_context);

std::shared_ptr<ast_direct_declarator> parse_direct_declarator(ast_parse_context &parse_context);

std::shared_ptr<ast_parameter_type_list> parse_parameter_type_list(ast_parse_context &parse_context);

std::shared_ptr<ast_parameter_declaration> parse_parameter_declaration(ast_parse_context &parse_context);

std::shared_ptr<ast_abstract_declarator> parse_abstract_declarator(ast_parse_context &parse_context);

std::shared_ptr<ast_enum_specifier> parse_enum_specifier(ast_parse_context &parse_context);

std::shared_ptr<ast_direct_abstract_declarator> parse_direct_abstract_declarator(ast_parse_context &parse_context);

std::shared_ptr<ast_enumerator> parse_enumerator(ast_parse_context &parse_context);

std::shared_ptr<ast_conditional_expression> parse_conditional_expression(ast_parse_context &parse_context);

std::shared_ptr<ast_logical_or_expression> parse_logical_or_expression(ast_parse_context &parse_context);

std::shared_ptr<ast_expression> parse_expression(ast_parse_context &parse_context);

std::shared_ptr<ast_logical_and_expression> parse_logical_and_expression(ast_parse_context &parse_context);

std::shared_ptr<ast_inclusive_or_expression> parse_inclusive_or_expression(ast_parse_context &parse_context);

std::shared_ptr<ast_exclusive_or_expression> parse_exclusive_or_expression(ast_parse_context &parse_context);

std::shared_ptr<ast_and_expression> parse_and_expression(ast_parse_context &parse_context);

std::shared_ptr<ast_equality_expression> parse_equality_expression(ast_parse_context &parse_context);

std::shared_ptr<ast_relational_expression> parse_relational_expression(ast_parse_context &parse_context);

std::shared_ptr<ast_shift_expression> parse_shift_expression(ast_parse_context &parse_context);

std::shared_ptr<ast_additive_expression> parse_additive_expression(ast_parse_context &parse_context);

std::shared_ptr<ast_multiplicative_expression> parse_multiplicative_expression(ast_parse_context &parse_context);

std::shared_ptr<ast_cast_expression> parse_cast_expression(ast_parse_context &parse_context);

std::shared_ptr<ast_unary_expression> parse_unary_expression(ast_parse_context &parse_context);

std::shared_ptr<ast_postfix_expression> parse_postfix_expression(ast_parse_context &parse_context);

std::shared_ptr<ast_primary_expression> parse_primary_expression(ast_parse_context &parse_context);

std::shared_ptr<ast_assignment_expression> parse_assignment_expression(ast_parse_context &parse_context);

std::shared_ptr<ast_type_name> parse_type_name(ast_parse_context &parse_context);

std::shared_ptr<ast_initializer_list> parse_initializer_list(ast_parse_context &parse_context);

std::shared_ptr<ast_designation> parse_designation(ast_parse_context &parse_context);

std::shared_ptr<ast_designator> parse_designator(ast_parse_context &parse_context);

std::shared_ptr<ast_optional_designation_and_initializer>
parse_optional_designation_and_initializer(ast_parse_context &parse_context);

std::shared_ptr<ast_identifier_list> parse_identifier_list(ast_parse_context &parse_context);

bool is_type_name(ast_parse_context &parse_context, std::shared_ptr<ast_token> token);

bool is_typedef_name(ast_parse_context &parse_context, std::string identifier);

std::shared_ptr<ast_unary_expression> extract_unary_expression_from_conditional_expression(
        std::shared_ptr<ast_conditional_expression> conditional_expression);

#endif // #ifndef node_h
