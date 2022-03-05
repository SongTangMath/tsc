#include "semantics_analysis.h"

/*

translation_unit
	: external_declaration
	| translation_unit external_declaration
	;

external_declaration
	: function_definition
	| declaration
	;
*/

int semantics_analysis(std::shared_ptr<ast_node> translation_unit) {
  int semantics_analysis_result = 0;
  std::vector<std::shared_ptr<ast_node>> external_declarations;
  std::shared_ptr<ast_node> node = translation_unit;
  semantics_analysis_context context;
  context.current_symbol_table_node = std::make_shared<symbol_table_node>();
  while (node->node_type == NODE_TYPE_TRANSLATION_UNIT &&
         node->node_sub_type == NODE_TYPE_TRANSLATION_UNIT_SUBTYPE_TRANSLATION_UNIT_EXTERNAL_DECLARATION) {
    external_declarations.push_back(node->items[1]);
    node = node->items[0];
  }

  external_declarations.push_back(node->items[0]);
  // 这里必须把 external_declarations 反过来.因为我们translation_unit文法定义是左递归的.
  // 语法树根节点的 external_declaration 实际上是最后一个
  external_declarations =
      std::vector<std::shared_ptr<ast_node>>(external_declarations.rbegin(), external_declarations.rend());
  for (std::shared_ptr<ast_node> external_declaration : external_declarations) {
    // printf("external_declaration->node_sub_type %d\n", external_declaration->node_sub_type);
    switch (external_declaration->node_sub_type) {
    case NODE_TYPE_EXTERNAL_DECLARATION_SUBTYPE_DECLARATION:
      semantics_analysis_result = analyze_declaration(external_declaration->items[0], context, true);
      if (semantics_analysis_result)
        return semantics_analysis_result;
      break;

    case NODE_TYPE_EXTERNAL_DECLARATION_SUBTYPE_FUNCTION_DEFINITION:
      semantics_analysis_result = analyze_function_definition(external_declaration->items[0], context);
      if (semantics_analysis_result)
        return semantics_analysis_result;
      break;

    default:
      break;
    }
  }
  return 0;
}

/*
declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';'
	| static_assert_declaration
	;
*/

int analyze_declaration(std::shared_ptr<ast_node> declaration, semantics_analysis_context &context, bool is_global) {

  if (declaration->node_sub_type == NODE_TYPE_DECLARATION_SUBTYPE_STATIC_ASSERT_DECLARATION) {
    printf("%s:%d error:\n\tstatic_assert_declaration not supported\n", input_file_name.c_str(),
           declaration->get_first_terminal_line_no());
    return 1;
  }
  int semantics_analysis_result = 0;
  std::shared_ptr<ast_node> declaration_specifiers = declaration->items[0];

  std::shared_ptr<tsc_type> type = std::make_shared<tsc_type>();
  semantics_analysis_result = analyze_declaration_specifiers(declaration_specifiers, context, type, is_global);
  if (semantics_analysis_result)
    return semantics_analysis_result;
  return 0;
}

int analyze_function_definition(std::shared_ptr<ast_node> function_definition, semantics_analysis_context &context) {
  return 0;
}

/*
declaration_specifiers
	: storage_class_specifier declaration_specifiers
	| storage_class_specifier
	| type_specifier declaration_specifiers
	| type_specifier
	| type_qualifier declaration_specifiers
	| type_qualifier
	| function_specifier declaration_specifiers
	| function_specifier
	| alignment_specifier declaration_specifiers
	| alignment_specifier
	;
*/

