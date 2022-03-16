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
  translation_unit->sub_nodes = external_declarations;
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
  for (std::shared_ptr<ast_node> enumerator : enumerators) {
    int calculated_enumeration_constant_value;
    semantics_analysis_result =
        analyze_enumerator(enumerator, context, type, is_global, next_value, calculated_enumeration_constant_value);
    next_value = calculated_enumeration_constant_value + 1;
    if (semantics_analysis_result)
      return semantics_analysis_result;
  }
  return 0;
}

int analyze_enumerator(std::shared_ptr<ast_node> enumerator, semantics_analysis_context &context,
                       std::shared_ptr<tsc_type> type, bool is_global, int next_value,
                       int &calculated_enumeration_constant_value) {
  int semantics_analysis_result = 0;
  int enumeration_constant_value = next_value;
  std::shared_ptr<ast_node> enumeration_constant = enumerator->items[0];
  std::shared_ptr<ast_node> constant_expression;
  std::shared_ptr<std::string> identifier = enumeration_constant->items[0]->lexeme;
  // 检查符号表中是否已经有同名的符号
  for (std::map<std::string, std::shared_ptr<tsc_symbol>>::iterator it =
           context.current_symbol_table_node->identifier_and_symbols.begin();
       it != context.current_symbol_table_node->identifier_and_symbols.end(); it++) {
    if (it->first == *identifier) {
      printf("%s:%d error:\n\rredeclared '%s'\n", input_file_name.c_str(),
             enumeration_constant->get_first_terminal_line_no(), identifier->c_str());
      return 1;
    }
  }
  //校验无误,加入tags

  std::shared_ptr<tsc_symbol> symbol = std::make_shared<tsc_symbol>();
  symbol->type = type;
  symbol->identifier = identifier;
  symbol->symbol_type = SYMBOL_TYPE_ENUMERATION_CONSTANT;
  context.current_symbol_table_node->identifier_and_symbols[*identifier] = symbol;
  switch (enumerator->node_sub_type) {
  case NODE_TYPE_ENUMERATOR_SUBTYPE_ENUMERATION_CONSTANT_ASSIGN_CONSTANT_EXPRESSION:
    constant_expression = enumerator->items[2];
    semantics_analysis_result = analyze_constant_expression(constant_expression, context);
    break;
  case NODE_TYPE_ENUMERATOR_SUBTYPE_ENUMERATION_CONSTANT:
    break;
  }

  return semantics_analysis_result;
}

int analyze_constant_expression(std::shared_ptr<ast_node> constant_expression, semantics_analysis_context &context) {
  std::shared_ptr<ast_node> conditional_expression = constant_expression->items[0];
  int semantics_analysis_result = analyze_conditional_expression(conditional_expression, context);

  if (semantics_analysis_result)
    return semantics_analysis_result;

  // check is constant expression

  return 0;
}

/*
conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression
	;
 */
int analyze_conditional_expression(std::shared_ptr<ast_node> conditional_expression,
                                   semantics_analysis_context &context) {
  int semantics_analysis_result = 0;
  std::shared_ptr<ast_node> logical_or_expression = conditional_expression->items[0];
  std::shared_ptr<ast_node> expression;
  std::shared_ptr<ast_node> next_conditional_expression;

  switch (conditional_expression->node_sub_type) {
  case NODE_TYPE_CONDITIONAL_EXPRESSION_SUBTYPE_LOGICAL_OR_EXPRESSION:
    semantics_analysis_result = analyze_logical_or_expression(logical_or_expression, context);
    break;
  case NODE_TYPE_ASSIGNMENT_EXPRESSION_SUBTYPE_UNARY_EXPRESSION_ASSIGNMENT_OPERATOR_ASSIGNMENT_EXPRESSION:
    expression = conditional_expression->items[2];
    next_conditional_expression = conditional_expression->items[4];
    semantics_analysis_result = analyze_expression(expression, context);

    if (semantics_analysis_result)
      return semantics_analysis_result;
    semantics_analysis_result = analyze_conditional_expression(next_conditional_expression, context);

    if (semantics_analysis_result)
      return semantics_analysis_result;
    break;
  }

  return semantics_analysis_result;
}

/*
logical_or_expression
	: logical_and_expression
	| logical_or_expression OR_OP logical_and_expression
	;

 */
int analyze_logical_or_expression(std::shared_ptr<ast_node> logical_or_expression,
                                  semantics_analysis_context &context) {
  int semantics_analysis_result = 0;

  std::shared_ptr<ast_node> logical_and_expression;
  std::shared_ptr<ast_node> next_logical_or_expression;

  switch (logical_or_expression->node_sub_type) {
  case NODE_TYPE_LOGICAL_OR_EXPRESSION_SUBTYPE_LOGICAL_AND_EXPRESSION:
    logical_and_expression = logical_or_expression->items[0];

    break;
  case NODE_TYPE_LOGICAL_OR_EXPRESSION_SUBTYPE_LOGICAL_OR_EXPRESSION_OR_OP_LOGICAL_AND_EXPRESSION:

    next_logical_or_expression = logical_or_expression->items[0];
    logical_and_expression = logical_or_expression->items[2];

    break;
  }

  semantics_analysis_result = analyze_logical_and_expression(logical_and_expression, context);
  if (semantics_analysis_result)
    return semantics_analysis_result;

  if (next_logical_or_expression)
    semantics_analysis_result = analyze_logical_or_expression(next_logical_or_expression, context);

  return semantics_analysis_result;
}

