#include "parser.h"
#include <map>
#include <set>

enum {
    PRIMITIVE_TYPE_VOID,
    PRIMITIVE_TYPE_CHAR,
    PRIMITIVE_TYPE_UNSIGNED_CHAR,
    PRIMITIVE_TYPE_SHORT,
    PRIMITIVE_TYPE_UNSIGNED_SHORT,
    PRIMITIVE_TYPE_ENUM,
    PRIMITIVE_TYPE_INT,
    PRIMITIVE_TYPE_UNSIGNED_INT,
    PRIMITIVE_TYPE_LONG,
    PRIMITIVE_TYPE_UNSIGNED_LONG,
    PRIMITIVE_TYPE_LONG_LONG,
    PRIMITIVE_TYPE_UNSIGNED_LONG_LONG,
    PRIMITIVE_TYPE_FLOAT,
    PRIMITIVE_TYPE_DOUBLE,
    PRIMITIVE_TYPE_LONG_DOUBLE,
    RECORD_TYPE_STRUCT_OR_UNION,
    SCALAR_TYPE_POINTER,
    SCALAR_TYPE_ARRAY,
    SUB_TYPE_STRUCT,
    SUB_TYPE_UNION,
    TYPE_FUNCTION
};

enum {
    SYMBOL_TYPE_VARIABLE,
    SYMBOL_TYPE_TEMPORARY_VARIABLE,
    SYMBOL_TYPE_FUNCTION,
    SYMBOL_TYPE_ICONSTANT,
    SYMBOL_TYPE_FCONSTANT,
    SYMBOL_TYPE_ENUMERATION_CONSTANT,
    SYMBOL_TYPE_POINTER_CONSTANT
};

enum {
    BINARY_OPERATOR_ADD,
    BINARY_OPERATOR_SUB,
    BINARY_OPERATOR_MUL,
    BINARY_OPERATOR_DIV,
    BINARY_OPERATOR_MOD,
    BINARY_OPERATOR_EQ_OP,
    BINARY_OPERATOR_NE_OP,

    BINARY_OPERATOR_LESS_THAN,
    BINARY_OPERATOR_GREATER_THAN,

    BINARY_OPERATOR_LESS_THAN_OR_EQUAL,
    BINARY_OPERATOR_GREATER_THAN_OR_EQUAL,
    BINARY_OPERATOR_AND,
    BINARY_OPERATOR_OR,
    BINARY_OPERATOR_BITAND,
    BINARY_OPERATOR_BITOR,
    BINARY_OPERATOR_BITXOR,
    BINARY_OPERATOR_LEFT_SHIFT,
    BINARY_OPERATOR_RIGHT_SHIFT,

    UNARY_OPERATOR_BIT_AND,
    UNARY_OPERATOR_MUL,
    UNARY_OPERATOR_ADD,
    UNARY_OPERATOR_SUB,
    UNARY_OPERATOR_BITNOT,
    UNARY_OPERATOR_NOT,

    OPERATOR_ARRAY_SUBSCRIPT,
    OPERATOR_FUNCTION_CALL,
    OPERATOR_MEMBER_ACCESS,
    OPERATOR_POINTER_MEMBER_ACCESS,
    OPERATOR_POST_INC,
    OPERATOR_POST_DEC,
    OPERATOR_PRE_INC,
    OPERATOR_PRE_DEC,

    OPERATOR_CAST,
    OPERATOR_ASSIGN,
    OPERATOR_MUL_ASSIGN,
    OPERATOR_DIV_ASSIGN,
    OPERATOR_MOD_ASSIGN,
    OPERATOR_ADD_ASSIGN,
    OPERATOR_SUB_ASSIGN,
    OPERATOR_LEFT_SHIFT_ASSIGN,
    OPERATOR_RIGHT_SHIFT_ASSIGN,
    OPERATOR_BITAND_ASSIGN,
    OPERATOR_BITXOR_ASSIGN,
    OPERATOR_BITOR_ASSIGN,
    OPERATOR_QUESTION_COLON
};

enum {
    DECLARATION_SPECIFIERS_LOCATION_NONE,
    DECLARATION_SPECIFIERS_LOCATION_DECLARATION,
    DECLARATION_SPECIFIERS_LOCATION_PARAMETER_LIST
};

enum {
    STATEMENT_TYPE_COMPOUND_STATEMENT,
    STATEMENT_TYPE_LABELED_STATEMENT,
    STATEMENT_TYPE_EXPRESSION_STATEMENT,
    STATEMENT_TYPE_SELECTION_STATEMENT,
    STATEMENT_TYPE_ITERATION_STATEMENT,
    STATEMENT_TYPE_JUMP_STATEMENT,