int analyze_declaration_specifiers(std::shared_ptr<ast_node> declaration_specifiers,
                                   semantics_analysis_context &context, std::shared_ptr<tsc_type> type,
                                   bool is_global) {
  std::vector<std::shared_ptr<ast_node>> storage_class_specifiers;
  std::vector<std::shared_ptr<ast_node>> type_specifiers;
  std::vector<std::shared_ptr<ast_node>> type_qualifiers;
  std::vector<std::shared_ptr<ast_node>> function_specifiers;

  std::shared_ptr<ast_node> node = declaration_specifiers;

  int semantics_analysis_result = 0;

  while (node->node_type == NODE_TYPE_DECLARATION_SPECIFIERS) {
    bool has_next = true;

    switch (node->node_sub_type) {

    case NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_STORAGE_CLASS_SPECIFIER_DECLARATION_SPECIFIERS:
      storage_class_specifiers.push_back(node->items[0]);
      node = node->items[1];
      break;

    case NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_STORAGE_CLASS_SPECIFIER:
      storage_class_specifiers.push_back(node->items[0]);
      has_next = false;
      break;

    case NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_TYPE_SPECIFIER_DECLARATION_SPECIFIERS:
      type_specifiers.push_back(node->items[0]);
      node = node->items[1];
      break;

    case NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_TYPE_SPECIFIER:
      type_specifiers.push_back(node->items[0]);
      has_next = false;
      break;

    case NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_TYPE_QUALIFIER_DECLARATION_SPECIFIERS:
      type_qualifiers.push_back(node->items[0]);
      node = node->items[1];
      break;
    case NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_TYPE_QUALIFIER:
      type_qualifiers.push_back(node->items[0]);
      has_next = false;
      break;
    case NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_FUNCTION_SPECIFIER_DECLARATION_SPECIFIERS:
      function_specifiers.push_back(node->items[0]);
      node = node->items[1];
      break;

    case NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_FUNCTION_SPECIFIER:
      function_specifiers.push_back(node->items[0]);
      has_next = false;
      break;

    case NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_ALIGNMENT_SPECIFIER_DECLARATION_SPECIFIERS:
    case NODE_TYPE_DECLARATION_SPECIFIERS_SUBTYPE_ALIGNMENT_SPECIFIER:
      printf("%s:%d error:\n\talignment_specifier not supported\n", input_file_name.c_str(),
             declaration_specifiers->get_first_terminal_line_no());
      return 1;

    default:
      break;
    }

    if (!has_next)
      break;
  }

  semantics_analysis_result = check_storage_class_specifiers(storage_class_specifiers, is_global, type);
  if (semantics_analysis_result)
    return semantics_analysis_result;

  semantics_analysis_result = check_type_qualifiers(type_qualifiers, is_global, type);
  if (semantics_analysis_result)
    return semantics_analysis_result;

  semantics_analysis_result = check_function_specifiers(function_specifiers, is_global, type);
  if (semantics_analysis_result)
    return semantics_analysis_result;

  semantics_analysis_result = check_primitive_type_specifiers(type_specifiers, is_global, type);
  if (semantics_analysis_result)
    return semantics_analysis_result;

  // printf("%d\n", type->type_id);

  //基本类型解析完毕.考虑struct union enum.注意符号表的处理.

  switch (type->type_id) {
  case RECORD_TYPE_ENUM: {

    std::shared_ptr<ast_node> enum_specifier = type_specifiers[0]->items[0];
    semantics_analysis_result = analyze_enum_specifier(enum_specifier, context, type, is_global);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    break;
  }
  case RECORD_TYPE_STRUCT_OR_UNION: {

    std::shared_ptr<ast_node> struct_or_union_specifier = type_specifiers[0]->items[0];
    semantics_analysis_result = analyze_struct_or_union_specifier(struct_or_union_specifier, context, type, is_global);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    break;
  }

  default:
    break;
  }

  return 0;
}

int check_function_specifiers(std::vector<std::shared_ptr<ast_node>> &function_specifiers, bool is_global,
                              std::shared_ptr<tsc_type> type) {
  for (std::shared_ptr<ast_node> function_specifier : function_specifiers) {
    switch (function_specifier->node_sub_type) {
    case NODE_TYPE_FUNCTION_SPECIFIER_SUBTYPE_INLINE:
      type->is_inline = true;
    case NODE_TYPE_FUNCTION_SPECIFIER_SUBTYPE_NORETURN:
      type->is_noreturn = true;
      break;
    }
  }
  return 0;
}
/*
storage_class_specifier
	: TYPEDEF	
	| EXTERN
	| STATIC
	| THREAD_LOCAL
	| AUTO
	| REGISTER
	;
*/