/*
logical_and_expression
	: inclusive_or_expression
	| logical_and_expression AND_OP inclusive_or_expression
	;
 */
int analyze_logical_and_expression(std::shared_ptr<ast_node> logical_and_expression,
                                   semantics_analysis_context &context) {
  int semantics_analysis_result = 0;

  std::shared_ptr<ast_node> inclusive_or_expression;
  std::shared_ptr<ast_node> next_logical_and_expression;

  switch (logical_and_expression->node_sub_type) {
  case NODE_TYPE_LOGICAL_AND_EXPRESSION_SUBTYPE_INCLUSIVE_OR_EXPRESSION:
    inclusive_or_expression = logical_and_expression->items[0];
    break;
  case NODE_TYPE_LOGICAL_AND_EXPRESSION_SUBTYPE_LOGICAL_AND_EXPRESSION_AND_OP_INCLUSIVE_OR_EXPRESSION:
    next_logical_and_expression = logical_and_expression->items[0];
    inclusive_or_expression = logical_and_expression->items[2];

    break;
  }
  semantics_analysis_result = analyze_inclusive_or_expression(inclusive_or_expression, context);
  if (semantics_analysis_result)
    return semantics_analysis_result;
  if (next_logical_and_expression)
    semantics_analysis_result = analyze_logical_and_expression(next_logical_and_expression, context);

  return semantics_analysis_result;
}

/*
inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression
	;
 */
int analyze_inclusive_or_expression(std::shared_ptr<ast_node> inclusive_or_expression,
                                    semantics_analysis_context &context) {
  int semantics_analysis_result = 0;

  std::shared_ptr<ast_node> exclusive_or_expression = inclusive_or_expression->items[0];
  std::shared_ptr<ast_node> next_inclusive_or_expression;

  switch (inclusive_or_expression->node_sub_type) {
  case NODE_TYPE_INCLUSIVE_OR_EXPRESSION_SUBTYPE_EXCLUSIVE_OR_EXPRESSION:
    exclusive_or_expression = inclusive_or_expression->items[0];
    break;
  case NODE_TYPE_INCLUSIVE_OR_EXPRESSION_SUBTYPE_INCLUSIVE_OR_EXPRESSION_BITOR_EXCLUSIVE_OR_EXPRESSION:
    exclusive_or_expression = inclusive_or_expression->items[2];
    next_inclusive_or_expression = inclusive_or_expression->items[0];

    break;
  }

  semantics_analysis_result = analyze_exclusive_or_expression(exclusive_or_expression, context);
  if (semantics_analysis_result)
    return semantics_analysis_result;
  if (next_inclusive_or_expression)
    semantics_analysis_result = analyze_inclusive_or_expression(next_inclusive_or_expression, context);

  return semantics_analysis_result;
}

/*
exclusive_or_expression
	: and_expression
	| exclusive_or_expression '^' and_expression
	;
 */
int analyze_exclusive_or_expression(std::shared_ptr<ast_node> exclusive_or_expression,
                                    semantics_analysis_context &context) {
  int semantics_analysis_result = 0;

  std::shared_ptr<ast_node> and_expression = exclusive_or_expression->items[0];
  std::shared_ptr<ast_node> next_exclusive_or_expression;

  switch (exclusive_or_expression->node_sub_type) {
  case NODE_TYPE_EXCLUSIVE_OR_EXPRESSION_SUBTYPE_AND_EXPRESSION:
    and_expression = exclusive_or_expression->items[0];
    break;
  case NODE_TYPE_EXCLUSIVE_OR_EXPRESSION_SUBTYPE_EXCLUSIVE_OR_EXPRESSION_BITXOR_AND_EXPRESSION:
    and_expression = exclusive_or_expression->items[2];
    next_exclusive_or_expression = exclusive_or_expression->items[0];
    break;
  }

  semantics_analysis_result = analyze_and_expression(and_expression, context);
  if (semantics_analysis_result)
    return semantics_analysis_result;

  if (next_exclusive_or_expression)
    semantics_analysis_result = analyze_exclusive_or_expression(next_exclusive_or_expression, context);

  return semantics_analysis_result;
}

/*
and_expression
	: equality_expression
	| and_expression '&' equality_expression
	;
 */
int analyze_and_expression(std::shared_ptr<ast_node> and_expression, semantics_analysis_context &context) {
  int semantics_analysis_result = 0;

  std::shared_ptr<ast_node> equality_expression;
  std::shared_ptr<ast_node> next_and_expression;

  switch (and_expression->node_sub_type) {
  case NODE_TYPE_AND_EXPRESSION_SUBTYPE_EQUALITY_EXPRESSION:
    equality_expression = and_expression->items[0];
    break;
  case NODE_TYPE_AND_EXPRESSION_SUBTYPE_AND_EXPRESSION_BITAND_EQUALITY_EXPRESSION:
    equality_expression = and_expression->items[2];
    next_and_expression = and_expression->items[0];

    break;
  }
  semantics_analysis_result = analyze_equality_expression(equality_expression, context);
  if (semantics_analysis_result)
    return semantics_analysis_result;
  if (next_and_expression)
    semantics_analysis_result = analyze_and_expression(next_and_expression, context);

  return semantics_analysis_result;
}