    STATEMENT_SUB_TYPE_IF,
    STATEMENT_SUB_TYPE_IF_ELSE,
    STATEMENT_SUB_TYPE_SWITCH
};

void setup_type_system();

struct symbol_table_node {
    std::shared_ptr<symbol_table_node> parent;
    std::map<std::string, std::shared_ptr<tsc_symbol>> identifier_and_symbols;
    std::map<std::string, std::shared_ptr<tsc_type>> struct_union_enum_names; //通常也称为tags
};

struct tsc_function {
    std::string name;
    std::shared_ptr<tsc_symbol> function_symbol;
    std::shared_ptr<ast_node> compound_statement_node;

    std::set<std::string> labels;
};

struct semantics_analysis_context {
    //符号表允许struct tag,变量名或者函数名可以相同.例如 struct A{}; int A; OK.
    // void func(); int func; OK
    std::shared_ptr<symbol_table_node> current_symbol_table_node;
    std::vector<std::shared_ptr<tsc_function>> functions;
};

int semantics_analysis(std::shared_ptr<ast_node> translation_unit);

int analyze_declaration(std::shared_ptr<ast_node> declaration, semantics_analysis_context &context,
                        std::shared_ptr<tsc_symbol> &symbol);

int analyze_function_definition(std::shared_ptr<ast_node> function_definition, semantics_analysis_context &context);

int analyze_declaration_specifiers(std::shared_ptr<ast_node> declaration_specifiers,
                                   semantics_analysis_context &context, std::shared_ptr<tsc_symbol> &symbol,
                                   int declaration_specifiers_location);

int check_type_specifiers(std::vector<std::shared_ptr<ast_node>> &type_specifiers, semantics_analysis_context &context,
                          std::shared_ptr<tsc_symbol> &symbol, int declaration_specifiers_location);

int check_type_qualifiers(std::vector<std::shared_ptr<ast_node>> &type_qualifiers, semantics_analysis_context &context,
                          std::shared_ptr<tsc_symbol> &symbol);

int check_storage_class_specifiers(std::vector<std::shared_ptr<ast_node>> &storage_class_specifiers,
                                   semantics_analysis_context &context, std::shared_ptr<tsc_symbol> &symbol);

int check_function_specifiers(std::vector<std::shared_ptr<ast_node>> &function_specifiers,
                              std::shared_ptr<tsc_symbol> &symbol);

int analyze_enum_specifier(std::shared_ptr<ast_node> enum_specifier, semantics_analysis_context &context,
                           std::shared_ptr<tsc_symbol> &symbol, int declaration_specifiers_location);

int analyze_struct_or_union_specifier(std::shared_ptr<ast_node> struct_or_union_specifier,
                                      semantics_analysis_context &context, std::shared_ptr<tsc_symbol> &symbol,
                                      int declaration_specifiers_location);

//  如果有2个struct_union_enum同名,它们是否一致.
//  例如type1是声明struct A; type2是定义struct A{...};则一致.如果type2是个union那么不一致
bool check_type_compatibility(std::shared_ptr<tsc_type> type1, std::shared_ptr<tsc_type> type2);

bool check_same_type(std::shared_ptr<tsc_type> type1, std::shared_ptr<tsc_type> type2, bool consider_const);

bool check_complete_type(std::shared_ptr<tsc_type> type);

int analyze_enumerator_list(std::shared_ptr<ast_node> enumerator_list, semantics_analysis_context &context,
                            std::shared_ptr<tsc_symbol> &symbol);

int analyze_enumerator(std::shared_ptr<ast_node> enumerator, semantics_analysis_context &context, int next_value,
                       int &calculated_enumeration_constant_value);

int analyze_constant_expression(std::shared_ptr<ast_node> constant_expression, semantics_analysis_context &context);

int analyze_conditional_expression(std::shared_ptr<ast_node> conditional_expression,
                                   semantics_analysis_context &context);

int analyze_logical_or_expression(std::shared_ptr<ast_node> logical_or_expression, semantics_analysis_context &context);

int analyze_logical_and_expression(std::shared_ptr<ast_node> logical_and_expression,
                                   semantics_analysis_context &context);

int analyze_inclusive_or_expression(std::shared_ptr<ast_node> inclusive_or_expression,
                                    semantics_analysis_context &context);

int analyze_exclusive_or_expression(std::shared_ptr<ast_node> exclusive_or_expression,
                                    semantics_analysis_context &context);

int analyze_and_expression(std::shared_ptr<ast_node> and_expression, semantics_analysis_context &context);

int analyze_equality_expression(std::shared_ptr<ast_node> equality_expression, semantics_analysis_context &context);

int analyze_relational_expression(std::shared_ptr<ast_node> relational_expression, semantics_analysis_context &context);

int analyze_shift_expression(std::shared_ptr<ast_node> shift_expression, semantics_analysis_context &context);

