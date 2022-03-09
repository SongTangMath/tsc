#include "parser.h"
#include <map>

enum {
  PRIMITIVE_TYPE_VOID,
  PRIMITIVE_TYPE_CHAR,
  PRIMITIVE_TYPE_UNSIGNED_CHAR,
  PRIMITIVE_TYPE_SHORT,
  PRIMITIVE_TYPE_UNSIGNED_SHORT,
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
  RECORD_TYPE_ENUM
};

struct tsc_type {
  bool const_type_qualifier_set;
  bool restrict_type_qualifier_set;
  bool volatile_type_qualifier_set;

  bool is_typedef;
  bool is_extern;
  bool is_static;
  bool is_register;
  //for functions
  bool is_inline;
  bool is_noreturn;
  //for struct union enum
  bool is_complete; // struct A; -> incomplete

  int type_id;
};

struct symbol_table_node {
  std::shared_ptr<symbol_table_node> parent;
  std::map<std::string, std::shared_ptr<tsc_type>> identifier_and_types;
  std::map<std::string, std::shared_ptr<tsc_type>> struct_union_enum_names; //通常也称为tags
};

struct semantics_analysis_context {
  std::shared_ptr<symbol_table_node> current_symbol_table_node;
};

int semantics_analysis(std::shared_ptr<ast_node> translation_unit);
int analyze_declaration(std::shared_ptr<ast_node> declaration, semantics_analysis_context &context, bool is_global);
int analyze_function_definition(std::shared_ptr<ast_node> function_definition, semantics_analysis_context &context);
int analyze_declaration_specifiers(std::shared_ptr<ast_node> declaration_specifiers,
                                   semantics_analysis_context &context, std::shared_ptr<tsc_type> type, bool is_global);
int check_primitive_type_specifiers(std::vector<std::shared_ptr<ast_node>> &type_specifiers, bool is_global,
                                    std::shared_ptr<tsc_type> type);

int check_type_qualifiers(std::vector<std::shared_ptr<ast_node>> &type_qualifiers, bool is_global,
                          std::shared_ptr<tsc_type> type);

int check_storage_class_specifiers(std::vector<std::shared_ptr<ast_node>> &storage_class_specifiers, bool is_global,
                                   std::shared_ptr<tsc_type> type);

int check_function_specifiers(std::vector<std::shared_ptr<ast_node>> &function_specifiers, bool is_global,
                              std::shared_ptr<tsc_type> type);

int analyze_enum_specifier(std::shared_ptr<ast_node> enum_specifier, semantics_analysis_context &context,
                           std::shared_ptr<tsc_type> type, bool is_global);
int analyze_struct_or_union_specifier(std::shared_ptr<ast_node> struct_or_union_specifier,
                                      semantics_analysis_context &context, std::shared_ptr<tsc_type> type,
                                      bool is_global);

//  如果有2个struct_union_enum同名,它们是否一致.
//  例如type1是声明struct A; type2是定义struct A{...};则一致.如果type2是个union那么不一致
bool check_type_compatibility(std::shared_ptr<tsc_type> type1, std::shared_ptr<tsc_type> type2);

int analyze_enumerator_list(std::shared_ptr<ast_node> enumerator_list, semantics_analysis_context &context,
                            std::shared_ptr<tsc_type> type, bool is_global);
//first=semantics_analysis_result second=  enumeration_constant_value
std::pair<int, int> analyze_enumerator(std::shared_ptr<ast_node> enumerator, semantics_analysis_context &context,
                                       std::shared_ptr<tsc_type> type, bool is_global, int next_value);

int analyze_constant_expression(std::shared_ptr<ast_node> constant_expression, semantics_analysis_context &context);
int analyze_conditional_expression(std::shared_ptr<ast_node> conditional_expression, semantics_analysis_context &context);

int analyze_logical_or_expression(std::shared_ptr<ast_node> logical_or_expression, semantics_analysis_context &context);
int analyze_logical_and_expression(std::shared_ptr<ast_node> logical_and_expression, semantics_analysis_context &context);

int analyze_inclusive_or_expression(std::shared_ptr<ast_node> inclusive_or_expression, semantics_analysis_context &context);
int analyze_exclusive_or_expression(std::shared_ptr<ast_node> exclusive_or_expression, semantics_analysis_context &context);
int analyze_and_expression(std::shared_ptr<ast_node> and_expression, semantics_analysis_context &context);

int analyze_equality_expression(std::shared_ptr<ast_node> equality_expression, semantics_analysis_context &context);
int analyze_relational_expression(std::shared_ptr<ast_node> relational_expression, semantics_analysis_context &context);
int analyze_shift_expression(std::shared_ptr<ast_node> shift_expression, semantics_analysis_context &context);

int analyze_additive_expression(std::shared_ptr<ast_node> additive_expression, semantics_analysis_context &context);
int analyze_multiplicative_expression(std::shared_ptr<ast_node> multiplicative_expression, semantics_analysis_context &context);
int analyze_cast_expression(std::shared_ptr<ast_node> cast_expression, semantics_analysis_context &context);

int analyze_unary_expression(std::shared_ptr<ast_node> unary_expression, semantics_analysis_context &context);
int analyze_postfix_expression(std::shared_ptr<ast_node> postfix_expression, semantics_analysis_context &context);
int analyze_primary_expression(std::shared_ptr<ast_node> primary_expression, semantics_analysis_context &context);


int analyze_expression(std::shared_ptr<ast_node> expression, semantics_analysis_context &context);

int analyze_assignment_expression(std::shared_ptr<ast_node> assignment_expression, semantics_analysis_context &context);
int analyze_argument_expression_list(std::shared_ptr<ast_node> argument_expression_list, semantics_analysis_context &context);