/*
equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression
	| equality_expression NE_OP relational_expression
	;
 */
int analyze_equality_expression(std::shared_ptr<ast_node> equality_expression, semantics_analysis_context &context) {
  int semantics_analysis_result = 0;

  std::shared_ptr<ast_node> relational_expression;
  std::shared_ptr<ast_node> next_equality_expression;

  switch (equality_expression->node_sub_type) {
  case NODE_TYPE_EQUALITY_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION:
    relational_expression = equality_expression->items[0];
    break;
  case NODE_TYPE_EQUALITY_EXPRESSION_SUBTYPE_EQUALITY_EXPRESSION_EQ_OP_RELATIONAL_EXPRESSION:
  case NODE_TYPE_EQUALITY_EXPRESSION_SUBTYPE_EQUALITY_EXPRESSION_NE_OP_RELATIONAL_EXPRESSION:
    relational_expression = equality_expression->items[2];
    next_equality_expression = equality_expression->items[0];
    break;
  }

  semantics_analysis_result = analyze_relational_expression(relational_expression, context);
  if (semantics_analysis_result)
    return semantics_analysis_result;
  if (next_equality_expression)
    semantics_analysis_result = analyze_equality_expression(next_equality_expression, context);

  return semantics_analysis_result;
}

/*
relational_expression
	: shift_expression
	| relational_expression '<' shift_expression
	| relational_expression '>' shift_expression
	| relational_expression LE_OP shift_expression
	| relational_expression GE_OP shift_expression
	;
 */
int analyze_relational_expression(std::shared_ptr<ast_node> relational_expression,
                                  semantics_analysis_context &context) {
  int semantics_analysis_result = 0;

  std::shared_ptr<ast_node> shift_expression = relational_expression->items[0];
  std::shared_ptr<ast_node> next_relational_expression;

  switch (relational_expression->node_sub_type) {
  case NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_SHIFT_EXPRESSION:
    shift_expression = relational_expression->items[0];
    break;
  case NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_LESS_THAN_SHIFT_EXPRESSION:
  case NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_GREATER_THAN_SHIFT_EXPRESSION:
  case NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_LE_OP_SHIFT_EXPRESSION:
  case NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_GE_OP_SHIFT_EXPRESSION:
    shift_expression = relational_expression->items[2];
    next_relational_expression = relational_expression->items[0];

    break;
  }
  semantics_analysis_result = analyze_shift_expression(shift_expression, context);
  if (semantics_analysis_result)
    return semantics_analysis_result;

  if (next_relational_expression)
    semantics_analysis_result = analyze_relational_expression(next_relational_expression, context);

  return semantics_analysis_result;
}

/*
shift_expression
	: additive_expression
	| shift_expression LEFT_OP additive_expression
	| shift_expression RIGHT_OP additive_expression
	;
 */
int analyze_shift_expression(std::shared_ptr<ast_node> shift_expression, semantics_analysis_context &context) {
  int semantics_analysis_result = 0;

  std::shared_ptr<ast_node> additive_expression;
  std::shared_ptr<ast_node> next_shift_expression;

  switch (shift_expression->node_sub_type) {
  case NODE_TYPE_SHIFT_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION:
    additive_expression = shift_expression->items[0];
    break;
  case NODE_TYPE_SHIFT_EXPRESSION_SUBTYPE_SHIFT_EXPRESSION_LEFT_SHIFT_ADDITIVE_EXPRESSION:
  case NODE_TYPE_SHIFT_EXPRESSION_SUBTYPE_SHIFT_EXPRESSION_RIGHT_SHIFT_ADDITIVE_EXPRESSION:
    additive_expression = shift_expression->items[2];
    next_shift_expression = shift_expression->items[0];

    break;
  }
  semantics_analysis_result = analyze_additive_expression(additive_expression, context);
  if (semantics_analysis_result)
    return semantics_analysis_result;
  if (next_shift_expression)
    semantics_analysis_result = analyze_shift_expression(next_shift_expression, context);

  return semantics_analysis_result;
}

/*
additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression
	| additive_expression '-' multiplicative_expression
	;
 */
int analyze_additive_expression(std::shared_ptr<ast_node> additive_expression, semantics_analysis_context &context) {
  int semantics_analysis_result = 0;

  std::shared_ptr<ast_node> multiplicative_expression;
  std::shared_ptr<ast_node> next_additive_expression;

  switch (additive_expression->node_sub_type) {
  case NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION:
    multiplicative_expression = additive_expression->items[0];
    break;
  case NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION_ADD_MULTIPLICATIVE_EXPRESSION:
  case NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION_SUB_MULTIPLICATIVE_EXPRESSION:
    additive_expression = additive_expression->items[2];
    next_additive_expression = additive_expression->items[0];
    break;
  }
  semantics_analysis_result = analyze_multiplicative_expression(multiplicative_expression, context);
  if (semantics_analysis_result)
    return semantics_analysis_result;
  if (next_additive_expression)
    semantics_analysis_result = analyze_additive_expression(next_additive_expression, context);

  return semantics_analysis_result;
}