int analyze_additive_expression(std::shared_ptr<ast_node> additive_expression, semantics_analysis_context &context);

int analyze_multiplicative_expression(std::shared_ptr<ast_node> multiplicative_expression,
                                      semantics_analysis_context &context);

int analyze_cast_expression(std::shared_ptr<ast_node> cast_expression, semantics_analysis_context &context);

int analyze_unary_expression(std::shared_ptr<ast_node> unary_expression, semantics_analysis_context &context);

int analyze_postfix_expression(std::shared_ptr<ast_node> postfix_expression, semantics_analysis_context &context);

int analyze_primary_expression(std::shared_ptr<ast_node> primary_expression, semantics_analysis_context &context);

int analyze_type_name(std::shared_ptr<ast_node> type_name, semantics_analysis_context &context);

int analyze_expression(std::shared_ptr<ast_node> expression, semantics_analysis_context &context);

int analyze_constant(std::shared_ptr<ast_node> constant, semantics_analysis_context &context);

int check_integer_constant(std::shared_ptr<ast_node> integer_constant);

int check_floating_constant(std::shared_ptr<ast_node> floating_constant);

int analyze_assignment_expression(std::shared_ptr<ast_node> assignment_expression, semantics_analysis_context &context);

int analyze_argument_expression_list(std::shared_ptr<ast_node> argument_expression_list,
                                     semantics_analysis_context &context, std::shared_ptr<tsc_type> function_type);

int analyze_string(std::shared_ptr<ast_node> string_node, semantics_analysis_context &context);

bool is_integer_constant(const std::shared_ptr<ast_node> &node);

bool is_floating_constant(const std::shared_ptr<ast_node> &node);

bool is_constant(const std::shared_ptr<ast_node> &node);

bool is_array_or_pointer(const std::shared_ptr<ast_node> &node);

bool is_integer(const std::shared_ptr<ast_node> &node);

bool is_floating_number(const std::shared_ptr<ast_node> &node);

bool is_number(const std::shared_ptr<ast_node> &node);

bool is_integer_or_floating_number(const std::shared_ptr<ast_node> &node);

bool is_struct_union_enum_number(const std::shared_ptr<ast_node> &node);

bool is_scalar_type(const std::shared_ptr<tsc_type> &type);

std::shared_ptr<tsc_statement_context> find_parent_statement_context(const std::shared_ptr<tsc_statement_context> &current_statement_context,int parent_statement_type_to_find);

std::shared_ptr<tsc_symbol> lookup_symbol(std::shared_ptr<symbol_table_node> symbol_table_node,
                                          const std::string &symbol_identifier, bool search_outer);

std::shared_ptr<tsc_type> lookup_type(std::shared_ptr<symbol_table_node> symbol_table_node,
                                      const std::string &type_name, bool search_outer);

int construct_binary_expression_symbol(std::shared_ptr<ast_node> parent, int binary_operator,
                                       std::shared_ptr<ast_node> left, std::shared_ptr<ast_node> right);

int construct_binary_expression_symbol_type(std::shared_ptr<ast_node> parent, int binary_operator,
                                            std::shared_ptr<ast_node> left, std::shared_ptr<ast_node> right);

int construct_unary_expression_symbol(std::shared_ptr<ast_node> parent, int unary_operator,
                                      std::shared_ptr<ast_node> operand);

int analyze_init_declarator_list(std::shared_ptr<ast_node> init_declarator_list, semantics_analysis_context &context,
                                 std::shared_ptr<tsc_symbol> &symbol);

int analyze_specifier_qualifier_list(std::shared_ptr<ast_node> specifier_qualifier_list,
                                     semantics_analysis_context &context);

int analyze_struct_declaration_list(std::shared_ptr<ast_node> struct_declaration_list,
                                    semantics_analysis_context &context, std::shared_ptr<tsc_symbol> &symbol);

int analyze_struct_declaration(std::shared_ptr<ast_node> struct_declaration, semantics_analysis_context &context,
                               std::shared_ptr<tsc_symbol> &symbol);

int analyze_struct_declarator_list(std::shared_ptr<ast_node> struct_declarator_list,
                                   semantics_analysis_context &context, std::shared_ptr<tsc_symbol> &symbol,
                                   std::shared_ptr<tsc_symbol> &field_symbol);

int analyze_struct_declarator(std::shared_ptr<ast_node> struct_declarator, semantics_analysis_context &context,
                              std::shared_ptr<tsc_symbol> &symbol, std::shared_ptr<ast_node> &out_identifier_node);

int analyze_declarator(std::shared_ptr<ast_node> declarator, semantics_analysis_context &context,
                       std::shared_ptr<ast_node> &out_identifier_node);

