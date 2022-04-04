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
    std::shared_ptr<tsc_symbol> symbol = std::make_shared<tsc_symbol>();
    symbol->type = std::make_shared<tsc_type>();
    switch (external_declaration->node_sub_type) {
    case NODE_TYPE_EXTERNAL_DECLARATION_SUBTYPE_DECLARATION:
      semantics_analysis_result = analyze_declaration(external_declaration->items[0], context, symbol);
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

int analyze_declaration(std::shared_ptr<ast_node> declaration, semantics_analysis_context &context,
                        std::shared_ptr<tsc_symbol> &symbol) {

  int semantics_analysis_result = 0;
  switch (declaration->node_sub_type) {
  case NODE_TYPE_DECLARATION_SUBTYPE_DECLARATION_SPECIFIERS_SEMI_COLON: {
    std::shared_ptr<ast_node> declaration_specifiers = declaration->items[0];
    semantics_analysis_result = analyze_declaration_specifiers(declaration_specifiers, context, symbol);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    //如果没有init_declarator_list说明是声明一个类型如struct A{...};此时如果不是匿名enum_struct_union需要加入tags
    // primitive类型也没有name 这里如果之前已经有过同名的struct union enum了,需要检查之前的是否是incomplete.如果是才用当前定义覆盖.
    if (symbol->type->name)
      context.current_symbol_table_node->struct_union_enum_names[*symbol->type->name] = symbol->type;

  } break;
  case NODE_TYPE_DECLARATION_SUBTYPE_DECLARATION_SPECIFIERS_INIT_DECLARATOR_LIST_SEMI_COLON: {

    std::shared_ptr<ast_node> declaration_specifiers = declaration->items[0];
    std::shared_ptr<ast_node> init_declarator_list = declaration->items[1];
    semantics_analysis_result = analyze_declaration_specifiers(declaration_specifiers, context, symbol);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    semantics_analysis_result = analyze_init_declarator_list(init_declarator_list, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    //如果有init_declarator_list,可能是声明一个类型如struct A{...} a;此时也需要加入tags且A必须是complete type
    //注意指针变量的声明 struct A *p;则A可以是incomplete
  } break;
  case NODE_TYPE_DECLARATION_SUBTYPE_STATIC_ASSERT_DECLARATION:
    printf("%s:%d error:\n\tstatic_assert_declaration not supported\n", input_file_name.c_str(),
           declaration->get_first_terminal_line_no());
    return 1;
  }

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
                                   semantics_analysis_context &context, std::shared_ptr<tsc_symbol> &symbol) {
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

  semantics_analysis_result = check_storage_class_specifiers(storage_class_specifiers, context, symbol);
  if (semantics_analysis_result)
    return semantics_analysis_result;

  semantics_analysis_result = check_type_qualifiers(type_qualifiers, context, symbol);
  if (semantics_analysis_result)
    return semantics_analysis_result;

  semantics_analysis_result = check_function_specifiers(function_specifiers, symbol);
  if (semantics_analysis_result)
    return semantics_analysis_result;

  semantics_analysis_result = check_type_specifiers(type_specifiers, context, symbol);
  if (semantics_analysis_result)
    return semantics_analysis_result;

  return 0;
}

int check_function_specifiers(std::vector<std::shared_ptr<ast_node>> &function_specifiers,
                              std::shared_ptr<tsc_symbol> &symbol) {
  for (std::shared_ptr<ast_node> function_specifier : function_specifiers) {
    switch (function_specifier->node_sub_type) {
    case NODE_TYPE_FUNCTION_SPECIFIER_SUBTYPE_INLINE:
      symbol->type->is_inline = true;
    case NODE_TYPE_FUNCTION_SPECIFIER_SUBTYPE_NORETURN:
      symbol->type->is_noreturn = true;
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

int check_storage_class_specifiers(std::vector<std::shared_ptr<ast_node>> &storage_class_specifiers,
                                   semantics_analysis_context &context, std::shared_ptr<tsc_symbol> &symbol) {

  int typedef_count = 0;
  int extern_count = 0;
  int static_count = 0;
  int auto_count = 0;
  int register_count = 0;

  for (std::shared_ptr<ast_node> storage_class_specifier : storage_class_specifiers) {
    switch (storage_class_specifier->node_sub_type) {
    case NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_TYPEDEF:
      typedef_count++;
      symbol->is_typedef = true;
      break;

    case NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_EXTERN:
      extern_count++;
      symbol->is_extern = true;
      break;

    case NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_STATIC:
      static_count++;
      symbol->is_static = true;
      break;

    case NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_THREAD_LOCAL:
      printf("%s:%d error:\n\tunsupported C11 'thread_local' in storage_class_specifiers\n", input_file_name.c_str(),
             storage_class_specifier->get_first_terminal_line_no());
      return 1;

    case NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_AUTO:
      //no parent means global(outermost)
      if (!context.current_symbol_table_node->parent) {
        printf("%s:%d error:\n\tglobal variables cannot be auto in storage_class_specifiers\n", input_file_name.c_str(),
               storage_class_specifier->get_first_terminal_line_no());
        return 1;
      }
      auto_count++;
      break;

    case NODE_TYPE_STORAGE_CLASS_SPECIFIER_SUBTYPE_REGISTER:
      if (!context.current_symbol_table_node->parent) {
        printf("%s:%d error:\n\tglobal variables cannot be register in storage_class_specifiers\n",
               input_file_name.c_str(), storage_class_specifier->get_first_terminal_line_no());
        return 1;
      }
      register_count++;
      symbol->is_register = true;
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

int check_type_qualifiers(std::vector<std::shared_ptr<ast_node>> &type_qualifiers, semantics_analysis_context &context,
                          std::shared_ptr<tsc_symbol> &symbol) {

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
      symbol->type->const_type_qualifier_set = true;
      break;
    case NODE_TYPE_TYPE_QUALIFIER_SUBTYPE_RESTRICT:
      if (!context.current_symbol_table_node->parent) {
        printf("%s:%d error:\n\tglobal variables cannot be restrict in type_qualifiers\n", input_file_name.c_str(),
               type_qualifier->get_first_terminal_line_no());
        return 1;
      }
      symbol->type->restrict_type_qualifier_set = true;
      break;
    case NODE_TYPE_TYPE_QUALIFIER_SUBTYPE_VOLATILE:
      symbol->type->volatile_type_qualifier_set = true;
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

int check_type_specifiers(std::vector<std::shared_ptr<ast_node>> &type_specifiers, semantics_analysis_context &context,
                          std::shared_ptr<tsc_symbol> &symbol) {
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
      type_id = PRIMITIVE_TYPE_ENUM;
      struct_union_enum_count++;
      break;
    case NODE_TYPE_TYPE_SPECIFIER_SUBTYPE_TYPEDEF_NAME:
    default:
      break;
    }
  }

  //全局变量default int.在不支持typedef的时候不会出现这种情况
  if (!context.current_symbol_table_node->parent && type_specifiers.empty()) {
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
    case PRIMITIVE_TYPE_ENUM:

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
    case PRIMITIVE_TYPE_ENUM:

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
    case PRIMITIVE_TYPE_ENUM:
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
    case PRIMITIVE_TYPE_ENUM:
      printf("%s:%d error:\n\t'long' and 'enum' in declaration specifiers\n", input_file_name.c_str(),
             type_specifiers[0]->get_first_terminal_line_no());
      return 1;
    default:
      break;
    }
  }

  switch (type_id) {
  case PRIMITIVE_TYPE_VOID:
    symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_void
                                                          : global_types::primitive_type_const_void;

    break;
  case PRIMITIVE_TYPE_CHAR:
    symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_char
                                                          : global_types::primitive_type_const_char;
    break;
  case PRIMITIVE_TYPE_UNSIGNED_CHAR:
    symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_unsigned_char
                                                          : global_types::primitive_type_const_unsigned_char;
    break;
  case PRIMITIVE_TYPE_SHORT:
    symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_short
                                                          : global_types::primitive_type_const_short;
    break;
  case PRIMITIVE_TYPE_UNSIGNED_SHORT:
    symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_unsigned_short
                                                          : global_types::primitive_type_const_unsigned_short;
    break;
  case PRIMITIVE_TYPE_ENUM: {
    symbol->type->type_id = PRIMITIVE_TYPE_ENUM;
    symbol->type->type_size = sizeof(int);
    std::shared_ptr<ast_node> enum_specifier = type_specifiers[0]->items[0];
    int semantics_analysis_result = analyze_enum_specifier(enum_specifier, context, symbol);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    break;
  }

  break;
  case PRIMITIVE_TYPE_INT:
    symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_int
                                                          : global_types::primitive_type_const_int;
    break;

  case PRIMITIVE_TYPE_UNSIGNED_INT:
    symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_unsigned_int
                                                          : global_types::primitive_type_const_unsigned_int;
    break;
  case PRIMITIVE_TYPE_LONG:
    symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_long
                                                          : global_types::primitive_type_const_long;
    break;
  case PRIMITIVE_TYPE_UNSIGNED_LONG:
    symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_unsigned_long
                                                          : global_types::primitive_type_const_unsigned_long;
    break;
  case PRIMITIVE_TYPE_LONG_LONG:
    symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_long_long
                                                          : global_types::primitive_type_const_long_long;
    break;
  case PRIMITIVE_TYPE_UNSIGNED_LONG_LONG:
    symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_unsigned_long_long
                                                          : global_types::primitive_type_const_unsigned_long_long;
    break;
  case PRIMITIVE_TYPE_FLOAT:
    symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_float
                                                          : global_types::primitive_type_const_float;
    break;
  case PRIMITIVE_TYPE_DOUBLE:
    symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_double
                                                          : global_types::primitive_type_const_double;
    break;
  case PRIMITIVE_TYPE_LONG_DOUBLE:
    symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_long_double
                                                          : global_types::primitive_type_const_long_double;
    break;
  case RECORD_TYPE_STRUCT_OR_UNION: {
    symbol->type->type_id = type_id;
    std::shared_ptr<ast_node> struct_or_union_specifier = type_specifiers[0]->items[0];
    int semantics_analysis_result = analyze_struct_or_union_specifier(struct_or_union_specifier, context, symbol);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    break;
  }

  break;
  default:
    break;
  }

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
                           std::shared_ptr<tsc_symbol> &symbol) {

  std::shared_ptr<ast_node> identifier_node; // enum A{...}; identifier=A
  std::shared_ptr<ast_node> enumerator_list;

  switch (enum_specifier->node_sub_type) {
  case NODE_TYPE_ENUM_SPECIFIER_SUBTYPE_ENUM_LEFT_BRACE_ENUMATOR_LIST_RIGHT_BRACE:
  case NODE_TYPE_ENUM_SPECIFIER_SUBTYPE_ENUM_LEFT_BRACE_ENUMATOR_LIST_COMMA_RIGHT_BRACE:
    enumerator_list = enum_specifier->items[2];
    symbol->type->is_complete = true;
    break;

  case NODE_TYPE_ENUM_SPECIFIER_SUBTYPE_ENUM_IDENTIFER_LEFT_BRACE_ENUMATOR_LIST_RIGHT_BRACE:
  case NODE_TYPE_ENUM_SPECIFIER_SUBTYPE_ENUM_IDENTIFER_LEFT_BRACE_ENUMATOR_LIST_COMMA_RIGHT_BRACE:
    identifier_node = enum_specifier->items[1];
    enumerator_list = enum_specifier->items[3];
    symbol->type->is_complete = true;
    break;

  case NODE_TYPE_ENUM_SPECIFIER_SUBTYPE_ENUM_IDENTIFER:
    identifier_node = enum_specifier->items[1];
    symbol->type->is_complete = false;
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
      //可以多次声明但是只能定义1次.检查之前是否已经有complete的declaration
      if (it->first == identifier) {
        if (!check_type_compatibility(it->second, symbol->type)) {
          printf("%s:%d error:\n\tincorrect tag '%s'\n", input_file_name.c_str(),
                 identifier_node->get_first_terminal_line_no(), identifier.c_str());
          return 1;
        }
        //之前可能已经有形如enum A;的incomplete declaration则用当前的declaration覆盖它(当前的declaration是否complete都可以覆盖)
        if (!it->second->is_complete)
          it->second = symbol->type;
        //之前已经有一个complete声明了,当前的必须是形如enum A;的声明否则是 redefinition
        else if (symbol->type->is_complete) {
          printf("%s:%d error:\n\tredefinition tag '%s'\n", input_file_name.c_str(),
                 identifier_node->get_first_terminal_line_no(), identifier.c_str());
          return 1;
        }
      }
    }
    symbol->type->name = identifier_node->lexeme;
  }

  if (enumerator_list)
    return analyze_enumerator_list(enumerator_list, context, symbol);
  else
    return 0;
}

/*
struct_or_union_specifier
: struct_or_union '{' struct_declaration_list '}'
| struct_or_union IDENTIFIER '{' struct_declaration_list '}'
| struct_or_union IDENTIFIER
;
*/

int analyze_struct_or_union_specifier(std::shared_ptr<ast_node> struct_or_union_specifier,
                                      semantics_analysis_context &context, std::shared_ptr<tsc_symbol> &symbol) {

  std::shared_ptr<ast_node> struct_or_union = struct_or_union_specifier->items[0];
  std::shared_ptr<ast_node> identifier_node;
  std::shared_ptr<ast_node> struct_declaration_list;

  switch (struct_or_union->node_sub_type) {
  case NODE_TYPE_STRUCT_OR_UNION_SUBTYPE_STRUCT:
    symbol->type->sub_type_id = SUB_TYPE_STRUCT;
    break;
  case NODE_TYPE_STRUCT_OR_UNION_SUBTYPE_UNION:
    symbol->type->sub_type_id = SUB_TYPE_UNION;
    break;
  }

  switch (struct_or_union_specifier->node_sub_type) {
  case NODE_TYPE_STRUCT_OR_UNION_SPECIFIER_SUBTYPE_STRUCT_OR_UNION_LEFT_BRACE_STRUCT_DECLARATION_LIST_RIGHT_BRACE: {
    symbol->type->is_complete = true;
    struct_declaration_list = struct_or_union_specifier->items[2];
  } break;

  case NODE_TYPE_STRUCT_OR_UNION_SPECIFIER_SUBTYPE_STRUCT_OR_UNION_IDENTIFIER_LEFT_BRACE_STRUCT_DECLARATION_LIST_RIGHT_BRACE: {
    symbol->type->is_complete = true;
    identifier_node = struct_or_union_specifier->items[1];
    struct_declaration_list = struct_or_union_specifier->items[3];
  }

  break;

  case NODE_TYPE_STRUCT_OR_UNION_SPECIFIER_SUBTYPE_IDENTIFIER: {
    symbol->type->is_complete = false;
    identifier_node = struct_or_union_specifier->items[1];
  }

  break;
  }

  if (identifier_node) {
    std::string identifier = *identifier_node->lexeme;
    // 检查符号表中是否已经有同名的符号
    for (std::map<std::string, std::shared_ptr<tsc_type>>::iterator it =
             context.current_symbol_table_node->struct_union_enum_names.begin();
         it != context.current_symbol_table_node->struct_union_enum_names.end(); it++) {
      //可以多次声明但是只能定义1次.检查之前是否已经有complete的declaration.这里的检查逻辑与enum一致
      if (it->first == identifier) {
        if (!check_type_compatibility(it->second, symbol->type)) {
          printf("%s:%d error:\n\tincorrect tag '%s'\n", input_file_name.c_str(),
                 identifier_node->get_first_terminal_line_no(), identifier.c_str());
          return 1;
        }
        if (!it->second->is_complete)
          it->second = symbol->type;
        else if (symbol->type->is_complete) {
          printf("%s:%d error:\n\tredefinition tag '%s'\n", input_file_name.c_str(),
                 identifier_node->get_first_terminal_line_no(), identifier.c_str());
          return 1;
        }
      }
    }
    symbol->type->name = identifier_node->lexeme;
  }

  if (struct_declaration_list)
    return analyze_struct_declaration_list(struct_declaration_list, context, symbol);

  return 0;
}

/*
struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;
*/
int analyze_struct_declaration_list(std::shared_ptr<ast_node> struct_declaration_list,
                                    semantics_analysis_context &context, std::shared_ptr<tsc_symbol> &symbol) {

  std::vector<std::shared_ptr<ast_node>> struct_declarations;
  std::shared_ptr<ast_node> node = struct_declaration_list;

  context.current_symbol_table_node = std::make_shared<symbol_table_node>();
  while (node->node_type == NODE_TYPE_STRUCT_DECLARATION_LIST &&
         node->node_sub_type == NODE_TYPE_STRUCT_DECLARATION_LIST_SUBTYPE_STRUCT_DECLARATION_LIST_STRUCT_DECLARATION) {
    struct_declarations.push_back(node->items[1]);
    node = node->items[0];
  }

  struct_declarations.push_back(node->items[0]);
  struct_declarations =
      std::vector<std::shared_ptr<ast_node>>(struct_declarations.rbegin(), struct_declarations.rend());
  struct_declaration_list->sub_nodes = struct_declarations;
  int semantics_analysis_result;
  for (std::shared_ptr<ast_node> struct_declaration : struct_declarations) {
    semantics_analysis_result = analyze_struct_declaration(struct_declaration, context, symbol);
    if (semantics_analysis_result)
      return semantics_analysis_result;
  }
  return 0;
}

/*
struct_declaration
	: specifier_qualifier_list ';'	 for anonymous struct/union
    | specifier_qualifier_list struct_declarator_list ';'
    | static_assert_declaration
;
*/

int analyze_struct_declaration(std::shared_ptr<ast_node> struct_declaration, semantics_analysis_context &context,
                               std::shared_ptr<tsc_symbol> &symbol) {
  //每个declarator都需要加入fields.一个struct_declaration可能有多个declarator如int a,b;
  int semantics_analysis_result;

  switch (struct_declaration->node_sub_type) {
  case NODE_TYPE_STRUCT_DECLARATION_SUBTYPE_SPECIFIER_QUALIFIER_LIST_SEMI_COLON: {
    std::shared_ptr<ast_node> specifier_qualifier_list = struct_declaration->items[0];

    semantics_analysis_result = analyze_specifier_qualifier_list(specifier_qualifier_list, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    std::shared_ptr<tsc_symbol> field_symbol = specifier_qualifier_list->symbol;
    //register an anonymous field
    std::shared_ptr<tsc_field> field = std::make_shared<tsc_field>();
    field->is_anonymous = true;
    //todo type must be complete
    field->type = field_symbol->type;
    field->is_bit_field = false;
    symbol->type->fields.push_back(field);

  } break;
  case NODE_TYPE_STRUCT_DECLARATION_SUBTYPE_SPECIFIER_QUALIFIER_LIST_STRUT_DECLARATOR_LIST_SEMI_COLON: {
    std::shared_ptr<ast_node> specifier_qualifier_list = struct_declaration->items[0];
    std::shared_ptr<ast_node> struct_declarator_list = struct_declaration->items[1];

    semantics_analysis_result = analyze_specifier_qualifier_list(specifier_qualifier_list, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    std::shared_ptr<tsc_symbol> field_symbol = specifier_qualifier_list->symbol;
    // non-anonymous fields registered in analyze_struct_declarator_list

    semantics_analysis_result =
        analyze_struct_declarator_list(struct_declarator_list, context, symbol, field_symbol->type);
    if (semantics_analysis_result)
      return semantics_analysis_result;

  } break;
  case NODE_TYPE_STRUCT_DECLARATION_SUBTYPE_STATIC_ASSERT_DECLARATION:
    printf("%s:%d error:\n\tstatic_assert_declaration not supported\n", input_file_name.c_str(),
           struct_declaration->get_first_terminal_line_no());
    return 1;
  }

  return 0;
}

/*
struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;
*/

int analyze_struct_declarator_list(std::shared_ptr<ast_node> struct_declarator_list,
                                   semantics_analysis_context &context, std::shared_ptr<tsc_symbol> &symbol,
                                   std::shared_ptr<tsc_type> field_type) {
  std::vector<std::shared_ptr<ast_node>> struct_declarators;
  std::shared_ptr<ast_node> node = struct_declarator_list;

  context.current_symbol_table_node = std::make_shared<symbol_table_node>();
  while (node->node_type == NODE_TYPE_STRUCT_DECLARATOR_LIST &&
         node->node_sub_type ==
             NODE_TYPE_STRUCT_DECLARATOR_LIST_SUBTYPE_STRUCT_DECLARATOR_LIST_COMMA_STRUCT_DECLARATOR) {
    struct_declarators.push_back(node->items[2]);
    node = node->items[0];
  }

  struct_declarators.push_back(node->items[0]);
  struct_declarators = std::vector<std::shared_ptr<ast_node>>(struct_declarators.rbegin(), struct_declarators.rend());
  struct_declarator_list->sub_nodes = struct_declarators;

  for (std::shared_ptr<ast_node> struct_declarator : struct_declarators) {
    std::shared_ptr<tsc_field> field = std::make_shared<tsc_field>();
    field->is_anonymous = false;
    //todo type可能与declarator有关.bit_field也还没处理.
    field->type = field_type;
    field->is_bit_field = false;
    symbol->type->fields.push_back(field);
  }

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
                            std::shared_ptr<tsc_symbol> &symbol) {

  std::vector<std::shared_ptr<ast_node>> enumerators;
  std::shared_ptr<ast_node> node = enumerator_list;

  while (node->node_type == NODE_TYPE_ENUMERATOR_LIST &&
         node->node_sub_type == NODE_TYPE_ENUMERATOR_LIST_SUBTYPE_ENUMERATOR_LIST_COMMA_ENUMERATOR) {
    enumerators.push_back(node->items[2]);
    node = node->items[0];
  }
  enumerators.push_back(node->items[0]);
  //这里也需要处理顺序.可以用前面的enumerator定义后面的enum B{ b1,b2=b1+10}; OK
  enumerators = std::vector<std::shared_ptr<ast_node>>(enumerators.rbegin(), enumerators.rend());
  enumerator_list->sub_nodes = enumerators;
  int semantics_analysis_result = 0;
  int next_value = 0;
  for (std::shared_ptr<ast_node> enumerator : enumerators) {
    int calculated_enumeration_constant_value;
    semantics_analysis_result =
        analyze_enumerator(enumerator, context, next_value, calculated_enumeration_constant_value);
    next_value = calculated_enumeration_constant_value + 1;
    if (semantics_analysis_result)
      return semantics_analysis_result;
  }
  return 0;
}

int analyze_enumerator(std::shared_ptr<ast_node> enumerator, semantics_analysis_context &context, int next_value,
                       int &calculated_enumeration_constant_value) {
  int semantics_analysis_result = 0;
  calculated_enumeration_constant_value = next_value;
  std::shared_ptr<ast_node> enumeration_constant = enumerator->items[0];
  std::shared_ptr<ast_node> constant_expression;
  std::shared_ptr<std::string> identifier = enumeration_constant->items[0]->lexeme;
  // 检查符号表中是否已经有同名的符号.经过gcc测试,在struct内部定义的enum的值的可以穿透到struct外部.
  // 所以struct的花括号并不是一个作用域
  // 例如全局范围内定义一个在union里面的enum,则在main()中可以读取到这个enum定义的常量
  // union A{  int a; enum B{c1,c2} t;  }; 如果没有这个't'会给一个warning declaration does not declare anything

  if (context.current_symbol_table_node->identifier_and_symbols.find(*identifier) !=
      context.current_symbol_table_node->identifier_and_symbols.end()) {
    printf("%s:%d error:\n\rredeclared '%s'\n", input_file_name.c_str(),
           enumeration_constant->get_first_terminal_line_no(), identifier->c_str());
    return 1;
  }
  //校验无误,加入符号表.这里要先校验constant expression避免 enum{x=x+1};这种问题

  switch (enumerator->node_sub_type) {
  case NODE_TYPE_ENUMERATOR_SUBTYPE_ENUMERATION_CONSTANT_ASSIGN_CONSTANT_EXPRESSION:
    constant_expression = enumerator->items[2];
    semantics_analysis_result = analyze_constant_expression(constant_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    calculated_enumeration_constant_value = constant_expression->symbol->value->long_long_value;
    break;
  case NODE_TYPE_ENUMERATOR_SUBTYPE_ENUMERATION_CONSTANT:
    break;
  }

  std::shared_ptr<tsc_symbol> symbol = std::make_shared<tsc_symbol>();
  //enum treated as int
  symbol->type = global_types::primitive_type_int;
  symbol->identifier = identifier;
  symbol->symbol_type = SYMBOL_TYPE_ENUMERATION_CONSTANT;
  symbol->value = std::make_shared<expression_value>();
  symbol->value->long_long_value = calculated_enumeration_constant_value;
  context.current_symbol_table_node->identifier_and_symbols[*identifier] = symbol;

  return semantics_analysis_result;
}

int analyze_constant_expression(std::shared_ptr<ast_node> constant_expression, semantics_analysis_context &context) {
  std::shared_ptr<ast_node> conditional_expression = constant_expression->items[0];
  int semantics_analysis_result = analyze_conditional_expression(conditional_expression, context);

  if (semantics_analysis_result)
    return semantics_analysis_result;

  if (conditional_expression->symbol->symbol_type != SYMBOL_TYPE_ICONSTANT) {
    printf("%s:%d error:\n\rexpected constant_expression\n", input_file_name.c_str(),
           constant_expression->get_first_terminal_line_no());
    return 1;
  }
  constant_expression->symbol = conditional_expression->symbol;
  printf("%s = %lld\n", constant_expression->get_expression().c_str(),
         conditional_expression->symbol->value->long_long_value);

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

  switch (conditional_expression->node_sub_type) {
  case NODE_TYPE_CONDITIONAL_EXPRESSION_SUBTYPE_LOGICAL_OR_EXPRESSION: {
    std::shared_ptr<ast_node> logical_or_expression = conditional_expression->items[0];
    semantics_analysis_result = analyze_logical_or_expression(logical_or_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    conditional_expression->symbol = logical_or_expression->symbol;
  } break;
  case NODE_TYPE_ASSIGNMENT_EXPRESSION_SUBTYPE_UNARY_EXPRESSION_ASSIGNMENT_OPERATOR_ASSIGNMENT_EXPRESSION: {
    std::shared_ptr<ast_node> logical_or_expression = conditional_expression->items[0];
    std::shared_ptr<ast_node> expression = conditional_expression->items[2];
    std::shared_ptr<ast_node> next_conditional_expression = conditional_expression->items[4];

    semantics_analysis_result = analyze_logical_or_expression(logical_or_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    semantics_analysis_result = analyze_expression(expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    semantics_analysis_result = analyze_conditional_expression(next_conditional_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    //todo 检查 expression 与 next_conditional_expression 是否可以有一个公共的类型.
    if (is_constant(logical_or_expression)) {
      switch (logical_or_expression->symbol->symbol_type) {
      case SYMBOL_TYPE_ICONSTANT:
        if (logical_or_expression->symbol->value->unsigned_long_long_value)
          conditional_expression->symbol = expression->symbol;
        else
          conditional_expression->symbol = next_conditional_expression->symbol;
        break;
      case SYMBOL_TYPE_FCONSTANT:
        if (logical_or_expression->symbol->value->long_double_value)
          conditional_expression->symbol = expression->symbol;
        else
          conditional_expression->symbol = next_conditional_expression->symbol;
        break;
      }
    } else {
      conditional_expression->symbol = std::make_shared<tsc_symbol>();
      conditional_expression->symbol->is_left_value = false;
      conditional_expression->symbol->symbol_type = SYMBOL_TYPE_TEMPORARY_VARIABLE;
    }
  }

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

  switch (logical_or_expression->node_sub_type) {
  case NODE_TYPE_LOGICAL_OR_EXPRESSION_SUBTYPE_LOGICAL_AND_EXPRESSION: {
    std::shared_ptr<ast_node> logical_and_expression = logical_or_expression->items[0];
    semantics_analysis_result = analyze_logical_and_expression(logical_and_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    logical_or_expression->symbol = logical_and_expression->symbol;

  }

  break;
  case NODE_TYPE_LOGICAL_OR_EXPRESSION_SUBTYPE_LOGICAL_OR_EXPRESSION_OR_OP_LOGICAL_AND_EXPRESSION:

    std::shared_ptr<ast_node> next_logical_or_expression = logical_or_expression->items[0];
    std::shared_ptr<ast_node> logical_and_expression = logical_or_expression->items[2];
    semantics_analysis_result = analyze_logical_and_expression(logical_and_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    semantics_analysis_result = analyze_logical_or_expression(next_logical_or_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    int binary_operator = BINARY_OPERATOR_OR;
    semantics_analysis_result = construct_binary_expression_symbol(logical_or_expression, binary_operator,
                                                                   next_logical_or_expression, logical_and_expression);

    break;
  }

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

  switch (logical_and_expression->node_sub_type) {
  case NODE_TYPE_LOGICAL_AND_EXPRESSION_SUBTYPE_INCLUSIVE_OR_EXPRESSION: {
    std::shared_ptr<ast_node> inclusive_or_expression = logical_and_expression->items[0];
    semantics_analysis_result = analyze_inclusive_or_expression(inclusive_or_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    logical_and_expression->symbol = inclusive_or_expression->symbol;

  }

  break;
  case NODE_TYPE_LOGICAL_AND_EXPRESSION_SUBTYPE_LOGICAL_AND_EXPRESSION_AND_OP_INCLUSIVE_OR_EXPRESSION: {
    std::shared_ptr<ast_node> next_logical_and_expression = logical_and_expression->items[0];
    std::shared_ptr<ast_node> inclusive_or_expression = logical_and_expression->items[2];

    semantics_analysis_result = analyze_logical_and_expression(next_logical_and_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    semantics_analysis_result = analyze_inclusive_or_expression(inclusive_or_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    int binary_operator = BINARY_OPERATOR_AND;
    semantics_analysis_result = construct_binary_expression_symbol(
        logical_and_expression, binary_operator, next_logical_and_expression, inclusive_or_expression);

  }

  break;
  }
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

  switch (inclusive_or_expression->node_sub_type) {
  case NODE_TYPE_INCLUSIVE_OR_EXPRESSION_SUBTYPE_EXCLUSIVE_OR_EXPRESSION: {
    std::shared_ptr<ast_node> exclusive_or_expression = inclusive_or_expression->items[0];
    semantics_analysis_result = analyze_exclusive_or_expression(exclusive_or_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    inclusive_or_expression->symbol = exclusive_or_expression->symbol;
  }

  break;
  case NODE_TYPE_INCLUSIVE_OR_EXPRESSION_SUBTYPE_INCLUSIVE_OR_EXPRESSION_BITOR_EXCLUSIVE_OR_EXPRESSION: {
    std::shared_ptr<ast_node> next_inclusive_or_expression = inclusive_or_expression->items[0];
    std::shared_ptr<ast_node> exclusive_or_expression = inclusive_or_expression->items[2];

    semantics_analysis_result = analyze_inclusive_or_expression(next_inclusive_or_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    semantics_analysis_result = analyze_exclusive_or_expression(exclusive_or_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    int binary_operator = BINARY_OPERATOR_BITXOR;
    semantics_analysis_result = construct_binary_expression_symbol(
        inclusive_or_expression, binary_operator, next_inclusive_or_expression, exclusive_or_expression);

  }

  break;
  }

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

  switch (exclusive_or_expression->node_sub_type) {
  case NODE_TYPE_EXCLUSIVE_OR_EXPRESSION_SUBTYPE_AND_EXPRESSION: {
    std::shared_ptr<ast_node> and_expression = exclusive_or_expression->items[0];
    semantics_analysis_result = analyze_and_expression(and_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    exclusive_or_expression->symbol = and_expression->symbol;
  }

  break;
  case NODE_TYPE_EXCLUSIVE_OR_EXPRESSION_SUBTYPE_EXCLUSIVE_OR_EXPRESSION_BITXOR_AND_EXPRESSION: {

    std::shared_ptr<ast_node> next_exclusive_or_expression = exclusive_or_expression->items[0];
    std::shared_ptr<ast_node> and_expression = exclusive_or_expression->items[2];

    semantics_analysis_result = analyze_exclusive_or_expression(next_exclusive_or_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    semantics_analysis_result = analyze_and_expression(and_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    int binary_operator = BINARY_OPERATOR_BITXOR;
    semantics_analysis_result = construct_binary_expression_symbol(exclusive_or_expression, binary_operator,
                                                                   next_exclusive_or_expression, and_expression);
  }

  break;
  }

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

  switch (and_expression->node_sub_type) {
  case NODE_TYPE_AND_EXPRESSION_SUBTYPE_EQUALITY_EXPRESSION: {
    std::shared_ptr<ast_node> equality_expression = and_expression->items[0];
    semantics_analysis_result = analyze_equality_expression(equality_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    and_expression->symbol = equality_expression->symbol;
  }

  break;
  case NODE_TYPE_AND_EXPRESSION_SUBTYPE_AND_EXPRESSION_BITAND_EQUALITY_EXPRESSION: {

    std::shared_ptr<ast_node> next_and_expression = and_expression->items[0];
    std::shared_ptr<ast_node> equality_expression = and_expression->items[2];

    semantics_analysis_result = analyze_and_expression(next_and_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    semantics_analysis_result = analyze_equality_expression(equality_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    int binary_operator = BINARY_OPERATOR_AND;
    semantics_analysis_result =
        construct_binary_expression_symbol(and_expression, binary_operator, next_and_expression, equality_expression);

  }

  break;
  }

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

  switch (equality_expression->node_sub_type) {
  case NODE_TYPE_EQUALITY_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION: {
    std::shared_ptr<ast_node> relational_expression = equality_expression->items[0];
    semantics_analysis_result = analyze_relational_expression(relational_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    equality_expression->symbol = relational_expression->symbol;
  }

  break;
  case NODE_TYPE_EQUALITY_EXPRESSION_SUBTYPE_EQUALITY_EXPRESSION_EQ_OP_RELATIONAL_EXPRESSION:
  case NODE_TYPE_EQUALITY_EXPRESSION_SUBTYPE_EQUALITY_EXPRESSION_NE_OP_RELATIONAL_EXPRESSION: {

    std::shared_ptr<ast_node> next_equality_expression = equality_expression->items[0];
    std::shared_ptr<ast_node> relational_expression = equality_expression->items[2];

    semantics_analysis_result = analyze_equality_expression(next_equality_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    semantics_analysis_result = analyze_relational_expression(relational_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    int binary_operator = 0;
    switch (equality_expression->node_sub_type) {
    case NODE_TYPE_EQUALITY_EXPRESSION_SUBTYPE_EQUALITY_EXPRESSION_EQ_OP_RELATIONAL_EXPRESSION:
      binary_operator = BINARY_OPERATOR_EQ_OP;
      break;
    case NODE_TYPE_EQUALITY_EXPRESSION_SUBTYPE_EQUALITY_EXPRESSION_NE_OP_RELATIONAL_EXPRESSION:
      binary_operator = BINARY_OPERATOR_NE_OP;
    }
    semantics_analysis_result = construct_binary_expression_symbol(equality_expression, binary_operator,
                                                                   next_equality_expression, relational_expression);

  }

  break;
  }

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

  switch (relational_expression->node_sub_type) {
  case NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_SHIFT_EXPRESSION: {
    std::shared_ptr<ast_node> shift_expression = relational_expression->items[0];
    semantics_analysis_result = analyze_shift_expression(shift_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    relational_expression->symbol = shift_expression->symbol;
  }

  break;
  case NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_LESS_THAN_SHIFT_EXPRESSION:
  case NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_GREATER_THAN_SHIFT_EXPRESSION:
  case NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_LE_OP_SHIFT_EXPRESSION:
  case NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_GE_OP_SHIFT_EXPRESSION: {

    std::shared_ptr<ast_node> next_relational_expression = relational_expression->items[0];
    std::shared_ptr<ast_node> shift_expression = relational_expression->items[2];

    semantics_analysis_result = analyze_relational_expression(next_relational_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    semantics_analysis_result = analyze_shift_expression(shift_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    int binary_operator = 0;
    switch (relational_expression->node_sub_type) {
    case NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_LESS_THAN_SHIFT_EXPRESSION:
      binary_operator = BINARY_OPERATOR_LESS_THAN;
      break;
    case NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_GREATER_THAN_SHIFT_EXPRESSION:
      binary_operator = BINARY_OPERATOR_GREATER_THAN;
      break;
    case NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_LE_OP_SHIFT_EXPRESSION:
      binary_operator = BINARY_OPERATOR_LESS_THAN_OR_EQUAL;
      break;
    case NODE_TYPE_RELATIONAL_EXPRESSION_SUBTYPE_RELATIONAL_EXPRESSION_GE_OP_SHIFT_EXPRESSION:
      binary_operator = BINARY_OPERATOR_GREATER_THAN_OR_EQUAL;
      break;
    }
    semantics_analysis_result = construct_binary_expression_symbol(relational_expression, binary_operator,
                                                                   next_relational_expression, shift_expression);

  }

  break;
  }

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

  std::shared_ptr<ast_node> next_shift_expression;

  switch (shift_expression->node_sub_type) {
  case NODE_TYPE_SHIFT_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION: {
    std::shared_ptr<ast_node> additive_expression = shift_expression->items[0];
    semantics_analysis_result = analyze_additive_expression(additive_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    shift_expression->symbol = additive_expression->symbol;

  }

  break;
  case NODE_TYPE_SHIFT_EXPRESSION_SUBTYPE_SHIFT_EXPRESSION_LEFT_SHIFT_ADDITIVE_EXPRESSION:
  case NODE_TYPE_SHIFT_EXPRESSION_SUBTYPE_SHIFT_EXPRESSION_RIGHT_SHIFT_ADDITIVE_EXPRESSION: {
    next_shift_expression = shift_expression->items[0];
    std::shared_ptr<ast_node> additive_expression = shift_expression->items[2];

    semantics_analysis_result = analyze_additive_expression(additive_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    semantics_analysis_result = analyze_shift_expression(next_shift_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    int binary_operator = 0;
    switch (shift_expression->node_sub_type) {
    case NODE_TYPE_SHIFT_EXPRESSION_SUBTYPE_SHIFT_EXPRESSION_LEFT_SHIFT_ADDITIVE_EXPRESSION:
      binary_operator = BINARY_OPERATOR_LEFT_SHIFT;
      break;
    case NODE_TYPE_SHIFT_EXPRESSION_SUBTYPE_SHIFT_EXPRESSION_RIGHT_SHIFT_ADDITIVE_EXPRESSION:
      binary_operator = BINARY_OPERATOR_RIGHT_SHIFT;
      break;
    }
    semantics_analysis_result = construct_binary_expression_symbol(shift_expression, binary_operator,
                                                                   next_shift_expression, additive_expression);
  }

  break;
  }

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

  switch (additive_expression->node_sub_type) {
  case NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION: {
    std::shared_ptr<ast_node> multiplicative_expression = additive_expression->items[0];
    semantics_analysis_result = analyze_multiplicative_expression(multiplicative_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    additive_expression->symbol = multiplicative_expression->symbol;
  }

  break;
  case NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION_ADD_MULTIPLICATIVE_EXPRESSION:
  case NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION_SUB_MULTIPLICATIVE_EXPRESSION: {
    std::shared_ptr<ast_node> multiplicative_expression = additive_expression->items[2];
    std::shared_ptr<ast_node> next_additive_expression = additive_expression->items[0];
    semantics_analysis_result = analyze_multiplicative_expression(multiplicative_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    semantics_analysis_result = analyze_additive_expression(next_additive_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    int binary_operator = 0;
    switch (additive_expression->node_sub_type) {
    case NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION_ADD_MULTIPLICATIVE_EXPRESSION:
      binary_operator = BINARY_OPERATOR_ADD;
      break;
    case NODE_TYPE_ADDITIVE_EXPRESSION_SUBTYPE_ADDITIVE_EXPRESSION_SUB_MULTIPLICATIVE_EXPRESSION:
      binary_operator = BINARY_OPERATOR_SUB;
      break;
    }
    semantics_analysis_result = construct_binary_expression_symbol(
        additive_expression, binary_operator, additive_expression->items[0], additive_expression->items[2]);
    if (semantics_analysis_result)
      return semantics_analysis_result;

  } break;
  }

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

  switch (multiplicative_expression->node_sub_type) {
  case NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_CAST_EXPRESSION: {
    std::shared_ptr<ast_node> cast_expression = multiplicative_expression->items[0];
    semantics_analysis_result = analyze_cast_expression(cast_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    multiplicative_expression->symbol = cast_expression->symbol;
  } break;
  case NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION_MUL_CAST_EXPRESSION:
  case NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION_DIV_CAST_EXPRESSION:
  case NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION_MOD_CAST_EXPRESSION: {
    std::shared_ptr<ast_node> next_multiplicative_expression = multiplicative_expression->items[0];
    std::shared_ptr<ast_node> cast_expression = multiplicative_expression->items[2];
    semantics_analysis_result = analyze_cast_expression(cast_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    semantics_analysis_result = analyze_multiplicative_expression(next_multiplicative_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    int binary_operator = 0;
    switch (multiplicative_expression->node_sub_type) {
    case NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION_MUL_CAST_EXPRESSION:
      // A * a;可能A是一个类型,a是指向A的指针或者A,a都是identifier这是一个乘法表达式.不支持typedef则没有这个问题
      binary_operator = BINARY_OPERATOR_MUL;
      break;
    case NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION_DIV_CAST_EXPRESSION:
      binary_operator = BINARY_OPERATOR_DIV;
      break;
    case NODE_TYPE_MULTIPLICATIVE_EXPRESSION_SUBTYPE_MULTIPLICATIVE_EXPRESSION_MOD_CAST_EXPRESSION:
      binary_operator = BINARY_OPERATOR_MOD;
      break;
    }
    semantics_analysis_result = construct_binary_expression_symbol(multiplicative_expression, binary_operator,
                                                                   next_multiplicative_expression, cast_expression);
    if (semantics_analysis_result)
      return semantics_analysis_result;
  } break;
  }

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

  switch (cast_expression->node_sub_type) {
  case NODE_TYPE_CAST_EXPRESSION_SUBTYPE_UNARY_EXPRESSION: {
    std::shared_ptr<ast_node> unary_expression = cast_expression->items[0];
    semantics_analysis_result = analyze_unary_expression(unary_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    cast_expression->symbol = unary_expression->symbol;
  }

  break;
  case NODE_TYPE_CAST_EXPRESSION_SUBTYPE_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS_CAST_EXPRESSION:
    std::shared_ptr<ast_node> type_name = cast_expression->items[1];
    std::shared_ptr<tsc_type> type_out = std::make_shared<tsc_type>();
    semantics_analysis_result = analyze_type_name(type_name, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    std::shared_ptr<ast_node> next_cast_expression = cast_expression->items[3];
    semantics_analysis_result = analyze_cast_expression(next_cast_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    cast_expression->symbol = std::make_shared<tsc_symbol>();
    // todo 对 const expression 强转还是 const expression 如 (int)1.0f
    // struct_union 不能强转为primitive type. 反之primitive type也不能转为 struct_union
    // 不能直接将double转为指针类型(gcc)
    cast_expression->symbol->symbol_type = SYMBOL_TYPE_TEMPORARY_VARIABLE;
    cast_expression->symbol->type = type_out;

    break;
  }

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

  switch (unary_expression->node_sub_type) {
  case NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION: {
    std::shared_ptr<ast_node> postfix_expression = unary_expression->items[0];
    semantics_analysis_result = analyze_postfix_expression(postfix_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    unary_expression->symbol = postfix_expression->symbol;
  }

  break;
  case NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_INC_OP_UNARY_EXPRESSION:
  case NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_DEC_OP_UNARY_EXPRESSION: {
    // ++和--要求操作数为左值.可以对double ++和--
    //todo unary_expression construct symbol
    std::shared_ptr<ast_node> next_unary_expression = unary_expression->items[1];
    semantics_analysis_result = analyze_unary_expression(next_unary_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    if (!next_unary_expression->symbol->is_left_value) {
      printf("%s:%d error:\n\tlvalue expected in unary_expression\n", input_file_name.c_str(),
             unary_expression->get_first_terminal_line_no());
      semantics_analysis_result = 1;
      return semantics_analysis_result;
    }
    unary_expression->symbol = std::make_shared<tsc_symbol>();
    unary_expression->symbol->symbol_type = SYMBOL_TYPE_TEMPORARY_VARIABLE;
    unary_expression->symbol->type = next_unary_expression->symbol->type;

  }

  break;
  case NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_UNARY_OPERATOR_CAST_EXPRESSION: {
    std::shared_ptr<ast_node> unary_operator = unary_expression->items[0];
    std::shared_ptr<ast_node> cast_expression = unary_expression->items[1];

    semantics_analysis_result = analyze_cast_expression(cast_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    unary_expression->symbol = std::make_shared<tsc_symbol>();
    unary_expression->symbol->operands.push_back(cast_expression->symbol);
    unary_expression->symbol->is_left_value = false;
    // unary operator包括 MUL(指针deference) BITAND(取地址)等.类型根据 unary operator决定
    // 字符串字面量可以取地址 char (*p)[4]=&"abc";
    switch (unary_operator->node_sub_type) {
    case NODE_TYPE_UNARY_OPERATOR_SUBTYPE_BITAND: {
      // '&' 取地址.结果type是pointer to type of cast_expression
      unary_expression->symbol->operator_id = UNARY_OPERATOR_BIT_AND;
      unary_expression->symbol->symbol_type = SYMBOL_TYPE_TEMPORARY_VARIABLE;
      if (cast_expression->symbol->memory_location) {
        printf("%s:%d error:\n\tcast_expression does not have a memory location in unary_expression\n",
               input_file_name.c_str(), unary_expression->get_first_terminal_line_no());
        semantics_analysis_result = 1;
        return semantics_analysis_result;
      }

      unary_expression->symbol->type = construct_pointer_to(cast_expression->symbol->type);
    }

    break;
    case NODE_TYPE_UNARY_OPERATOR_SUBTYPE_MUL: {
      // '*' deference.结果type是type of cast_expression 的underlying type
      // void* p; p[2]; ->OK. warning  dereferencing 'void *' pointer.
      // char ch=p[2] error: void value not ignored as it ought to be
      unary_expression->symbol->operator_id = UNARY_OPERATOR_MUL;
      unary_expression->symbol->symbol_type = SYMBOL_TYPE_TEMPORARY_VARIABLE;
      if (is_array_or_pointer(cast_expression)) {
        printf("%s:%d error:\n\tcast_expression should be array or pointer in unary_expression\n",
               input_file_name.c_str(), unary_expression->get_first_terminal_line_no());
        semantics_analysis_result = 1;
        return semantics_analysis_result;
      }

      if (cast_expression->symbol->type->underlying_type->type_id == PRIMITIVE_TYPE_VOID) {
        printf("%s:%d warning:\n\tdereferencing 'void *' pointer in unary_expression\n", input_file_name.c_str(),
               unary_expression->get_first_terminal_line_no());
      }

      unary_expression->symbol->type = cast_expression->symbol->type->underlying_type;
    } break;
    case NODE_TYPE_UNARY_OPERATOR_SUBTYPE_ADD: {
      // '+'算数取正号.需要类型是算数类型(不能是struct union array pointer).
      // 注意这个operator作用后就不再是lvalue所以这里不能直接令unary_expression->symbol=cast_expression->symbol

      if (!is_integer_or_floating_number(cast_expression)) {
        printf("%s:%d error:\n\tcast_expression should be integer or floating_number in unary_expression\n",
               input_file_name.c_str(), unary_expression->get_first_terminal_line_no());
        semantics_analysis_result = 1;
        return semantics_analysis_result;
      }
      semantics_analysis_result =
          construct_unary_expression_symbol(unary_expression, UNARY_OPERATOR_ADD, cast_expression);
      if (semantics_analysis_result)
        return semantics_analysis_result;
    } break;
    case NODE_TYPE_UNARY_OPERATOR_SUBTYPE_SUB: {
      // '-'算数取负.需要类型是算数类型,同时要处理constant expression取负的问题
      unary_expression->symbol->operator_id = UNARY_OPERATOR_SUB;

      if (!is_integer_or_floating_number(cast_expression)) {
        printf("%s:%d error:\n\tcast_expression should be integer or floating_number in unary_expression\n",
               input_file_name.c_str(), unary_expression->get_first_terminal_line_no());
        semantics_analysis_result = 1;
        return semantics_analysis_result;
      }

      semantics_analysis_result =
          construct_unary_expression_symbol(unary_expression, UNARY_OPERATOR_SUB, cast_expression);
      if (semantics_analysis_result)
        return semantics_analysis_result;
    } break;
    case NODE_TYPE_UNARY_OPERATOR_SUBTYPE_BITNOT: {
      // '~'按位取反.需要类型是整数类型
      unary_expression->symbol->operator_id = UNARY_OPERATOR_BITNOT;

      if (!is_integer(cast_expression)) {
        printf("%s:%d error:\n\tcast_expression should be integer in unary_expression\n", input_file_name.c_str(),
               unary_expression->get_first_terminal_line_no());
        semantics_analysis_result = 1;
        return semantics_analysis_result;
      }

      semantics_analysis_result =
          construct_unary_expression_symbol(unary_expression, UNARY_OPERATOR_BITNOT, cast_expression);
      if (semantics_analysis_result)
        return semantics_analysis_result;
    } break;
    case NODE_TYPE_UNARY_OPERATOR_SUBTYPE_OPERATOR_NOT: {
      // '!'逻辑not.需要类型是算数类型
      unary_expression->symbol->operator_id = UNARY_OPERATOR_NOT;

      if (!is_integer_or_floating_number(cast_expression)) {
        printf("%s:%d error:\n\tcast_expression should be integer or floating_number in unary_expression\n",
               input_file_name.c_str(), unary_expression->get_first_terminal_line_no());
        semantics_analysis_result = 1;
        return semantics_analysis_result;
      }

      semantics_analysis_result =
          construct_unary_expression_symbol(unary_expression, UNARY_OPERATOR_NOT, cast_expression);
      if (semantics_analysis_result)
        return semantics_analysis_result;
    } break;
    }
  }

  break;
  case NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_SIZEOF_UNARY_EXPRESSION: {
    std::shared_ptr<ast_node> next_unary_expression = unary_expression->items[1];
    semantics_analysis_result = analyze_unary_expression(next_unary_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    if (next_unary_expression->symbol->type == global_types::primitive_type_void ||
        next_unary_expression->symbol->type == global_types::primitive_type_const_void) {
      printf("%s:%d error:\n\tinvalid application of 'sizeof' to an incomplete type 'void' in unary_expression\n",
             input_file_name.c_str(), unary_expression->get_first_terminal_line_no());
      return 1;
    }

    if (!next_unary_expression->symbol->type->is_complete) {
      printf("%s:%d error:\n\tinvalid application of 'sizeof' to an incomplete type '%s' in unary_expression\n",
             input_file_name.c_str(), unary_expression->get_first_terminal_line_no(),
             next_unary_expression->symbol->type->name->c_str());
      return 1;
    }
    unary_expression->symbol = std::make_shared<tsc_symbol>();
    unary_expression->symbol->symbol_type = SYMBOL_TYPE_ICONSTANT;
    unary_expression->symbol->type = global_types::primitive_type_sizeof;
    unary_expression->symbol->value = std::make_shared<expression_value>();
    unary_expression->symbol->value->size_value = next_unary_expression->symbol->type->type_size;
  }

  break;
  case NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_SIZEOF_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS: {
    std::shared_ptr<ast_node> type_name = unary_expression->items[2];

    semantics_analysis_result = analyze_type_name(type_name, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    std::shared_ptr<tsc_type> type_out = type_name->symbol->type;
    if (type_out == global_types::primitive_type_void || type_out == global_types::primitive_type_const_void) {
      printf("%s:%d error:\n\tinvalid application of 'sizeof' to an incomplete type 'void' in unary_expression\n",
             input_file_name.c_str(), unary_expression->get_first_terminal_line_no());
      return 1;
    }
    //如果是enum struct union必须是complete type
    if (is_struct_union_enum_number(type_name)) {
      std::string type_name_identifier = *type_out->name;
      std::shared_ptr<tsc_type> type = lookup_type(context.current_symbol_table_node, type_name_identifier, true);
      if (!type) {
        printf("%s:%d error:\n\ttag '%s' not found in unary_expression\n", input_file_name.c_str(),
               unary_expression->get_first_terminal_line_no(), type_name_identifier.c_str());
        return 1;
      } else if (!type->is_complete) {
        printf("%s:%d error:\n\tsizeof incomplete type '%s' in unary_expression\n", input_file_name.c_str(),
               unary_expression->get_first_terminal_line_no(), type_name_identifier.c_str());
        return 1;
      }
    }

    unary_expression->symbol = std::make_shared<tsc_symbol>();
    unary_expression->symbol->symbol_type = SYMBOL_TYPE_ICONSTANT;
    unary_expression->symbol->type = global_types::primitive_type_sizeof;
    unary_expression->symbol->value = std::make_shared<expression_value>();
    unary_expression->symbol->value->size_value = type_out->type_size;
  }

  break;
  case NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_ALIGNOF_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS:
    printf("%s:%d error:\n\tunsupported C11 '_Alignof' in unary_expression\n", input_file_name.c_str(),
           unary_expression->get_first_terminal_line_no());
    return 1;
  }

  return semantics_analysis_result;
}

/*
type_name
	: specifier_qualifier_list abstract_declarator
	| specifier_qualifier_list
	;
 */
int analyze_type_name(std::shared_ptr<ast_node> type_name, semantics_analysis_context &context) {
  int semantics_analysis_result = 0;

  switch (type_name->node_sub_type) {
  case NODE_TYPE_TYPE_NAME_SUBTYPE_SPECIFIER_QUALIFIER_LIST_ABSTRACT_DECLARATOR: {
    std::shared_ptr<ast_node> specifier_qualifier_list = type_name->items[0];
    std::shared_ptr<ast_node> abstract_declarator = type_name->items[1];
    semantics_analysis_result = analyze_specifier_qualifier_list(specifier_qualifier_list, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    //todo abstract declarator
  } break;
  case NODE_TYPE_TYPE_NAME_SUBTYPE_SPECIFIER_QUALIFIER_LIST: {
    std::shared_ptr<ast_node> specifier_qualifier_list = type_name->items[0];

    semantics_analysis_result = analyze_specifier_qualifier_list(specifier_qualifier_list, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    type_name->symbol = specifier_qualifier_list->symbol;
  } break;
  }
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

  // 如果 postfix_expression 是一个 primary_expression 而这个 primary_expression 是一个 identifier
  // 我们必须知道我们要找的是函数还是变量(C中const并不是真的常量)
  // 10 + postfix_expression 那这个 postfix_expression 如果是 identifier 则是变量
  // 10 + postfix_expression() 那这个 postfix_expression 如果是 identifier 则是函数.在最外层的时候我们应该找一个变量
  std::shared_ptr<ast_node> next_postfix_expression;

  switch (postfix_expression->node_sub_type) {
  case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_PRIMARY_EXPRESSION: {
    std::shared_ptr<ast_node> primary_expression = postfix_expression->items[0];
    semantics_analysis_result = analyze_primary_expression(primary_expression, context, SYMBOL_TYPE_VARIABLE);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    postfix_expression->symbol = primary_expression->symbol;
  }

  break;
  // todo other situation
  // next_postfix_expression should be a function
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
int analyze_primary_expression(std::shared_ptr<ast_node> primary_expression, semantics_analysis_context &context,
                               int symbol_type_to_find) {
  int semantics_analysis_result = 0;

  switch (primary_expression->node_sub_type) {
  case NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_IDENTIFIER: {
    //这里的identifier可能是函数名,变量名或者类型名字.
    std::string symbol_identifier = *primary_expression->items[0]->lexeme;
    switch (symbol_type_to_find) {
    case SYMBOL_TYPE_VARIABLE: {
      std::shared_ptr<tsc_symbol> symbol =
          lookup_variable_symbol(context.current_symbol_table_node, symbol_identifier, true);
      if (!symbol) {
        printf("%s:%d error:\n\tidentifier '%s' not found in primary_expression\n", input_file_name.c_str(),
               primary_expression->get_first_terminal_line_no(), symbol_identifier.c_str());
        return 1;
      }
      primary_expression->symbol = symbol;
      if (!is_constant(primary_expression) && !primary_expression->symbol->memory_location)
        primary_expression->symbol->memory_location = std::make_shared<tsc_memory_location>();

    } break;
    case SYMBOL_TYPE_FUNCTION: {
      std::shared_ptr<tsc_symbol> symbol =
          lookup_function_symbol(context.current_symbol_table_node, symbol_identifier, true);
      if (!symbol) {
        printf("%s:%d error:\n\tfunction '%s' not found in primary_expression\n", input_file_name.c_str(),
               primary_expression->get_first_terminal_line_no(), symbol_identifier.c_str());
        return 1;
      }
      primary_expression->symbol = symbol;
    } break;
    default:
      printf("%s:%d error:\n\tshould not reach here\n", input_file_name.c_str(),
             primary_expression->get_first_terminal_line_no());
      semantics_analysis_result = 1;
    }

  }

  break;
  case NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_CONSTANT: {
    std::shared_ptr<ast_node> constant = primary_expression->items[0];
    semantics_analysis_result = analyze_constant(constant, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    primary_expression->symbol = constant->symbol;
  }

  break;

  case NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_STRING: {
    std::shared_ptr<ast_node> string_node = primary_expression->items[0];
    string_node->symbol = std::make_shared<tsc_symbol>();
    // "abc"的类型为char[4] 字符串字面量类型在c和cpp中不同.在cpp中是const char*
    // "abc"++ error: lvalue required as increment operand
    // 在cpp中的error则是 increment of read-only location ‘(const char*)"abc"’
    // "abc"[0]='u'; warning: assignment of read-only location ‘"abc"[0]’
    string_node->symbol->type = std::make_shared<tsc_type>();
    string_node->symbol->type->underlying_type = global_types::primitive_type_char;
    string_node->symbol->is_left_value = false;
    string_node->symbol->value = std::make_shared<expression_value>();
    string_node->symbol->value->string_value = std::make_shared<std::string>();
    semantics_analysis_result = analyze_string(string_node, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    string_node->symbol->type->array_length =
        std::make_shared<size_t>(string_node->symbol->value->string_value->length() + 1);

  }

  break;
  case NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_LEFT_PARENTHESIS_EXPRESSION_RIGHT_PARENTHESIS: {
    std::shared_ptr<ast_node> expression = primary_expression->items[1];

    semantics_analysis_result = analyze_expression(expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    primary_expression->symbol = expression->symbol;

  }

  break;

  case NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_GENERIC_SELECTION:
    printf("%s:%d error:\n\tunsupported C11 'generic selection' in primary_expression\n", input_file_name.c_str(),
           primary_expression->get_first_terminal_line_no());
    return 1;
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
  while (expression->node_type == NODE_TYPE_EXPRESSION &&
         expression->node_sub_type == NODE_TYPE_EXPRESSION_SUBTYPE_EXPRESSION_COMMA_ASSIGNMENT_EXPRESSION) {
    assignment_expressions.push_back(node->items[2]);
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

  //这里要额外检验expression是否含有多个assignment_expression 是则修改symbol_type;
  // int a[2,3]; ->syntax error condition_expression必须含有括号才可能含有逗号表达式
  // int a[(2,3)]; ->error: variably modified 'a' at file scope 含有逗号那就不是constant expression
  std::shared_ptr<tsc_symbol> symbol = assignment_expressions[assignment_expressions.size() - 1]->symbol;
  if (assignment_expressions.size() == 1)
    expression->symbol = symbol;
  else {
    expression->symbol = std::make_shared<tsc_symbol>(*symbol);
    expression->symbol->symbol_type = SYMBOL_TYPE_TEMPORARY_VARIABLE;
  }

  return semantics_analysis_result;
}

/*
assignment_expression
    : conditional_expression
    | unary_expression assignment_operator assignment_expression
    ;
 */

int analyze_assignment_expression(std::shared_ptr<ast_node> assignment_expression,
                                  semantics_analysis_context &context) {
  int semantics_analysis_result = 0;
  // int*p=&(a=1+2); error: a=1+2 is not lvalue

  switch (assignment_expression->node_sub_type) {
  case NODE_TYPE_ASSIGNMENT_EXPRESSION_SUBTYPE_CONDITIONAL_EXPRESSION: {
    std::shared_ptr<ast_node> conditional_expression = assignment_expression->items[0];
    semantics_analysis_result = analyze_conditional_expression(conditional_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;
    assignment_expression->symbol = conditional_expression->symbol;
  }

  break;
  case NODE_TYPE_ASSIGNMENT_EXPRESSION_SUBTYPE_UNARY_EXPRESSION_ASSIGNMENT_OPERATOR_ASSIGNMENT_EXPRESSION: {
    std::shared_ptr<ast_node> unary_expression = assignment_expression->items[0];
    std::shared_ptr<ast_node> assignment_expression = assignment_expression->items[2];

    semantics_analysis_result = analyze_unary_expression(unary_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    semantics_analysis_result = analyze_assignment_expression(assignment_expression, context);
    if (semantics_analysis_result)
      return semantics_analysis_result;

    //todo assignment_expression construct symbol
  }
  }

  return semantics_analysis_result;
}

/*
string
	: STRING_LITERAL
	| FUNC_NAME
	;
 */

int analyze_string(std::shared_ptr<ast_node> string_node, semantics_analysis_context &context) {
  std::shared_ptr<std::string> string_literal;
  int semantics_analysis_result = 0;
  switch (string_node->node_sub_type) {
  case NODE_TYPE_STRING_SUBTYPE_STRING_LITERAL:
    *string_node->symbol->value->string_value = extract_string(*string_node->items[0]->lexeme);
    string_node->symbol->memory_location = std::make_shared<tsc_memory_location>();
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
  constant->symbol = constant->items[0]->symbol;
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
    } else {
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

bool is_integer_constant(const std::shared_ptr<ast_node> &node) {
  return node->symbol->symbol_type == SYMBOL_TYPE_ICONSTANT ||
         node->symbol->symbol_type == SYMBOL_TYPE_ENUMERATION_CONSTANT;
}

bool is_floating_constant(const std::shared_ptr<ast_node> &node) {
  return node->symbol->symbol_type == SYMBOL_TYPE_FCONSTANT;
}

bool is_constant(const std::shared_ptr<ast_node> &node) {
  //注意不包括string literal.字符串字面量有内存地址,需要特殊处理
  return is_integer_constant(node) || is_floating_constant(node);
}
bool is_array_or_pointer(const std::shared_ptr<ast_node> &node) {
  return node->symbol->type->type_id == SCALAR_TYPE_ARRAY || node->symbol->type->type_id == SCALAR_TYPE_POINTER;
}

bool is_integer_or_floating_number(const std::shared_ptr<ast_node> &node) {
  return PRIMITIVE_TYPE_CHAR <= node->symbol->type->type_id &&
         node->symbol->type->type_id <= PRIMITIVE_TYPE_LONG_DOUBLE;
}
bool is_struct_union_enum_number(const std::shared_ptr<ast_node> &node) {
  switch (node->symbol->type->type_id) {
  case PRIMITIVE_TYPE_ENUM:
  case RECORD_TYPE_STRUCT_OR_UNION:
    return true;
  default:
    return false;
  }
}

bool is_integer(const std::shared_ptr<ast_node> &node) {
  return PRIMITIVE_TYPE_CHAR <= node->symbol->type->type_id &&
         node->symbol->type->type_id <= PRIMITIVE_TYPE_UNSIGNED_LONG_LONG;
}

//根据左右子表达式类型以及运算符表达式进行校验.如果是常量表达式则会求值
int construct_binary_expression_symbol(std::shared_ptr<ast_node> parent, int binary_operator,
                                       std::shared_ptr<ast_node> left, std::shared_ptr<ast_node> right) {
  std::shared_ptr<tsc_symbol> symbol = std::make_shared<tsc_symbol>();
  parent->symbol = symbol;
  parent->symbol->operator_id = binary_operator;
  parent->symbol->operands.push_back(left->symbol);
  parent->symbol->operands.push_back(right->symbol);
  symbol->value = std::make_shared<expression_value>();

  symbol->is_left_value = false;
  //两指针类型不能相加.同类型指针可以相减(不考虑const) int* - int* 类型为int. 指针与整数可以加减.
  //整型都提升为unsigned long long进行计算

  if (left->symbol->type->type_id == PRIMITIVE_TYPE_VOID) {
    printf("%s:%d error:\n\tinvalid left expression type void\n", input_file_name.c_str(),
           left->get_first_terminal_line_no());
    return 1;
  }
  if (right->symbol->type->type_id == PRIMITIVE_TYPE_VOID) {
    printf("%s:%d error:\n\tinvalid right expression type void\n", input_file_name.c_str(),
           left->get_first_terminal_line_no());
    return 1;
  }

  if (left->symbol->type->type_id == RECORD_TYPE_STRUCT_OR_UNION) {
    printf("%s:%d error:\n\tinvalid left expression type struct_or_union\n", input_file_name.c_str(),
           left->get_first_terminal_line_no());
    return 1;
  }
  if (right->symbol->type->type_id == RECORD_TYPE_STRUCT_OR_UNION) {
    printf("%s:%d error:\n\tinvalid right expression type struct_or_union\n", input_file_name.c_str(),
           left->get_first_terminal_line_no());
    return 1;
  }

  //如果left right都是数值型(包括enum)则结果也是数值型且type_id是left right中type_id较大那个.
  //如果left right较大的是enum结果处理为int.另外如果运算符是&&,||,>,>=,<,<=,==,!=结果类型为int
  int result_type_id;
  if (is_constant(left) && is_constant(right)) {
    result_type_id = std::max(left->symbol->type->type_id, right->symbol->type->type_id);

    switch (binary_operator) {
    case BINARY_OPERATOR_EQ_OP:
    case BINARY_OPERATOR_NE_OP:
    case BINARY_OPERATOR_GREATER_THAN:
    case BINARY_OPERATOR_LESS_THAN:
    case BINARY_OPERATOR_GREATER_THAN_OR_EQUAL:
    case BINARY_OPERATOR_LESS_THAN_OR_EQUAL:
    case BINARY_OPERATOR_AND:
    case BINARY_OPERATOR_OR:
      result_type_id = PRIMITIVE_TYPE_INT;
    default:
      break;
    }

    switch (result_type_id) {
    case PRIMITIVE_TYPE_CHAR:
      symbol->type = global_types::primitive_type_char;
      break;
    case PRIMITIVE_TYPE_UNSIGNED_CHAR:
      symbol->type = global_types::primitive_type_unsigned_char;
      break;
    case PRIMITIVE_TYPE_SHORT:
      symbol->type = global_types::primitive_type_short;
      break;
    case PRIMITIVE_TYPE_UNSIGNED_SHORT:
      symbol->type = global_types::primitive_type_unsigned_short;
      break;
    case PRIMITIVE_TYPE_ENUM:
      symbol->type = global_types::primitive_type_int;
      break;
    case PRIMITIVE_TYPE_INT:
      symbol->type = global_types::primitive_type_int;
      break;
    case PRIMITIVE_TYPE_UNSIGNED_INT:
      symbol->type = global_types::primitive_type_unsigned_int;
      break;
    case PRIMITIVE_TYPE_LONG:
      symbol->type = global_types::primitive_type_long;
      break;
    case PRIMITIVE_TYPE_UNSIGNED_LONG:
      symbol->type = global_types::primitive_type_unsigned_long;
      break;
    case PRIMITIVE_TYPE_LONG_LONG:
      symbol->type = global_types::primitive_type_long_long;
      break;
    case PRIMITIVE_TYPE_UNSIGNED_LONG_LONG:
      symbol->type = global_types::primitive_type_unsigned_long_long;
      break;
    case PRIMITIVE_TYPE_FLOAT:
      symbol->type = global_types::primitive_type_float;
      break;
    case PRIMITIVE_TYPE_DOUBLE:
      symbol->type = global_types::primitive_type_double;
      break;
    case PRIMITIVE_TYPE_LONG_DOUBLE:
      symbol->type = global_types::primitive_type_long_double;
      break;
    default:
      printf("should not reach here %s:%d\n", __FILE__, __LINE__);
      return 1;
    }
  }

  //对于整数除法和mod检查分母为0.对于gcc如果分子可能不为0只会给warning
  // int f(int i){ return i/0;} warning: division by zero [-Wdiv-by-zero]
  //如果是constant expression但是left是浮点型的,结果会提升为浮点型,此时不需要warning或者error

  if (PRIMITIVE_TYPE_CHAR <= left->symbol->type->type_id &&
      left->symbol->type->type_id <= PRIMITIVE_TYPE_UNSIGNED_LONG_LONG &&
      right->symbol->symbol_type == SYMBOL_TYPE_ICONSTANT && right->symbol->value->unsigned_long_long_value == 0ull) {
    switch (binary_operator) {
    case BINARY_OPERATOR_DIV:
    case BINARY_OPERATOR_MOD:
      if (left->symbol->symbol_type == SYMBOL_TYPE_ICONSTANT) {
        printf("%s:%d error:\n\tdiv or mod by zero %s\n", input_file_name.c_str(), right->get_first_terminal_line_no(),
               parent->get_expression().c_str());
        return 1;
      } else {
        printf("%s:%d warning:\n\tdiv or mod by zero %s\n", input_file_name.c_str(),
               right->get_first_terminal_line_no(), parent->get_expression().c_str());
      }
    default:
      break;
    }
  }

  // todo 实际上不能直接取union中的long long value(可能把负值int转为正值long long)另外有符号除法和无符号除法不是一个指令
  // 其它运算可以提升为unsigned long long
  // 数值强转.unsigned转为更高位数的signed类型则前面填0. signed转为更高位数signed则符号位移到最前面其它不动.移位规则同理.
  // 如果移位操作的right操作数是负的,结果与这个负数的除去符号位的部分有关,比较复杂. 例如char ch=0x10;ch<<-1结果还是0x10
  // 0x10<<-1结果是0x8.
  // char有8位.-1作为int所有32位都是1.低8位视为unsigned char结果为127.也就是char型左移-1等价于左移127.
  // 我们可以验证char型左移-128或者-2147483648总是不变(低8位全是0)所以移位的时候right用union中的signed还是unsigned值并没有影响
  // 在gcc上如果移位运算的right操作数为负的常量或者大于left的位数(例如left是long long则right>=64)会给warning.另外实际上right操作数只有低8位有效
  // 相关指令为shl(等价于sal)系列.如sall(left操作数为32位的时候是l.对应的后缀有bwlq不过似乎测试发现left是32位int或以下的时候都生成sall)
  // right操作数总是位于CL寄存器(RCX的低8位)
  if (is_integer_constant(left) && is_integer_constant(right)) {
    symbol->symbol_type = SYMBOL_TYPE_ICONSTANT;
    switch (binary_operator) {
    case BINARY_OPERATOR_ADD:
      symbol->value->long_long_value = left->symbol->value->long_long_value + right->symbol->value->long_long_value;
      break;
    case BINARY_OPERATOR_SUB:
      symbol->value->long_long_value = left->symbol->value->long_long_value - right->symbol->value->long_long_value;
      break;
    case BINARY_OPERATOR_MUL:
      symbol->value->long_long_value = left->symbol->value->long_long_value * right->symbol->value->long_long_value;
      break;
    case BINARY_OPERATOR_DIV:
      symbol->value->long_long_value = left->symbol->value->long_long_value / right->symbol->value->long_long_value;
      break;
    case BINARY_OPERATOR_MOD:
      symbol->value->long_long_value = left->symbol->value->long_long_value % right->symbol->value->long_long_value;
      break;
    case BINARY_OPERATOR_EQ_OP:
      symbol->value->long_long_value = left->symbol->value->long_long_value == right->symbol->value->long_long_value;
      break;
    case BINARY_OPERATOR_NE_OP:
      symbol->value->long_long_value = left->symbol->value->long_long_value != right->symbol->value->long_long_value;
      break;
    case BINARY_OPERATOR_GREATER_THAN:
      symbol->value->long_long_value = left->symbol->value->long_long_value > right->symbol->value->long_long_value;
      break;
    case BINARY_OPERATOR_LESS_THAN:
      symbol->value->long_long_value = left->symbol->value->long_long_value < right->symbol->value->long_long_value;
      break;
    case BINARY_OPERATOR_GREATER_THAN_OR_EQUAL:
      symbol->value->long_long_value = left->symbol->value->long_long_value >= right->symbol->value->long_long_value;
      break;
    case BINARY_OPERATOR_LESS_THAN_OR_EQUAL:
      symbol->value->long_long_value = left->symbol->value->long_long_value <= right->symbol->value->long_long_value;
      break;
    case BINARY_OPERATOR_AND:
      symbol->value->long_long_value = left->symbol->value->long_long_value && right->symbol->value->long_long_value;
      break;
    case BINARY_OPERATOR_OR:
      symbol->value->long_long_value = left->symbol->value->long_long_value || right->symbol->value->long_long_value;
      break;
    case BINARY_OPERATOR_BITAND:
      symbol->value->long_long_value = left->symbol->value->long_long_value & right->symbol->value->long_long_value;
      break;
    case BINARY_OPERATOR_BITOR:
      symbol->value->long_long_value = left->symbol->value->long_long_value | right->symbol->value->long_long_value;
      break;
    case BINARY_OPERATOR_BITXOR:
      symbol->value->long_long_value = left->symbol->value->long_long_value ^ right->symbol->value->long_long_value;
      break;
    case BINARY_OPERATOR_LEFT_SHIFT:
      symbol->value->long_long_value = left->symbol->value->long_long_value << right->symbol->value->long_long_value;
      break;
    case BINARY_OPERATOR_RIGHT_SHIFT:
      symbol->value->long_long_value = left->symbol->value->long_long_value >> right->symbol->value->long_long_value;
      break;
    default:
      printf("should not reach here %s:%d\n", __FILE__, __LINE__);
      return 1;
    }
  }

  // todo 浮点型运算(包括整数与浮点数运算),指针,数组运算的校验.
  // gcc & clang: void*p; p+1; ->OK. offset is 1

  return 0;
}

int construct_unary_expression_symbol(std::shared_ptr<ast_node> parent, int unary_operator,
                                      std::shared_ptr<ast_node> operand) {
  //先置为临时变量.如果发现是 constant expression 再进行修改
  parent->symbol->symbol_type = SYMBOL_TYPE_TEMPORARY_VARIABLE;
  parent->symbol->type = operand->symbol->type;
  parent->symbol->value = std::make_shared<expression_value>();

  if (is_integer_constant(operand)) {
    parent->symbol->symbol_type = SYMBOL_TYPE_ICONSTANT;
    switch (unary_operator) {

    case UNARY_OPERATOR_ADD:
      parent->symbol->value = operand->symbol->value;
      break;
    case UNARY_OPERATOR_SUB:
      parent->symbol->value->long_long_value = -operand->symbol->value->long_long_value;
      break;
    case UNARY_OPERATOR_BITNOT:
      parent->symbol->value->long_long_value = ~operand->symbol->value->long_long_value;
      break;
    case UNARY_OPERATOR_NOT:
      parent->symbol->type = global_types::primitive_type_int;
      parent->symbol->value->int_value = !operand->symbol->value->long_long_value;
      break;
    default:
      printf("should not reach here %s:%d\n", __FILE__, __LINE__);
      return 1;
    }
  } else if (is_floating_constant(operand)) {
    parent->symbol->symbol_type = SYMBOL_TYPE_FCONSTANT;
    switch (unary_operator) {

    case UNARY_OPERATOR_ADD:
      parent->symbol->value = operand->symbol->value;
      break;
    case UNARY_OPERATOR_SUB:
      parent->symbol->value->double_value = -operand->symbol->value->double_value;
      break;

    case UNARY_OPERATOR_NOT:
      parent->symbol->type = global_types::primitive_type_int;
      parent->symbol->value->int_value = !operand->symbol->value->double_value;
      break;
    default:
      printf("should not reach here %s:%d\n", __FILE__, __LINE__);
      return 1;
    }
  }

  return 0;
}

/*
init_declarator_list
	: init_declarator
	| init_declarator_list ',' init_declarator
	;
 */
int analyze_init_declarator_list(std::shared_ptr<ast_node> init_declarator_list, semantics_analysis_context &context) {
  return 0;
}
/*
specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	| type_qualifier specifier_qualifier_list
	| type_qualifier
	;
 */
int analyze_specifier_qualifier_list(std::shared_ptr<ast_node> specifier_qualifier_list,
                                     semantics_analysis_context &context) {

  std::vector<std::shared_ptr<ast_node>> type_specifiers;
  std::vector<std::shared_ptr<ast_node>> type_qualifiers;

  std::shared_ptr<ast_node> node = specifier_qualifier_list;

  int semantics_analysis_result = 0;

  while (node->node_type == NODE_TYPE_SPECIFIER_QUALIFIER_LIST) {
    bool has_next = true;

    switch (node->node_sub_type) {
    case NODE_TYPE_SPECIFIER_QUALIFIER_LIST_SUBTYPE_TYPE_SPECIFIER_SPECIFIER_QUALIFIER_LIST:
      type_specifiers.push_back(node->items[0]);
      node = node->items[1];
      break;

    case NODE_TYPE_SPECIFIER_QUALIFIER_LIST_SUBTYPE_TYPE_SPECIFIER:
      type_specifiers.push_back(node->items[0]);
      has_next = false;
      break;

    case NODE_TYPE_SPECIFIER_QUALIFIER_LIST_SUBTYPE_TYPE_QUALIFIER_TYPE_SPECIFIER_QUALIFIER_LIST:
      type_qualifiers.push_back(node->items[0]);
      node = node->items[1];
      break;

    case NODE_TYPE_SPECIFIER_QUALIFIER_LIST_SUBTYPE_TYPE_QUALIFIER:
      type_qualifiers.push_back(node->items[0]);
      has_next = false;
      break;
    }
    if (!has_next)
      break;
  }
  std::shared_ptr<tsc_symbol> symbol = std::make_shared<tsc_symbol>();
  symbol->type = std::make_shared<tsc_type>();
  semantics_analysis_result = check_type_specifiers(type_specifiers, context, symbol);
  if (semantics_analysis_result)
    return semantics_analysis_result;

  semantics_analysis_result = check_type_qualifiers(type_qualifiers, context, symbol);
  if (semantics_analysis_result)
    return semantics_analysis_result;
  specifier_qualifier_list->symbol = symbol;
  return 0;
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
  global_types::primitive_type_char->type_size = sizeof(char);

  global_types::primitive_type_unsigned_char->type_id = PRIMITIVE_TYPE_UNSIGNED_CHAR;
  global_types::primitive_type_unsigned_char->type_size = sizeof(unsigned char);

  global_types::primitive_type_short->type_id = PRIMITIVE_TYPE_SHORT;
  global_types::primitive_type_short->type_size = sizeof(short);

  global_types::primitive_type_unsigned_short->type_id = PRIMITIVE_TYPE_UNSIGNED_SHORT;
  global_types::primitive_type_unsigned_short->type_size = sizeof(unsigned short);

  global_types::primitive_type_int->type_id = PRIMITIVE_TYPE_INT;
  global_types::primitive_type_int->type_size = sizeof(int);

  global_types::primitive_type_unsigned_int->type_id = PRIMITIVE_TYPE_UNSIGNED_INT;
  global_types::primitive_type_unsigned_int->type_size = sizeof(unsigned int);

  global_types::primitive_type_long->type_id = PRIMITIVE_TYPE_LONG;
  global_types::primitive_type_long->type_size = sizeof(long);

  global_types::primitive_type_unsigned_long->type_id = PRIMITIVE_TYPE_UNSIGNED_LONG;
  global_types::primitive_type_unsigned_long->type_size = sizeof(unsigned long);

  global_types::primitive_type_long_long->type_id = PRIMITIVE_TYPE_LONG_LONG;
  global_types::primitive_type_long_long->type_size = sizeof(long long);

  global_types::primitive_type_unsigned_long_long->type_id = PRIMITIVE_TYPE_UNSIGNED_LONG_LONG;
  global_types::primitive_type_unsigned_long_long->type_size = sizeof(unsigned long long);

  global_types::primitive_type_float->type_id = PRIMITIVE_TYPE_FLOAT;
  global_types::primitive_type_float->type_size = sizeof(float);

  global_types::primitive_type_double->type_id = PRIMITIVE_TYPE_DOUBLE;
  global_types::primitive_type_double->type_size = sizeof(double);

  global_types::primitive_type_long_double->type_id = PRIMITIVE_TYPE_LONG_DOUBLE;
  global_types::primitive_type_long_double->type_size = sizeof(long double);

  global_types::primitive_type_const_void->type_id = PRIMITIVE_TYPE_VOID;
  //gcc extension sizeof(void)=1 clang say it's error

  global_types::primitive_type_const_char->type_id = PRIMITIVE_TYPE_CHAR;
  global_types::primitive_type_const_char->type_size = sizeof(char);

  global_types::primitive_type_const_unsigned_char->type_id = PRIMITIVE_TYPE_UNSIGNED_CHAR;
  global_types::primitive_type_const_unsigned_char->type_size = sizeof(unsigned char);

  global_types::primitive_type_const_short->type_id = PRIMITIVE_TYPE_SHORT;
  global_types::primitive_type_const_short->type_size = sizeof(short);

  global_types::primitive_type_const_unsigned_short->type_id = PRIMITIVE_TYPE_UNSIGNED_SHORT;
  global_types::primitive_type_const_unsigned_short->type_size = sizeof(unsigned short);

  global_types::primitive_type_const_int->type_id = PRIMITIVE_TYPE_INT;
  global_types::primitive_type_const_int->type_size = sizeof(int);

  global_types::primitive_type_const_unsigned_int->type_id = PRIMITIVE_TYPE_UNSIGNED_INT;
  global_types::primitive_type_const_unsigned_int->type_size = sizeof(unsigned int);

  global_types::primitive_type_const_long->type_id = PRIMITIVE_TYPE_LONG;
  global_types::primitive_type_const_long->type_size = sizeof(long);

  global_types::primitive_type_const_unsigned_long->type_id = PRIMITIVE_TYPE_UNSIGNED_LONG;
  global_types::primitive_type_const_unsigned_long->type_size = sizeof(unsigned long);

  global_types::primitive_type_const_long_long->type_id = PRIMITIVE_TYPE_LONG_LONG;
  global_types::primitive_type_const_long_long->type_size = sizeof(long long);

  global_types::primitive_type_const_unsigned_long_long->type_id = PRIMITIVE_TYPE_UNSIGNED_LONG_LONG;
  global_types::primitive_type_const_unsigned_long_long->type_size = sizeof(unsigned long long);

  global_types::primitive_type_const_float->type_id = PRIMITIVE_TYPE_FLOAT;
  global_types::primitive_type_const_float->type_size = sizeof(float);

  global_types::primitive_type_const_double->type_id = PRIMITIVE_TYPE_DOUBLE;
  global_types::primitive_type_const_double->type_size = sizeof(double);

  global_types::primitive_type_const_long_double->type_id = PRIMITIVE_TYPE_LONG_DOUBLE;
  global_types::primitive_type_const_long_double->type_size = sizeof(long double);

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

  switch (sizeof(size_t)) {
  case 4:
    global_types::primitive_type_sizeof = global_types::primitive_type_unsigned_int;
    break;
  case 8:
  default:
    global_types::primitive_type_sizeof = global_types::primitive_type_unsigned_long_long;
    break;
  }
}

std::shared_ptr<tsc_type> construct_pointer_to(std::shared_ptr<tsc_type> type) {
  std::shared_ptr<tsc_type> pointer = std::make_shared<tsc_type>();
  pointer->type_id = SCALAR_TYPE_POINTER;
  pointer->underlying_type = type;
  return pointer;
}

std::shared_ptr<tsc_symbol> lookup_variable_symbol(std::shared_ptr<symbol_table_node> symbol_table_node,
                                                   const std::string &symbol_identifier, bool search_outer) {

  if (symbol_table_node->identifier_and_symbols.find(symbol_identifier) !=
      symbol_table_node->identifier_and_symbols.end())
    return symbol_table_node->identifier_and_symbols[symbol_identifier];
  if (search_outer && symbol_table_node->parent)
    return lookup_variable_symbol(symbol_table_node->parent, symbol_identifier, search_outer);
  else
    return std::shared_ptr<tsc_symbol>();
}
std::shared_ptr<tsc_symbol> lookup_function_symbol(std::shared_ptr<symbol_table_node> symbol_table_node,
                                                   const std::string &symbol_identifier, bool search_outer) {
  if (symbol_table_node->functions.find(symbol_identifier) != symbol_table_node->identifier_and_symbols.end())
    return symbol_table_node->identifier_and_symbols[symbol_identifier];
  if (search_outer && symbol_table_node->parent)
    return lookup_variable_symbol(symbol_table_node->parent, symbol_identifier, search_outer);
  else
    return std::shared_ptr<tsc_symbol>();
}

std::shared_ptr<tsc_type> lookup_type(std::shared_ptr<symbol_table_node> symbol_table_node,
                                      const std::string &type_name, bool search_outer) {
  if (symbol_table_node->struct_union_enum_names.find(type_name) != symbol_table_node->struct_union_enum_names.end())
    return symbol_table_node->struct_union_enum_names[type_name];
  if (search_outer && symbol_table_node->parent)
    return lookup_type(symbol_table_node->parent, type_name, search_outer);
  else
    return std::shared_ptr<tsc_type>();
}