/*
multiplicative_expression
	: cast_expression
	| multiplicative_expression '*' cast_expression
	| multiplicative_expression '/' cast_expression
	| multiplicative_expression '%' cast_expression
	;
 */
int analyze_multiplicative_expression(std::shared_ptr<ast_node> multiplicative_expression,
                                      semantics_analysis_context &context) {
  int semantics_analysis_result = 0;

  std::shared_ptr<ast_node> cast_expression;
  std::shared_ptr<ast_node> next_multiplicative_expression;

  switch (multiplicative_expression->node_sub_type) {
  case NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_CAST_EXPRESSION:
    cast_expression = multiplicative_expression->items[0];
    break;
  case NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION_MUL_CAST_EXPRESSION:
  case NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION_DIV_CAST_EXPRESSION:
  case NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION_MOD_CAST_EXPRESSION:

    cast_expression = multiplicative_expression->items[2];
    next_multiplicative_expression = multiplicative_expression->items[0];
    break;
  }
  semantics_analysis_result = analyze_cast_expression(cast_expression, context);
  if (semantics_analysis_result)
    return semantics_analysis_result;
  if (next_multiplicative_expression)
    semantics_analysis_result = analyze_multiplicative_expression(next_multiplicative_expression, context);

  return semantics_analysis_result;
}

/*
cast_expression
	: unary_expression
	| '(' type_name ')' cast_expression
	;
 */
int analyze_cast_expression(std::shared_ptr<ast_node> cast_expression, semantics_analysis_context &context) {
  int semantics_analysis_result = 0;

  std::shared_ptr<ast_node> unary_expression;
  std::shared_ptr<ast_node> type_name;
  std::shared_ptr<ast_node> next_cast_expression;

  switch (cast_expression->node_sub_type) {
  case NODE_TYPE_CAST_EXPRESSION_SUBTYPE_UNARY_EXPRESSION:
    unary_expression = cast_expression->items[0];
    break;
  case NODE_TYPE_CAST_EXPRESSION_SUBTYPE_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS_CAST_EXPRESSION:
    type_name = cast_expression->items[1];
    next_cast_expression = cast_expression->items[3];
    break;
  }
  semantics_analysis_result = analyze_unary_expression(unary_expression, context);
  if (semantics_analysis_result)
    return semantics_analysis_result;

  std::shared_ptr<tsc_type> type_out;
  if (type_name) {
    type_out = std::make_shared<tsc_type>();
    semantics_analysis_result = analyze_type_name(type_name, context, type_out);
  }

  if (semantics_analysis_result)
    return semantics_analysis_result;

  if (next_cast_expression)
    semantics_analysis_result = analyze_cast_expression(next_cast_expression, context);

  return semantics_analysis_result;
}

/*
unary_expression
	: postfix_expression
	| INC_OP unary_expression
	| DEC_OP unary_expression
	| unary_operator cast_expression
	| SIZEOF unary_expression
	| SIZEOF '(' type_name ')'
	| ALIGNOF '(' type_name ')'
	;
 */
int analyze_unary_expression(std::shared_ptr<ast_node> unary_expression, semantics_analysis_context &context) {
  int semantics_analysis_result = 0;

  std::shared_ptr<ast_node> postfix_expression;
  std::shared_ptr<ast_node> next_unary_expression;
  std::shared_ptr<ast_node> type_name;
  std::shared_ptr<ast_node> cast_expression;
  switch (unary_expression->node_sub_type) {
  case NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION:
    postfix_expression = unary_expression->items[0];
    break;
  case NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_INC_OP_UNARY_EXPRESSION:
  case NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_DEC_OP_UNARY_EXPRESSION:
    next_unary_expression = unary_expression->items[1];
    break;
  case NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_UNARY_OPERATOR_CAST_EXPRESSION:
    cast_expression = unary_expression->items[1];
    break;
  case NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_SIZEOF_UNARY_EXPRESSION:
    next_unary_expression = unary_expression->items[1];
    break;
  case NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_SIZEOF_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS:
    type_name = unary_expression->items[2];
    break;
  case NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_ALIGNOF_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS:
    printf("%s:%d error:\n\tunsupported C11 '_Alignof' in unary_expression\n", input_file_name.c_str(),
           unary_expression->get_first_terminal_line_no());
    return 1;
  }
  std::shared_ptr<tsc_type> type_out;
  if (type_name) {
    type_out = std::make_shared<tsc_type>();
    semantics_analysis_result = analyze_type_name(type_name, context, type_out);
  }

  if (semantics_analysis_result)
    return semantics_analysis_result;

  if (postfix_expression) {
    semantics_analysis_result = analyze_postfix_expression(postfix_expression, context);
  }
  if (semantics_analysis_result)
    return semantics_analysis_result;

  return semantics_analysis_result;
}