int analyze_direct_declarator(std::shared_ptr<ast_node> direct_declarator, semantics_analysis_context &context,
                              std::shared_ptr<ast_node> &out_identifier_node);

int analyze_pointer(std::shared_ptr<ast_node> pointer, semantics_analysis_context &context,
                    std::shared_ptr<tsc_symbol> &derived_declarator_symbol,
                    std::shared_ptr<tsc_symbol> &direct_declarator_symbol);

int analyze_parameter_type_list(std::shared_ptr<ast_node> parameter_type_list, semantics_analysis_context &context,
                                std::shared_ptr<tsc_symbol> &function_symbol);

int analyze_type_qualifier_list(std::shared_ptr<ast_node> type_qualifier_list, semantics_analysis_context &context,
                                std::shared_ptr<tsc_symbol> &symbol);

int analyze_init_declarator(std::shared_ptr<ast_node> init_declarator, semantics_analysis_context &context,
                            std::shared_ptr<tsc_symbol> &symbol, std::shared_ptr<ast_node> &out_identifier_node);

int add_declarator_identifier_to_symbol_table(std::shared_ptr<ast_node> init_declarator_or_declarator,
                                              semantics_analysis_context &context,
                                              std::shared_ptr<ast_node> declarator_identifier_node);

int add_type_name_to_symbol_table(std::shared_ptr<tsc_symbol> &symbol, semantics_analysis_context &context,
                                  std::shared_ptr<ast_node> identifier_node);

int add_nested_anonymous_struct_fields_to_parent_struct(std::shared_ptr<tsc_symbol> &parent_struct_symbol,
                                                        std::shared_ptr<tsc_symbol> &child_struct_symbol,
                                                        semantics_analysis_context &context);

int analyze_initializer(std::shared_ptr<ast_node> initializer, semantics_analysis_context &context,
                        std::shared_ptr<tsc_symbol> symbol_to_initialize);

int analyze_parameter_declaration(std::shared_ptr<ast_node> parameter_declaration, semantics_analysis_context &context,
                                  std::shared_ptr<tsc_symbol> &function_symbol,
                                  std::shared_ptr<tsc_symbol> &out_parameter_symbol);

int analyze_abstract_declarator(std::shared_ptr<ast_node> abstract_declarator, semantics_analysis_context &context,
                                std::shared_ptr<tsc_symbol> &out_anonymous_symbol);

int analyze_direct_abstract_declarator(std::shared_ptr<ast_node> direct_abstract_declarator,
                                       semantics_analysis_context &context,
                                       std::shared_ptr<tsc_symbol> &out_anonymous_symbol);

int check_function_or_pointer_to_function(std::shared_ptr<ast_node> postfix_expression,
                                          semantics_analysis_context &context,
                                          std::shared_ptr<tsc_type> &out_function_type);

int analyze_initializer_list(std::shared_ptr<ast_node> initializer_list, semantics_analysis_context &context,
                             std::shared_ptr<tsc_symbol> &symbol_to_initialize);

int analyze_designation(std::shared_ptr<ast_node> designation, semantics_analysis_context &context,
                             std::shared_ptr<tsc_symbol> &designated_symbol_to_initialize);

int analyze_designator(std::shared_ptr<ast_node> designator, semantics_analysis_context &context,
                        std::shared_ptr<tsc_symbol> &designated_symbol_to_initialize);

bool check_can_assign(const std::shared_ptr<tsc_symbol> &left, const std::shared_ptr<tsc_symbol> &right);

int check_common_type(const std::shared_ptr<ast_node> &first, const std::shared_ptr<ast_node> &second,
                      std::shared_ptr<tsc_type> &out_type);

int analyze_compound_statement(std::shared_ptr<ast_node> compound_statement, semantics_analysis_context &context,
                               std::shared_ptr<tsc_function> function);

int analyze_statement(std::shared_ptr<ast_node> statement, semantics_analysis_context &context,
                      std::shared_ptr<tsc_function> function);

int analyze_labeled_statement(std::shared_ptr<ast_node> labeled_statement, semantics_analysis_context &context,
                              std::shared_ptr<tsc_function> function);

int analyze_expression_statement(std::shared_ptr<ast_node> expression_statement, semantics_analysis_context &context,
                                 std::shared_ptr<tsc_function> function);

int analyze_selection_statement(std::shared_ptr<ast_node> selection_statement, semantics_analysis_context &context,
                                std::shared_ptr<tsc_function> function);

int analyze_iteration_statement(std::shared_ptr<ast_node> iteration_statement, semantics_analysis_context &context,
                                std::shared_ptr<tsc_function> function);

int analyze_jump_statement(std::shared_ptr<ast_node> jump_statement, semantics_analysis_context &context,
                           std::shared_ptr<tsc_function> function);
