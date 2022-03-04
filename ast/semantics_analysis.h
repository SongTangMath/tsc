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

  bool is_inline;
  bool is_noreturn;

  int type_id;
};

struct symbol_table_node {
  std::shared_ptr<symbol_table_node> parent;
  std::map<std::string, tsc_type> identifier_types;
  std::map<std::string, tsc_type> struct_or_union_names;
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