/*
type_name
	: specifier_qualifier_list abstract_declarator
	| specifier_qualifier_list
	;
 */
int analyze_type_name(std::shared_ptr<ast_node> type_name, semantics_analysis_context &context,
                      std::shared_ptr<tsc_type> out_type) {
  int semantics_analysis_result = 0;
  return semantics_analysis_result;
}

/*
postfix_expression
	: primary_expression
	| postfix_expression '[' expression ']'
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	| postfix_expression '.' IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP
	| postfix_expression DEC_OP
	| '(' type_name ')' '{' initializer_list '}'
	| '(' type_name ')' '{' initializer_list ',' '}'
	;
 */
int analyze_postfix_expression(std::shared_ptr<ast_node> postfix_expression, semantics_analysis_context &context) {
  int semantics_analysis_result = 0;
  std::shared_ptr<ast_node> primary_expression;
  std::shared_ptr<ast_node> next_postfix_expression;

  switch (postfix_expression->node_sub_type) {
  case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_PRIMARY_EXPRESSION:
    primary_expression = postfix_expression->items[0];
    break;
  case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_LEFT_BRACKET_EXPRESSION_RIGHT_BRACKET:
  case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_LEFT_PARENTHESIS_RIGHT_PARENTHESIS:
  case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_LEFT_PARENTHESIS_ARGUMENT_EXPRESSION_LIST_RIGHT_PARENTHESIS:
  case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_DOT_IDENTIFIER:
  case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_PTR_OP_IDENTIFIER:
  case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_INC_OP:
  case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_DEC_OP:
  case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS_LEFT_BRACE_INITIALIZER_LIST_RIGHT_BRACE:
  case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS_LEFT_BRACE_INITIALIZER_LIST_COMMA_RIGHT_BRACE:

    next_postfix_expression = postfix_expression->items[0];
    break;
  }

  if (primary_expression)
    semantics_analysis_result = analyze_primary_expression(primary_expression, context);
  if (semantics_analysis_result)
    return semantics_analysis_result;

  return semantics_analysis_result;
}

/*
primary_expression
	: IDENTIFIER
	| constant
	| string
	| '(' expression ')'
	| generic_selection
	;
 */