int check_storage_class_specifiers(std::vector<std::shared_ptr<ast_node>> &storage_class_specifiers, bool is_global,
                                   std::shared_ptr<tsc_type> type) {

  int typedef_count = 0;
  int extern_count = 0;
  int static_count = 0;
  int auto_count = 0;
  int register_count = 0;

  for (std::shared_ptr<ast_node> storage_class_specifier : storage_class_specifiers) {
    switch (storage_class_specifier->node_sub_type) {
    case NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_TYPEDEF:
      typedef_count++;
      type->is_typedef = true;
      break;

    case NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_EXTERN:
      extern_count++;
      type->is_extern = true;
      break;

    case NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_STATIC:
      static_count++;
      type->is_static = true;
      break;

    case NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_THREAD_LOCAL:
      printf("%s:%d error:\n\tunsupported C11 'thread_local' in storage_class_specifiers\n", input_file_name.c_str(),
             storage_class_specifier->get_first_terminal_line_no());
      return 1;

    case NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_AUTO:
      if (is_global) {
        printf("%s:%d error:\n\tglobal variables cannot be auto in storage_class_specifiers\n", input_file_name.c_str(),
               storage_class_specifier->get_first_terminal_line_no());
        return 1;
      }
      auto_count++;
      break;

    case NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_REGISTER:
      if (is_global) {
        printf("%s:%d error:\n\tglobal variables cannot be register in storage_class_specifiers\n",
               input_file_name.c_str(), storage_class_specifier->get_first_terminal_line_no());
        return 1;
      }
      register_count++;
      type->is_register = true;
      break;
    }
  }

  if (typedef_count > 1) {
    printf("%s:%d error:\n\tduplicate 'typedef' in storage_class_specifiers\n", input_file_name.c_str(),
           storage_class_specifiers[0]->get_first_terminal_line_no());
    return 1;
  }
  if (extern_count > 1) {
    printf("%s:%d error:\n\tduplicate 'extern' in storage_class_specifiers\n", input_file_name.c_str(),
           storage_class_specifiers[0]->get_first_terminal_line_no());
    return 1;
  }
  if (static_count > 1) {
    printf("%s:%d error:\n\tduplicate 'static' in storage_class_specifiers\n", input_file_name.c_str(),
           storage_class_specifiers[0]->get_first_terminal_line_no());
    return 1;
  }
  if (auto_count > 1) {
    printf("%s:%d error:\n\tduplicate 'auto' in storage_class_specifiers\n", input_file_name.c_str(),
           storage_class_specifiers[0]->get_first_terminal_line_no());
    return 1;
  }
  if (register_count > 1) {
    printf("%s:%d error:\n\tduplicate 'register' in storage_class_specifiers\n", input_file_name.c_str(),
           storage_class_specifiers[0]->get_first_terminal_line_no());
    return 1;
  }
  return 0;
}

/* 
  type_qualifier
	: CONST
	| RESTRICT
	| VOLATILE
	| ATOMIC
	;

  */