int analyze_primary_expression(std::shared_ptr<ast_node> primary_expression, semantics_analysis_context &context) {
  int semantics_analysis_result = 0;
  //这里的identifier可能是函数名
  std::shared_ptr<ast_node> identifier_node;
  std::shared_ptr<ast_node> constant;
  std::shared_ptr<ast_node> string_node;
  std::shared_ptr<ast_node> expression;

  switch (primary_expression->node_sub_type) {
  case NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_IDENTIFIER:
    identifier_node = primary_expression->items[0];
    break;
  case NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_CONSTANT:
    constant = primary_expression->items[0];
    break;

  case NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_STRING: {
    string_node = primary_expression->items[0];
    string_node->symbol = std::make_shared<tsc_symbol>();
    string_node->symbol->type = global_types::composite_type_const_char_star;
    string_node->symbol->value = std::make_shared<expression_value>();
    string_node->symbol->value->string_value = std::make_shared<std::string>();
    semantics_analysis_result = analyze_string(string_node, context, *string_node->symbol->value->string_value);
    if (semantics_analysis_result)
      return semantics_analysis_result;

  }

  break;
  case NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_LEFT_PARENTHESIS_EXPRESSION_RIGHT_PARENTHESIS:
    expression = primary_expression->items[1];
    break;

  case NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_GENERIC_SELECTION:
    printf("%s:%d error:\n\tunsupported C11 'generic selection' in primary_expression\n", input_file_name.c_str(),
           primary_expression->get_first_terminal_line_no());
    return 1;
  }

  if (constant) {
    semantics_analysis_result = analyze_constant(constant, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    primary_expression->symbol = constant->symbol;
  }

  // int a[2,3]; ->syntax error condition_expression必须含有括号才可能含有逗号表达式
  // int a[(2,3)]; ->error: variably modified 'a' at file scope 含有逗号那就不是constant expression
  if (expression) {
    semantics_analysis_result = analyze_expression(expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    //这里要额外检验expression是否含有多个assignment_expression 是则修改symbol_type;
    primary_expression->symbol = std::make_shared<tsc_symbol>(*expression->symbol);
    if (expression->sub_nodes.size() > 1)
      primary_expression->symbol->symbol_type = SYMBOL_TYPE_TEMPORARY_VARIABLE;
  }

  return semantics_analysis_result;
}

/*
expression
	: assignment_expression
	| expression ',' assignment_expression
	;
 */
int analyze_expression(std::shared_ptr<ast_node> expression, semantics_analysis_context &context) {
  int semantics_analysis_result = 0;
  std::vector<std::shared_ptr<ast_node>> assignment_expressions;
  std::shared_ptr<ast_node> node = expression;
  while (expression->node_type == NODE_TYPE_ASSIGNMENT_EXPRESSION &&
         expression->node_sub_type ==
             NODE_TYPE_ASSIGNMENT_EXPRESSION_SUBTYPE_UNARY_EXPRESSION_ASSIGNMENT_OPERATOR_ASSIGNMENT_EXPRESSION) {
    assignment_expressions.push_back(node->items[1]);
    node = node->items[0];
  }
  assignment_expressions.push_back(node->items[0]);
  assignment_expressions =
      std::vector<std::shared_ptr<ast_node>>(assignment_expressions.rbegin(), assignment_expressions.rend());
  expression->sub_nodes = assignment_expressions;

  for (std::shared_ptr<ast_node> assignment_expression : assignment_expressions) {
    semantics_analysis_result = analyze_assignment_expression(assignment_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
  }
  //逗号表达式的结果是最后一项
  expression->symbol = assignment_expressions[assignment_expressions.size() - 1]->symbol;

  return semantics_analysis_result;
}

int analyze_assignment_expression(std::shared_ptr<ast_node> assignment_expression,
                                  semantics_analysis_context &context) {
  int semantics_analysis_result = 0;
  return semantics_analysis_result;
}

/*
string
	: STRING_LITERAL
	| FUNC_NAME
	;
 */

int analyze_string(std::shared_ptr<ast_node> string_node, semantics_analysis_context &context,
                   std::string &out_string) {
  std::shared_ptr<std::string> string_literal;
  int semantics_analysis_result = 0;
  switch (string_node->node_sub_type) {
  case NODE_TYPE_STRING_SUBTYPE_STRING_LITERAL:
    out_string = extract_string(*string_node->items[0]->lexeme);
    break;
  case NODE_TYPE_STRING_SUBTYPE_FUNC_NAME:
    printf("%s:%d error:\n\tunsupported C99 '__func__' in string\n", input_file_name.c_str(),
           string_node->get_first_terminal_line_no());
    semantics_analysis_result = 1;
    break;
  }
  return semantics_analysis_result;
}

/*
constant
	: I_CONSTANT	includes character_constant
    | F_CONSTANT
    | ENUMERATION_CONSTANT	 after it has been defined as such
;
 */
int analyze_constant(std::shared_ptr<ast_node> constant, semantics_analysis_context &context) {
  int semantics_analysis_result;
  switch (constant->node_sub_type) {
  case NODE_TYPE_CONSTANT_SUBTYPE_ICONSTANT:
    semantics_analysis_result = check_integer_constant(constant->items[0]);
    break;
  case NODE_TYPE_CONSTANT_SUBTYPE_FCONSTANT:
    semantics_analysis_result = check_floating_constant(constant->items[0]);
    break;
  case NODE_TYPE_CONSTANT_SUBTYPE_ENUMERATION_CONSTANT:
    printf("%s:%d error:\n\tshould not reach enumeration constant\n", input_file_name.c_str(),
           constant->get_first_terminal_line_no());
    semantics_analysis_result = 1;
    break;
  }
  return semantics_analysis_result;
}

int check_integer_constant(std::shared_ptr<ast_node> integer_constant) {
  std::string lexeme = *integer_constant->lexeme;
  integer_constant->symbol = std::make_shared<tsc_symbol>();
  int semantics_analysis_result = 0;
  integer_constant->symbol->symbol_type = SYMBOL_TYPE_ICONSTANT;
  integer_constant->symbol->value = std::make_shared<expression_value>();
  // std::stoi std::stol std::stoull std::stoll 最后一个参数base设置为0的时候可以根据输入串格式决定正确的base 这些函数会自动忽略后缀
  // 注意并没有stou或者stoui来完成到unsigned int的转换
  // 后缀与字面量类型的细节参见手册.u表示unsigned,l表示至少是long(可能是long long) ll表示是long long
  // 这里我们简单处理.没有后缀认为是int,l后缀认为是long,ll后缀认为是long long
  switch (integer_constant->lexeme_sub_type) {
  case I_CONSTANT_SUBTYPE_HEX_DIGIT:
    // (0[xX])[a-fA-F0-9]+(((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))？
  case I_CONSTANT_SUBTYPE_DECIMAL_DIGIT:
    // [1-9][0-9]*(((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))？
  case I_CONSTANT_SUBTYPE_OCTAL_DIGIT: {
    // 0[0-7]*(((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))?

    int suffix_length = 0;
    while (suffix_length < lexeme.length() && isalpha(lexeme[lexeme.length() - 1 - suffix_length]))
      suffix_length++;
    std::string suffix = lexeme.substr(lexeme.length() - suffix_length, suffix_length);
    if (is_unsigned_suffix(suffix)) {
      if (is_long_suffix(suffix)) {
        //unsigned long
        integer_constant->symbol->type = global_types::primitive_type_unsigned_long;
        integer_constant->symbol->value->unsigned_long_value = std::stoul(lexeme, nullptr, 0);

      } else if (is_long_long_suffix(suffix)) {
        //unsigned long long
        integer_constant->symbol->type = global_types::primitive_type_unsigned_long_long;
        integer_constant->symbol->value->unsigned_long_long_value = std::stoull(lexeme, nullptr, 0);

      } else {
        //unsigned int
        integer_constant->symbol->type = global_types::primitive_type_unsigned_int;
        integer_constant->symbol->value->unsigned_int_value = (unsigned int)std::stoul(lexeme, nullptr, 0);
      }
    }

    else {
      if (is_long_suffix(suffix)) {
        // long
        integer_constant->symbol->type = global_types::primitive_type_long;
        integer_constant->symbol->value->long_value = std::stol(lexeme, nullptr, 0);

      } else if (is_long_long_suffix(suffix)) {
        // long long
        integer_constant->symbol->type = global_types::primitive_type_long_long;
        integer_constant->symbol->value->long_long_value = std::stoll(lexeme, nullptr, 0);

      } else {
        // int
        integer_constant->symbol->type = global_types::primitive_type_int;
        integer_constant->symbol->value->int_value = std::stoi(lexeme, nullptr, 0);
      }
    }

  }

  break;
  case I_CONSTANT_SUBTYPE_CHAR_DIGIT:
    // (u|U|L)?'([^'\\\n]|(\\(['"\?\\abfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+)))+'
    // 前缀可以是u,U,l然后是一对单引号括起来的序列.序列中的一个元素可以是一个非'\'以及'\n'的普通字符或者一个转义字符.
    // 转义字符是普通转义字符如'\n'或者8进制(如\012)或者16进制(如\x12).这里我们不支持数字转义字符与宽字符
    {
      bool is_unsigned = false;
      bool is_wide = false;
      bool has_prefix = false;
      if (lexeme[0] == 'u' || lexeme[0] == 'U') {
        is_unsigned = true;
        has_prefix = true;
      }
      if (lexeme[0] == 'L') {
        is_wide = true;
        has_prefix = true;
      }
      std::string char_sequence;
      if (has_prefix)
        char_sequence = lexeme.substr(2, lexeme.length() - 3);
      else
        char_sequence = lexeme.substr(1, lexeme.length() - 2);
      integer_constant->symbol->type = global_types::primitive_type_char;

      if (char_sequence[0] == '\\') {
        if (char_sequence.length() == 1) {
          printf("%s:%d error:\n\tunknown escape sequence \n", input_file_name.c_str(),
                 integer_constant->get_first_terminal_line_no());
          semantics_analysis_result = 1;
          break;
        } else {
          integer_constant->symbol->value->char_value = escape_char(char_sequence[1]);
        }
      } else {
        integer_constant->symbol->value->char_value = char_sequence[0];
      }
    }
    break;
  }

  return semantics_analysis_result;
}
int check_floating_constant(std::shared_ptr<ast_node> floating_constant) {
  // std::stof std::stod std::stold 字符串转为float,double,long double
  std::string lexeme = *floating_constant->lexeme;
  floating_constant->symbol = std::make_shared<tsc_symbol>();
  int semantics_analysis_result = 0;
  floating_constant->symbol->symbol_type = SYMBOL_TYPE_FCONSTANT;
  floating_constant->symbol->value = std::make_shared<expression_value>();
  int suffix_length = 0;
  while (suffix_length < lexeme.length() && isalpha(lexeme[lexeme.length() - 1 - suffix_length]))
    suffix_length++;
  std::string suffix = lexeme.substr(lexeme.length() - suffix_length, suffix_length);
  if (is_long_double_suffix(suffix)) {
    floating_constant->symbol->type = global_types::primitive_type_long_long;
    floating_constant->symbol->value->long_double_value = std::stold(lexeme, nullptr);

  } else if (is_float_suffix(suffix)) {
    floating_constant->symbol->type = global_types::primitive_type_float;
    floating_constant->symbol->value->float_value = std::stof(lexeme, nullptr);

  } else {
    floating_constant->symbol->type = global_types::primitive_type_double;
    floating_constant->symbol->value->double_value = std::stod(lexeme, nullptr);
  }

  return semantics_analysis_result;
}

void setup_type_system() {

  global_types::primitive_type_void = std::make_shared<tsc_type>();
  global_types::primitive_type_char = std::make_shared<tsc_type>();
  global_types::primitive_type_unsigned_char = std::make_shared<tsc_type>();
  global_types::primitive_type_short = std::make_shared<tsc_type>();
  global_types::primitive_type_unsigned_short = std::make_shared<tsc_type>();
  global_types::primitive_type_int = std::make_shared<tsc_type>();
  global_types::primitive_type_unsigned_int = std::make_shared<tsc_type>();
  global_types::primitive_type_long = std::make_shared<tsc_type>();
  global_types::primitive_type_unsigned_long = std::make_shared<tsc_type>();
  global_types::primitive_type_long_long = std::make_shared<tsc_type>();
  global_types::primitive_type_unsigned_long_long = std::make_shared<tsc_type>();
  global_types::primitive_type_float = std::make_shared<tsc_type>();
  global_types::primitive_type_double = std::make_shared<tsc_type>();
  global_types::primitive_type_long_double = std::make_shared<tsc_type>();
  global_types::composite_type_const_char_star = std::make_shared<tsc_type>();
  global_types::primitive_type_const_void = std::make_shared<tsc_type>();
  global_types::primitive_type_const_char = std::make_shared<tsc_type>();
  global_types::primitive_type_const_unsigned_char = std::make_shared<tsc_type>();
  global_types::primitive_type_const_short = std::make_shared<tsc_type>();
  global_types::primitive_type_const_unsigned_short = std::make_shared<tsc_type>();
  global_types::primitive_type_const_int = std::make_shared<tsc_type>();
  global_types::primitive_type_const_unsigned_int = std::make_shared<tsc_type>();
  global_types::primitive_type_const_long = std::make_shared<tsc_type>();
  global_types::primitive_type_const_unsigned_long = std::make_shared<tsc_type>();
  global_types::primitive_type_const_long_long = std::make_shared<tsc_type>();
  global_types::primitive_type_const_unsigned_long_long = std::make_shared<tsc_type>();
  global_types::primitive_type_const_float = std::make_shared<tsc_type>();
  global_types::primitive_type_const_double = std::make_shared<tsc_type>();
  global_types::primitive_type_const_long_double = std::make_shared<tsc_type>();

  global_types::primitive_type_void->type_id = PRIMITIVE_TYPE_VOID;
  global_types::primitive_type_char->type_id = PRIMITIVE_TYPE_CHAR;
  global_types::primitive_type_unsigned_char->type_id = PRIMITIVE_TYPE_UNSIGNED_CHAR;
  global_types::primitive_type_short->type_id = PRIMITIVE_TYPE_SHORT;
  global_types::primitive_type_unsigned_short->type_id = PRIMITIVE_TYPE_UNSIGNED_SHORT;
  global_types::primitive_type_int->type_id = PRIMITIVE_TYPE_INT;
  global_types::primitive_type_unsigned_int->type_id = PRIMITIVE_TYPE_UNSIGNED_INT;
  global_types::primitive_type_long->type_id = PRIMITIVE_TYPE_LONG;
  global_types::primitive_type_unsigned_long->type_id = PRIMITIVE_TYPE_UNSIGNED_LONG;
  global_types::primitive_type_long_long->type_id = PRIMITIVE_TYPE_LONG_LONG;
  global_types::primitive_type_unsigned_long_long->type_id = PRIMITIVE_TYPE_UNSIGNED_LONG_LONG;
  global_types::primitive_type_float->type_id = PRIMITIVE_TYPE_FLOAT;
  global_types::primitive_type_double->type_id = PRIMITIVE_TYPE_DOUBLE;
  global_types::primitive_type_long_double->type_id = PRIMITIVE_TYPE_LONG_DOUBLE;

  global_types::primitive_type_const_void->type_id = PRIMITIVE_TYPE_VOID;
  global_types::primitive_type_const_char->type_id = PRIMITIVE_TYPE_CHAR;
  global_types::primitive_type_const_unsigned_char->type_id = PRIMITIVE_TYPE_UNSIGNED_CHAR;
  global_types::primitive_type_const_short->type_id = PRIMITIVE_TYPE_SHORT;
  global_types::primitive_type_const_unsigned_short->type_id = PRIMITIVE_TYPE_UNSIGNED_SHORT;
  global_types::primitive_type_const_int->type_id = PRIMITIVE_TYPE_INT;
  global_types::primitive_type_const_unsigned_int->type_id = PRIMITIVE_TYPE_UNSIGNED_INT;
  global_types::primitive_type_const_long->type_id = PRIMITIVE_TYPE_LONG;
  global_types::primitive_type_const_unsigned_long->type_id = PRIMITIVE_TYPE_UNSIGNED_LONG;
  global_types::primitive_type_const_long_long->type_id = PRIMITIVE_TYPE_LONG_LONG;
  global_types::primitive_type_const_unsigned_long_long->type_id = PRIMITIVE_TYPE_UNSIGNED_LONG_LONG;
  global_types::primitive_type_const_float->type_id = PRIMITIVE_TYPE_FLOAT;
  global_types::primitive_type_const_double->type_id = PRIMITIVE_TYPE_DOUBLE;
  global_types::primitive_type_const_long_double->type_id = PRIMITIVE_TYPE_LONG_DOUBLE;

  global_types::primitive_type_const_void->const_type_qualifier_set = true;
  global_types::primitive_type_const_char->const_type_qualifier_set = true;
  global_types::primitive_type_const_unsigned_char->const_type_qualifier_set = true;
  global_types::primitive_type_const_short->const_type_qualifier_set = true;
  global_types::primitive_type_const_unsigned_short->const_type_qualifier_set = true;
  global_types::primitive_type_const_int->const_type_qualifier_set = true;
  global_types::primitive_type_const_unsigned_int->const_type_qualifier_set = true;
  global_types::primitive_type_const_long->const_type_qualifier_set = true;
  global_types::primitive_type_const_unsigned_long->const_type_qualifier_set = true;
  global_types::primitive_type_const_long_long->const_type_qualifier_set = true;
  global_types::primitive_type_const_unsigned_long_long->const_type_qualifier_set = true;
  global_types::primitive_type_const_float->const_type_qualifier_set = true;
  global_types::primitive_type_const_double->const_type_qualifier_set = true;
  global_types::primitive_type_const_long_double->const_type_qualifier_set = true;

  global_types::composite_type_const_char_star = construct_pointer_to(global_types::primitive_type_const_char);
}