int check_type_qualifiers(std::vector<std::shared_ptr<ast_node>> &type_qualifiers, bool is_global,
                          std::shared_ptr<tsc_type> type) {

  /* 
    type_qualifier 
    restrict在C99引入用于表示指针所指向的内存只能被这个指针访问到,编译器可以据此做一些优化
    A register variable is a type of local variable.
    It is a hint to store the value in a register for faster access.
    A register variable can not be global or static.
    It can be defined only in a block.

    const const long ->OK
    */

  for (std::shared_ptr<ast_node> type_qualifier : type_qualifiers) {
    switch (type_qualifier->node_sub_type) {
    case NODE_TYPE_TYPE_QUALIFIER_SUBTYPE_CONST:
      type->const_type_qualifier_set = true;
      break;
    case NODE_TYPE_TYPE_QUALIFIER_SUBTYPE_RESTRICT:
      if (is_global) {
        printf("%s:%d error:\n\tglobal variables cannot be restrict in type_qualifiers\n", input_file_name.c_str(),
               type_qualifier->get_first_terminal_line_no());
        return 1;
      }
      type->restrict_type_qualifier_set = true;
      break;
    case NODE_TYPE_TYPE_QUALIFIER_SUBTYPE_VOLATILE:
      type->volatile_type_qualifier_set = true;
      break;
    case NODE_TYPE_TYPE_QUALIFIER_SUBTYPE_ATOMIC:
      printf("%s:%d error:\n\tunsupported C11 '_Atomic' in type_qualifiers\n", input_file_name.c_str(),
             type_qualifier->get_first_terminal_line_no());
      return 1;
    }
  }

  return 0;
}

/*
type_specifier
	: VOID
	| CHAR
	| SHORT
	| INT
	| LONG
	| FLOAT
	| DOUBLE
	| SIGNED
	| UNSIGNED
	| BOOL
	| COMPLEX
	| IMAGINARY	  	
	| atomic_type_specifier
	| struct_or_union_specifier
	| enum_specifier
	| TYPEDEF_NAME		
	;
*/

int check_primitive_type_specifiers(std::vector<std::shared_ptr<ast_node>> &type_specifiers, bool is_global,
                                    std::shared_ptr<tsc_type> type) {
  /*
    double long ->OK
    long short ->error
    short short ->error
    long long long ->error
    unsigned float ->error
    type_specifier规则 
    short与long不能共存.short至多1个long至多2个
    */

  int long_count = 0;
  int short_count = 0;
  int signed_count = 0;
  int unsigned_count = 0;
  int void_char_int_count = 0;
  int float_double_count = 0;
  int struct_union_enum_count = 0;
  int type_id = PRIMITIVE_TYPE_INT;

  for (std::shared_ptr<ast_node> type_specifier : type_specifiers) {

    switch (type_specifier->node_sub_type) {
    case NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_VOID:
      type_id = PRIMITIVE_TYPE_VOID;
      void_char_int_count++;
      break;
    case NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_CHAR:
      type_id = PRIMITIVE_TYPE_CHAR;
      void_char_int_count++;
      break;
    case NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_SHORT:
      void_char_int_count++;
      short_count++;
      break;
    case NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_INT:
      type_id = PRIMITIVE_TYPE_INT;
      void_char_int_count++;
      break;
    case NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_LONG:
      long_count++;
      break;
    case NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_FLOAT:
      type_id = PRIMITIVE_TYPE_FLOAT;
      float_double_count++;
      break;
    case NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_DOUBLE:
      type_id = PRIMITIVE_TYPE_DOUBLE;
      float_double_count++;
      break;
    case NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_SIGNED:
      signed_count++;
      break;
    case NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_UNSIGNED:
      unsigned_count++;
      break;
    case NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_BOOL:
      printf("%s:%d error:\n\tunsupported C99 '_Bool' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_COMPLEX:
      printf("%s:%d error:\n\tunsupported C99 '_Complex' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_IMAGINARY:
      printf("%s:%d error:\n\tunsupported C99 '_Imagnary' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_ATOMIC_TYPE_SPECIFIER:
      printf("%s:%d error:\n\tunsupported C11 '_Atomic' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_STRUCT_OR_UNION_SPECIFIER:
      type_id = RECORD_TYPE_STRUCT_OR_UNION;
      break;
    case NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_ENUM_SPECIFIER:
      type_id = RECORD_TYPE_ENUM;
      struct_union_enum_count++;
      break;
    case NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_TYPEDEF_NAME:
    default:
      break;
    }
  }

  //全局变量default int
  if (is_global && type_specifiers.empty()) {
    printf("%s:%d warning\n\tdeclaration specifiers default to int\n", input_file_name.c_str(),
           type_specifiers[0]->get_first_terminal_line_no());
    type_id = PRIMITIVE_TYPE_INT;
  }

  if (signed_count > 0 && unsigned_count > 0) {
    printf("%s:%d error:\n\tboth 'signed' and 'unsigned' in declaration specifiers\n", input_file_name.c_str(),
           type_specifiers[0]->get_first_terminal_line_no());
    return 1;
  }

  if (long_count > 0 && short_count > 0) {
    printf("%s:%d error:\n\tboth 'long' and 'short' in declaration specifiers\n", input_file_name.c_str(),
           type_specifiers[0]->get_first_terminal_line_no());
    return 1;
  }

  // too 'long'
  if (long_count > 2) {
    printf("%s:%d error:\n\ttoo 'long' in declaration specifiers\n", input_file_name.c_str(),
           type_specifiers[0]->get_first_terminal_line_no());
    return 1;
  }

  //duplicate 'short'
  if (short_count > 1) {
    printf("%s:%d error:\n\tduplicate 'short' in declaration specifiers\n", input_file_name.c_str(),
           type_specifiers[0]->get_first_terminal_line_no());
    return 1;
  }

  // duplicate 'signed'
  if (signed_count > 1) {
    printf("%s:%d error:\n\tduplicate 'signed' in declaration specifiers\n", input_file_name.c_str(),
           type_specifiers[0]->get_first_terminal_line_no());
    return 1;
  }

  // duplicate 'unsigned'
  if (unsigned_count > 1) {
    printf("%s:%d error:\n\tduplicate 'unsigned' in declaration specifiers\n", input_file_name.c_str(),
           type_specifiers[0]->get_first_terminal_line_no());
    return 1;
  }

  // duplicate 'void_char_int'
  if (void_char_int_count > 1) {
    printf("%s:%d error:\n\tduplicate 'void_char_int' in declaration specifiers\n", input_file_name.c_str(),
           type_specifiers[0]->get_first_terminal_line_no());
    return 1;
  }
  // duplicate 'float_double'
  if (float_double_count > 1) {
    printf("%s:%d error:\n\tduplicate 'float_double' in declaration specifiers\n", input_file_name.c_str(),
           type_specifiers[0]->get_first_terminal_line_no());
    return 1;
  }
  // duplicate 'struct_union_enum'
  if (struct_union_enum_count > 1) {
    printf("%s:%d error:\n\tduplicate 'struct_union_enum' in declaration specifiers\n", input_file_name.c_str(),
           type_specifiers[0]->get_first_terminal_line_no());
    return 1;
  }

  if (void_char_int_count + float_double_count + struct_union_enum_count > 1) {
    printf("%s:%d error:\n\tduplicate 'void_char_int' or 'float_double' or 'struct_union_enum' in "
           "declaration specifiers\n",
           input_file_name.c_str(), type_specifiers[0]->get_first_terminal_line_no());
    return 1;
  }

  //signed_count=1
  if (signed_count) {
    switch (type_id) {
    case PRIMITIVE_TYPE_VOID:
      printf("%s:%d error:\n\t'signed' and 'void' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case PRIMITIVE_TYPE_CHAR:
      break;
    case PRIMITIVE_TYPE_SHORT:
      break;
    case PRIMITIVE_TYPE_INT:
      break;
    case PRIMITIVE_TYPE_LONG:
      break;
    case PRIMITIVE_TYPE_FLOAT:
      printf("%s:%d error:\n\t'signed' and 'float' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case PRIMITIVE_TYPE_DOUBLE:
      printf("%s:%d error:\n\t'signed' and 'double' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case RECORD_TYPE_STRUCT_OR_UNION:
      printf("%s:%d error:\n\t'signed' and 'struct_or_union' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case RECORD_TYPE_ENUM:

      printf("%s:%d error:\n\t'signed' and 'enum' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    default:
      break;
    }
  }

  //unsigned_count=1
  if (unsigned_count) {
    switch (type_id) {
    case PRIMITIVE_TYPE_VOID:
      printf("%s:%d error:\n\t'unsigned' and 'void' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case PRIMITIVE_TYPE_CHAR:
      type_id = PRIMITIVE_TYPE_UNSIGNED_CHAR;
      break;
    case PRIMITIVE_TYPE_SHORT:
      type_id = PRIMITIVE_TYPE_UNSIGNED_SHORT;
      break;
    case PRIMITIVE_TYPE_INT:
      type_id = PRIMITIVE_TYPE_UNSIGNED_INT;
      break;
    case PRIMITIVE_TYPE_LONG:
      type_id = PRIMITIVE_TYPE_UNSIGNED_LONG;
      break;
    case PRIMITIVE_TYPE_FLOAT:
      printf("%s:%d error:\n\t'unsigned' and 'float' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case PRIMITIVE_TYPE_DOUBLE:
      printf("%s:%d error:\n\t'unsigned' and 'double' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case RECORD_TYPE_STRUCT_OR_UNION:
      printf("%s:%d error:\n\t'unsigned' and 'struct_or_union' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case RECORD_TYPE_ENUM:

      printf("%s:%d error:\n\t'unsigned' and 'enum' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    default:
      break;
    }
  }

  // long double->Ok short double->error
  // 上面已经将基本的数值类型分类,还需要处理long double, long long, unsigned long long的问题

  //short_count=1
  if (short_count) {
    switch (type_id) {
    case PRIMITIVE_TYPE_VOID:
      printf("%s:%d error:\n\t'short' and 'void' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case PRIMITIVE_TYPE_CHAR:
    case PRIMITIVE_TYPE_UNSIGNED_CHAR:
      printf("%s:%d error:\n\t'short' and 'char' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case PRIMITIVE_TYPE_SHORT:
    case PRIMITIVE_TYPE_UNSIGNED_SHORT:
      break;
    case PRIMITIVE_TYPE_INT:
      type_id = PRIMITIVE_TYPE_SHORT;
      break;
    case PRIMITIVE_TYPE_UNSIGNED_INT:
      type_id = PRIMITIVE_TYPE_UNSIGNED_SHORT;
      break;
    case PRIMITIVE_TYPE_LONG:
    case PRIMITIVE_TYPE_UNSIGNED_LONG:
      // should not happen
      break;
    case PRIMITIVE_TYPE_FLOAT:
      printf("%s:%d error:\n\t'short' and 'float' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case PRIMITIVE_TYPE_DOUBLE:
      printf("%s:%d error:\n\t'short' and 'double' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case RECORD_TYPE_STRUCT_OR_UNION:
      printf("%s:%d error:\n\t'short' and 'struct_or_union' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case RECORD_TYPE_ENUM:
      printf("%s:%d error:\n\t'short' and 'enum' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    default:
      break;
    }
  }
  // long_count=1 or 2
  if (long_count) {
    switch (type_id) {
    case PRIMITIVE_TYPE_VOID:
      printf("%s:%d error:\n\t'long' and 'void' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case PRIMITIVE_TYPE_CHAR:
    case PRIMITIVE_TYPE_UNSIGNED_CHAR:
      printf("%s:%d error:\n\t'long' and 'char' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case PRIMITIVE_TYPE_SHORT:
    case PRIMITIVE_TYPE_UNSIGNED_SHORT:
      //should not happen
      break;
    case PRIMITIVE_TYPE_INT:
    case PRIMITIVE_TYPE_LONG:
      if (long_count == 1)
        type_id = PRIMITIVE_TYPE_LONG;
      else
        type_id = PRIMITIVE_TYPE_LONG_LONG;
      break;
    case PRIMITIVE_TYPE_UNSIGNED_INT:
    case PRIMITIVE_TYPE_UNSIGNED_LONG:
      if (long_count == 1)
        type_id = PRIMITIVE_TYPE_UNSIGNED_LONG;
      else
        type_id = PRIMITIVE_TYPE_UNSIGNED_LONG_LONG;
      break;

    case PRIMITIVE_TYPE_FLOAT:
      printf("%s:%d error:\n\t'long' and 'float' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case PRIMITIVE_TYPE_DOUBLE:
      if (long_count >= 2) {
        printf("%s:%d error:\n\t'long long' and 'double' in declaration specifiers\n", input_file_name.c_str(),
               type_specifiers[0]->get_first_terminal_line_no());
        return 1;
      }
      type_id = PRIMITIVE_TYPE_LONG_DOUBLE;
      break;
    case RECORD_TYPE_STRUCT_OR_UNION:
      printf("%s:%d error:\n\t'long' and 'struct_or_union' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    case RECORD_TYPE_ENUM:
      printf("%s:%d error:\n\t'long' and 'enum' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    default:
      break;
    }
  }
  type->type_id = type_id;
  return 0;
}

/*
enum_specifier
	: ENUM '{' enumerator_list '}'
	| ENUM '{' enumerator_list ',' '}'
	| ENUM IDENTIFIER '{' enumerator_list '}'
	| ENUM IDENTIFIER '{' enumerator_list ',' '}'
	| ENUM IDENTIFIER
	;
*/

int analyze_enum_specifier(std::shared_ptr<ast_node> enum_specifier, semantics_analysis_context &context,
                           std::shared_ptr<tsc_type> type, bool is_global) {

  std::shared_ptr<ast_node> identifier_node; // enum A{...}; identifier=A
  std::shared_ptr<ast_node> enumerator_list;

  // printf("node_sub_type %d size %lu\n", enum_specifier->node_sub_type, enum_specifier->items.size());
  switch (enum_specifier->node_sub_type) {
  case NODE_TYPE_ENUM_SPECIFIER_SUBTYPE_ENUM_LEFT_BRACE_ENUMATOR_LIST_RIGHT_BRACE:
  case NODE_TYPE_ENUM_SPECIFIER_SUBTYPE_ENUM_LEFT_BRACE_ENUMATOR_LIST_COMMA_RIGHT_BRACE:
    enumerator_list = enum_specifier->items[2];
    type->is_complete = true;
    break;

  case NODE_TYPE_ENUM_SPECIFIER_SUBTYPE_ENUM_IDENTIFER_LEFT_BRACE_ENUMATOR_LIST_RIGHT_BRACE:
  case NODE_TYPE_ENUM_SPECIFIER_SUBTYPE_ENUM_IDENTIFER_LEFT_BRACE_ENUMATOR_LIST_COMMA_RIGHT_BRACE:
    identifier_node = enum_specifier->items[1];
    enumerator_list = enum_specifier->items[3];
    type->is_complete = true;
    break;

  case NODE_TYPE_ENUM_SPECIFIER_SUBTYPE_ENUM_IDENTIFER:
    identifier_node = enum_specifier->items[1];
    type->is_complete = false;
    break;
  }

  std::shared_ptr<symbol_table_node> current_symbol_table_node = context.current_symbol_table_node;
  std::string identifier;

  if (identifier_node) {
    identifier = *identifier_node->lexeme;
    // 检查符号表中是否已经有同名的符号
    for (std::map<std::string, std::shared_ptr<tsc_type>>::iterator it =
             context.current_symbol_table_node->struct_union_enum_names.begin();
         it != context.current_symbol_table_node->struct_union_enum_names.end(); it++) {
      //可以多次声明但是只能定义1次
      if (it->first == identifier && !check_type_compatibility(it->second, type)) {
        printf("%s:%d error:\n\tincorrect tag '%s'\n", input_file_name.c_str(),
               identifier_node->get_first_terminal_line_no(), identifier.c_str());
        return 1;
      }
    }
    //校验无误,加入tags
    context.current_symbol_table_node->struct_union_enum_names[identifier] = type;
  }

  if (enumerator_list)
    return analyze_enumerator_list(enumerator_list, context, type, is_global);
  else
    return 0;
}

int analyze_struct_or_union_specifier(std::shared_ptr<ast_node> struct_or_union_specifier,
                                      semantics_analysis_context &context, std::shared_ptr<tsc_type> type,
                                      bool is_global) {
  return 0;
}
bool check_type_compatibility(std::shared_ptr<tsc_type> type1, std::shared_ptr<tsc_type> type2) {
  if (type1->type_id != type2->type_id)
    return false;
  //至少1个是声明
  if (!type1->is_complete || !type2->is_complete)
    return true;
  return false;
}

/*
enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;

enumerator	
	: enumeration_constant '=' constant_expression
	| enumeration_constant
	;
*/

int analyze_enumerator_list(std::shared_ptr<ast_node> enumerator_list, semantics_analysis_context &context,
                            std::shared_ptr<tsc_type> type, bool is_global) {

  std::vector<std::shared_ptr<ast_node>> enumerators;
  std::shared_ptr<ast_node> node = enumerator_list;

  while (node->node_type == NODE_TYPE_ENUMERATOR_LIST &&
         node->node_sub_type == NODE_TYPE_ENUMERATOR_LIST_SUBTYPE_ENUMERATOR_LIST_COMMA_ENUMERATOR) {
    enumerators.push_back(node->items[2]);
    node = node->items[0];
  }
  enumerators.push_back(node->items[0]);
  enumerators = std::vector<std::shared_ptr<ast_node>>(enumerators.rbegin(), enumerators.rend());
  int semantics_analysis_result = 0;
  int next_value = 0;
  // printf("enumerators size %lu\n", enumerators.size());
  for (std::shared_ptr<ast_node> enumerator : enumerators) {
    // printf("analyze_enumerator %d %d\n", enumerator->node_type, enumerator->node_sub_type);
    std::pair<int, int> result = analyze_enumerator(enumerator, context, type, is_global, next_value);
    semantics_analysis_result = result.first;
    next_value = result.second;
    if (semantics_analysis_result)
      return semantics_analysis_result;
  }
  return 0;
}

std::pair<int, int> analyze_enumerator(std::shared_ptr<ast_node> enumerator, semantics_analysis_context &context,
                                       std::shared_ptr<tsc_type> type, bool is_global, int next_value) {
  int semantics_analysis_result = 0;
  int enumration_constant_value = next_value;
  // printf("enumerator size %lu\n", enumerator->items.size());
  std::shared_ptr<ast_node> enumeration_constant = enumerator->items[0];
  std::shared_ptr<ast_node> constant_expression;
  //  printf("enumeration_constant size %lu\n", enumeration_constant->items.size());
  std::string identifier = *enumeration_constant->items[0]->lexeme;
  //  printf("enumeration_constant identifier %s\n", identifier.c_str());
  // 检查符号表中是否已经有同名的符号
  for (std::map<std::string, std::shared_ptr<tsc_type>>::iterator it =
           context.current_symbol_table_node->identifier_and_types.begin();
       it != context.current_symbol_table_node->identifier_and_types.end(); it++) {
    if (it->first == identifier) {
      printf("%s:%d error:\n\rredeclared '%s'\n", input_file_name.c_str(),
             enumeration_constant->get_first_terminal_line_no(), identifier.c_str());
      return std::make_pair<int, int>(1, 0);
    }
  }
  //校验无误,加入tags
  context.current_symbol_table_node->identifier_and_types[identifier] = type;
  switch (enumerator->node_sub_type) {
  case NODE_TYPE_ENUMERATOR_SUBTYPE_ENUMERATION_CONSTANT_ASSIGN_CONSTANT_EXPRESSION:
    constant_expression = enumerator->items[2];
    break;
  case NODE_TYPE_ENUMERATOR_SUBTYPE_ENUMERATION_CONSTANT:
    break;
  }

  return std::make_pair<int, int>(std::move(semantics_analysis_result), std::move(enumration_constant_value));
}
