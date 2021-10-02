#include "semantics_analysis.h"
#include <set>
#include <cstddef>
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
            semantics_analysis_result = analyze_declaration_specifiers(declaration_specifiers, context, symbol,
                                                                       DECLARATION_SPECIFIERS_LOCATION_DECLARATION);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            // 如果没有init_declarator_list说明是声明一个类型如struct A{...};此时如果不是匿名enum_struct_union需要加入tags
            // primitive类型也没有name 这里如果之前已经有过同名的struct union enum了,需要检查之前的是否是incomplete.如果是才用当前定义覆盖.
            if (symbol->type->internal_name)
                context.current_symbol_table_node->struct_union_enum_names[*symbol->type->internal_name] = symbol->type;

        } break;
        case NODE_TYPE_DECLARATION_SUBTYPE_DECLARATION_SPECIFIERS_INIT_DECLARATOR_LIST_SEMI_COLON: {

            std::shared_ptr<ast_node> declaration_specifiers = declaration->items[0];
            std::shared_ptr<ast_node> init_declarator_list = declaration->items[1];
            semantics_analysis_result = analyze_declaration_specifiers(declaration_specifiers, context, symbol,
                                                                       DECLARATION_SPECIFIERS_LOCATION_DECLARATION);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            semantics_analysis_result = analyze_init_declarator_list(init_declarator_list, context, symbol);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            //如果有init_declarator_list,可能是声明一个类型如struct A{...} a;此时也需要加入tags且A必须是complete type
            //注意指针变量的声明 struct A *p;则A可以是incomplete
            // todo for global variables, initializer must be constant

        } break;
        case NODE_TYPE_DECLARATION_SUBTYPE_STATIC_ASSERT_DECLARATION:
            printf("%s:%d error:\n\tstatic_assert_declaration not supported\n", input_file_name.c_str(),
                   declaration->get_first_terminal_line_no());
            return 1;
    }

    return 0;
}

/*
function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement
	;

declaration_list
	: declaration
	| declaration_list declaration
	;
*/
int analyze_function_definition(std::shared_ptr<ast_node> function_definition, semantics_analysis_context &context) {

    // declarator的最底层declarator必须是函数.形如 int a{}也符合文法但不是合法的function_definition
    // 处理 K&R style
    int semantics_analysis_result = 0;
    std::shared_ptr<ast_node> declaration_specifiers = function_definition->items[0];
    std::shared_ptr<ast_node> declarator = function_definition->items[1];
    declarator->symbol = std::make_shared<tsc_symbol>();
    declarator->symbol->type = std::make_shared<tsc_type>();
    std::shared_ptr<ast_node> function_identifier_node;

    semantics_analysis_result = analyze_declaration_specifiers(declaration_specifiers, context, declarator->symbol,
                                                               DECLARATION_SPECIFIERS_LOCATION_DECLARATION);
    if (semantics_analysis_result)
        return semantics_analysis_result;
    semantics_analysis_result = analyze_declarator(declarator, context, function_identifier_node);

    if (semantics_analysis_result)
        return semantics_analysis_result;
    std::shared_ptr<tsc_symbol> function_symbol = function_identifier_node->symbol;
    if (function_symbol->symbol_type != SYMBOL_TYPE_FUNCTION) {
        printf("%s:%d error:\n\texpecting function declarator\n", input_file_name.c_str(),
               declarator->get_first_terminal_line_no());
        return 1;
    }

    std::shared_ptr<symbol_table_node> function_compound_statement_symbol_table_node =
            std::make_shared<symbol_table_node>();
    function_compound_statement_symbol_table_node->parent =
            function_symbol->type->function_signature->parameters_symbol_table_node;
    std::shared_ptr<symbol_table_node> original_symbol_table_node = context.current_symbol_table_node;

    std::shared_ptr<tsc_function> function = std::make_shared<tsc_function>();
    function->function_symbol = function_symbol;
    function->name = *function_symbol->identifier;
    //todo check compatibility with previous declaration or definition

    for (std::shared_ptr<tsc_function> previous_defined_function : context.functions) {
        if (previous_defined_function->name == function->name) {
            printf("%s:%d error:\n\tredefinition of function '%s'\n", input_file_name.c_str(),
                   declarator->get_first_terminal_line_no(), function->name.c_str());
            return 1;
        }
    }
    //为了递归调用,现在加入符号表.注意这一步要在切换符号表节点之前,保证加入的是全局符号表
    semantics_analysis_result = add_declarator_identifier_to_symbol_table(declarator, context, function_identifier_node);
    if (semantics_analysis_result)
        return semantics_analysis_result;

    context.current_symbol_table_node = function_compound_statement_symbol_table_node;
    //nullptr

    switch (function_definition->node_sub_type) {
        case NODE_TYPE_FUNCTION_DEFINITION_SUBTYPE_DECLARATION_SPECIFIERS_DECLARATOR_DECLARATION_LIST_COMPOUND_STATEMENT: {
            std::shared_ptr<ast_node> declaration_list = function_definition->items[2];
            std::shared_ptr<ast_node> compound_statement = function_definition->items[3];
            function->compound_statement_node = compound_statement;
            compound_statement->statement_context = std::make_shared<tsc_statement_context>();
            std::vector<std::shared_ptr<ast_node>> declarations;
            std::shared_ptr<ast_node> node = declaration_list;
            while (node->node_type == NODE_TYPE_DECLARATION_LIST &&
                   node->node_sub_type == NODE_TYPE_DECLARATION_LIST_SUBTYPE_DECLARATION_LIST_DECLARATION) {
                declarations.push_back(node->items[1]);
                node = node->items[0];
            }
            // K&R style跟在参数列表后面的declaration必须有declarator int f(a)double; {} ->error
            // 并且此时前面的声明必须是K&R的(has_proto=false)
            std::shared_ptr<tsc_function_signature> function_signature = function_symbol->type->function_signature;
            if (function_signature->has_proto) {
                printf("%s:%d error:\n\tinvalid K&R style function declarator\n", input_file_name.c_str(),
                       declarator->get_first_terminal_line_no());
                return 1;
            }

            declarations.push_back(node->items[0]);
            declarations = std::vector<std::shared_ptr<ast_node>>(declarations.rbegin(), declarations.rend());

            std::set<std::string> parsed_parameter_identifiers;
            for (std::shared_ptr<ast_node> declaration : declarations) {
                std::shared_ptr<tsc_symbol> symbol = std::make_shared<tsc_symbol>();
                symbol->type = std::make_shared<tsc_type>();
                semantics_analysis_result = analyze_declaration(declaration, context, symbol);

                if (semantics_analysis_result)
                    return semantics_analysis_result;
                //declaration has form declaration_specifiers init_declarator_list ';'
                if (declaration->node_sub_type == NODE_TYPE_DECLARATION_SUBTYPE_DECLARATION_SPECIFIERS_SEMI_COLON) {
                    printf("%s:%d error:\n\tdeclaration '%s' does not declare a parameter\n", input_file_name.c_str(),
                           declaration->get_first_terminal_line_no(), declaration->get_expression().c_str());
                    return 1;
                }

                std::shared_ptr<ast_node> init_declarator_list = declaration->items[1];
                //initializer not allowed
                for (std::shared_ptr<ast_node> init_declarator : init_declarator_list->sub_nodes) {
                    if (init_declarator->node_sub_type == NODE_TYPE_INIT_DECLARATOR_SUBTYPE_DECLARATOR_ASSIGN_INITIALIZER) {
                        printf("%s:%d error:\n\t initializer '%s' not allowed in K&R function definition\n", input_file_name.c_str(),
                               declaration->get_first_terminal_line_no(), init_declarator->get_expression().c_str());
                        return 1;
                    }
                }

                for (size_t index = 0; index < init_declarator_list->declarator_identifier_nodes.size(); index++) {

                    std::shared_ptr<ast_node> declarator_identifier_node = init_declarator_list->declarator_identifier_nodes[index];
                    std::shared_ptr<ast_node> init_declarator = init_declarator_list->sub_nodes[index];
                    std::string parameter_identifier = *declarator_identifier_node->lexeme;

                    //void func()int a;{} ->error
                    if (std::find(function_signature->identifiers.begin(), function_signature->identifiers.end(),
                                  parameter_identifier) == function_signature->identifiers.end()) {
                        printf("%s:%d error:\n\t parameter named '%s' is missing\n", input_file_name.c_str(),
                               declaration->get_first_terminal_line_no(), parameter_identifier.c_str());
                        return 1;
                    }

                    // no need to check following error. already checked in analyze_declaration
                    // void func(a)int a;double a{}
                    // analyze_declaration 会将'a'加入符号表
                }
            }

            semantics_analysis_result = analyze_compound_statement(compound_statement, context, function);

        } break;
        case NODE_TYPE_FUNCTION_DEFINITION_SUBTYPE_DECLARATION_SPECIFIERS_DECLARATOR_COMPOUND_STATEMENT: {
            // possibly K&R style with no parameters void func(){}
            std::shared_ptr<ast_node> compound_statement = function_definition->items[2];
            function->compound_statement_node = compound_statement;
            compound_statement->statement_context = std::make_shared<tsc_statement_context>();
            semantics_analysis_result = analyze_compound_statement(compound_statement, context, function);

        } break;
    }
    //恢复符号表节点
    context.current_symbol_table_node = original_symbol_table_node;
    context.functions.push_back(function);
    return semantics_analysis_result;
}

/*
compound_statement
	: '{' '}'
	| '{'  block_item_list '}'
	;

block_item_list
	: block_item
	| block_item_list block_item
	;

block_item
	: declaration
	| statement
	;
*/
int analyze_compound_statement(std::shared_ptr<ast_node> compound_statement, semantics_analysis_context &context,
                               std::shared_ptr<tsc_function> function) {
    int semantics_analysis_result = 0;
    std::vector<std::shared_ptr<ast_node>> declaration_or_statements;
    switch (compound_statement->node_sub_type) {
        case NODE_TYPE_COMPOUND_STATEMENT_SUBTYPE_LEFT_BRACE_RIGHT_BRACE:
            break;
        case NODE_TYPE_COMPOUND_STATEMENT_SUBTYPE_LEFT_BRACE_BLOCK_ITEM_LIST_RIGHT_BRACE: {
            std::shared_ptr<ast_node> block_item_list = compound_statement->items[1];
            std::shared_ptr<ast_node> node = block_item_list;

            while (node->node_type == NODE_TYPE_BLOCK_ITEM_LIST &&
                   node->node_sub_type == NODE_TYPE_BLOCK_ITEM_LIST_SUBTYPE_BLOCK_ITEM_LIST_BLOCK_ITEM) {
                declaration_or_statements.push_back(node->items[1]);
                node = node->items[0];
            }

            declaration_or_statements.push_back(node->items[0]);
            declaration_or_statements =
                    std::vector<std::shared_ptr<ast_node>>(declaration_or_statements.rbegin(), declaration_or_statements.rend());
            compound_statement->sub_nodes = declaration_or_statements;
            for (std::shared_ptr<ast_node> declaration_or_statement : declaration_or_statements) {

                switch (declaration_or_statement->node_sub_type) {
                    case NODE_TYPE_BLOCK_ITEM_SUBTYPE_DECLARATION: {
                        std::shared_ptr<ast_node> declaration = declaration_or_statement->items[0];
                        std::shared_ptr<tsc_symbol> symbol = std::make_shared<tsc_symbol>();
                        symbol->type = std::make_shared<tsc_type>();
                        semantics_analysis_result = analyze_declaration(declaration, context, symbol);
                        if (semantics_analysis_result)
                            return semantics_analysis_result;
                    } break;

                    case NODE_TYPE_BLOCK_ITEM_SUBTYPE_STATEMENT: {

                        std::shared_ptr<ast_node> statement = declaration_or_statement->items[0];
                        statement->statement_context = std::make_shared<tsc_statement_context>();
                        statement->statement_context->parent_statement_context = compound_statement->statement_context;
                        semantics_analysis_result = analyze_statement(statement, context, function);
                        if (semantics_analysis_result)
                            return semantics_analysis_result;
                    } break;
                }
            }
        }
    }
    return 0;
}

/*
statement
: labeled_statement
| compound_statement
| expression_statement
| selection_statement
| iteration_statement
| jump_statement
;
*/

int analyze_statement(std::shared_ptr<ast_node> statement, semantics_analysis_context &context,
                      std::shared_ptr<tsc_function> function) {
    int semantics_analysis_result = 0;
    //statement->statement_context已经设置好.
    switch (statement->node_sub_type) {
        case NODE_TYPE_STATEMENT_SUBTYPE_LABELED_STATEMENT: {
            std::shared_ptr<ast_node> labeled_statement = statement->items[0];
            labeled_statement->statement_context = statement->statement_context;
            labeled_statement->statement_context->statement_type = STATEMENT_TYPE_LABELED_STATEMENT;
            semantics_analysis_result = analyze_labeled_statement(labeled_statement, context, function);
        } break;
        case NODE_TYPE_STATEMENT_SUBTYPE_COMPOUND_STATEMENT: {
            std::shared_ptr<ast_node> compound_statement = statement->items[0];
            compound_statement->statement_context = statement->statement_context;
            compound_statement->statement_context->statement_type = STATEMENT_TYPE_COMPOUND_STATEMENT;

            std::shared_ptr<symbol_table_node> original_symbol_table_node = context.current_symbol_table_node;
            context.current_symbol_table_node = std::make_shared<symbol_table_node>();
            context.current_symbol_table_node->parent = original_symbol_table_node;
            semantics_analysis_result = analyze_compound_statement(compound_statement, context, function);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            context.current_symbol_table_node = original_symbol_table_node;
        } break;
        case NODE_TYPE_STATEMENT_SUBTYPE_EXPRESSION_STATEMENT: {
            std::shared_ptr<ast_node> expression_statement = statement->items[0];
            expression_statement->statement_context = statement->statement_context;
            expression_statement->statement_context->statement_type = STATEMENT_TYPE_EXPRESSION_STATEMENT;

            semantics_analysis_result = analyze_expression_statement(expression_statement, context, function);
        } break;
        case NODE_TYPE_STATEMENT_SUBTYPE_SELECTION_STATEMENT: {
            std::shared_ptr<ast_node> selection_statement = statement->items[0];
            selection_statement->statement_context = statement->statement_context;
            selection_statement->statement_context->statement_type = STATEMENT_TYPE_SELECTION_STATEMENT;

            semantics_analysis_result = analyze_selection_statement(selection_statement, context, function);
        } break;
        case NODE_TYPE_STATEMENT_SUBTYPE_ITERATION_STATEMENT: {
            std::shared_ptr<ast_node> iteration_statement = statement->items[0];
            iteration_statement->statement_context = statement->statement_context;
            iteration_statement->statement_context->statement_type = STATEMENT_TYPE_ITERATION_STATEMENT;

            semantics_analysis_result = analyze_iteration_statement(iteration_statement, context, function);
        } break;
        case NODE_TYPE_STATEMENT_SUBTYPE_JUMP_STATEMENT: {
            std::shared_ptr<ast_node> jump_statement = statement->items[0];
            jump_statement->statement_context = statement->statement_context;
            jump_statement->statement_context->statement_type = STATEMENT_TYPE_JUMP_STATEMENT;

            semantics_analysis_result = analyze_jump_statement(jump_statement, context, function);
        } break;
    }
    return semantics_analysis_result;
}

/*
labeled_statement
	: IDENTIFIER ':' statement
	| CASE constant_expression ':' statement
	| DEFAULT ':' statement
	;
*/

int analyze_labeled_statement(std::shared_ptr<ast_node> labeled_statement, semantics_analysis_context &context,
                              std::shared_ptr<tsc_function> function) {
    int semantics_analysis_result = 0;

    switch (labeled_statement->node_sub_type) {
        case NODE_TYPE_LABELED_STATEMENT_SUBTYPE_IDENTIFIER_COLON_STATEMENT: {
            // label names shall be unique within a function.
            std::string identifier = *labeled_statement->items[0]->lexeme;
            std::shared_ptr<ast_node> statement = labeled_statement->items[2];
            statement->statement_context = std::make_shared<tsc_statement_context>();
            statement->statement_context->parent_statement_context = labeled_statement->statement_context;

            if (function->labels.find(identifier) != function->labels.end()) {
                printf("%s:%d error:\n\tredefinition of label '%s'\n", input_file_name.c_str(),
                       labeled_statement->get_first_terminal_line_no(), identifier.c_str());
                return 1;
            }

            semantics_analysis_result = analyze_statement(statement, context, function);

        } break;
        case NODE_TYPE_LABELED_STATEMENT_SUBTYPE_CASE_CONSTANT_EXPRESSION_COLON_STATEMENT:
        case NODE_TYPE_LABELED_STATEMENT_SUBTYPE_DEFAULT_COLON_STATEMENT: {
            //这里注意 labeled_statement 可能在一到多个 compound_statement 中.不能直接用parent语句判断
            std::shared_ptr<tsc_statement_context> parent_switch_statement_context_candidate =
                    labeled_statement->statement_context->parent_statement_context;
            while (parent_switch_statement_context_candidate &&
                   (parent_switch_statement_context_candidate->statement_type != STATEMENT_TYPE_SELECTION_STATEMENT ||
                    parent_switch_statement_context_candidate->statement_sub_type != STATEMENT_SUB_TYPE_SWITCH)) {
                parent_switch_statement_context_candidate = parent_switch_statement_context_candidate->parent_statement_context;
            }
            if (!parent_switch_statement_context_candidate) {
                printf("%s:%d error:\n\t'case' or 'default' statement not in loop or switch statement\n", input_file_name.c_str(),
                       labeled_statement->get_first_terminal_line_no());
                return 1;
            }

            if (labeled_statement->node_sub_type ==
                NODE_TYPE_LABELED_STATEMENT_SUBTYPE_CASE_CONSTANT_EXPRESSION_COLON_STATEMENT) {
                // CASE constant_expression ':' statement
                std::shared_ptr<ast_node> constant_expression = labeled_statement->items[1];
                std::shared_ptr<ast_node> statement = labeled_statement->items[3];
                statement->statement_context = std::make_shared<tsc_statement_context>();
                statement->statement_context->parent_statement_context = labeled_statement->statement_context;

                semantics_analysis_result = analyze_constant_expression(constant_expression, context);
                if (semantics_analysis_result)
                    return semantics_analysis_result;

                int case_constant_value = constant_expression->symbol->value->int_value;
                std::set<int> &case_constant_values =
                        parent_switch_statement_context_candidate->switch_statement_context->case_constant_values;
                if (case_constant_values.find(case_constant_value) != case_constant_values.end()) {
                    printf("%s:%d error:\n\tduplicate case value %d\n", input_file_name.c_str(),
                           labeled_statement->get_first_terminal_line_no(), case_constant_value);
                    return 1;
                } else {
                    case_constant_values.insert(case_constant_value);
                }
                semantics_analysis_result = semantics_analysis_result = analyze_statement(statement, context, function);
            } else {
                // DEFAULT ':' statement
                std::shared_ptr<ast_node> statement = labeled_statement->items[2];
                semantics_analysis_result = semantics_analysis_result = analyze_statement(statement, context, function);
            }

        } break;
    }

    return semantics_analysis_result;
}

/*
expression_statement
	: ';'
	| expression ';'
	;

*/
int analyze_expression_statement(std::shared_ptr<ast_node> expression_statement, semantics_analysis_context &context,
                                 std::shared_ptr<tsc_function> function) {
    int semantics_analysis_result = 0;
    expression_statement->statement_context = std::make_shared<tsc_statement_context>();
    expression_statement->statement_context->statement_type = STATEMENT_TYPE_EXPRESSION_STATEMENT;
    switch (expression_statement->node_sub_type) {
        case NODE_TYPE_EXPRESSION_STATEMENT_SUBTYPE_SEMI_COLON:
            break;
        case NODE_TYPE_EXPRESSION_STATEMENT_SUBTYPE_EXPRESSION_SEMI_COLON: {
            std::shared_ptr<ast_node> expression = expression_statement->items[0];
            semantics_analysis_result = analyze_expression(expression, context);
        }
    }

    return semantics_analysis_result;
}

/*
selection_statement
	: IF '(' expression ')' statement ELSE statement
	| IF '(' expression ')' statement
	| SWITCH '(' expression ')' statement
	;
*/
int analyze_selection_statement(std::shared_ptr<ast_node> selection_statement, semantics_analysis_context &context,
                                std::shared_ptr<tsc_function> function) {
    int semantics_analysis_result = 0;
    switch (selection_statement->node_sub_type) {
        case NODE_TYPE_SELECTION_STATEMENT_SUBTYPE_IF_LEFT_PARENTHESIS_EXPRESSION_RIGHT_PARENTHESIS_STATEMENT_ELSE_STATEMENT: {
            //IF '(' expression ')' statement ELSE statement
            selection_statement->statement_context->statement_sub_type = STATEMENT_SUB_TYPE_IF_ELSE;

            std::shared_ptr<ast_node> expression = selection_statement->items[2];
            std::shared_ptr<ast_node> statement1 = selection_statement->items[4];
            std::shared_ptr<ast_node> statement2 = selection_statement->items[6];

            statement1->statement_context = std::make_shared<tsc_statement_context>();
            statement1->statement_context->parent_statement_context = selection_statement->statement_context;
            statement2->statement_context = std::make_shared<tsc_statement_context>();
            statement2->statement_context->parent_statement_context = selection_statement->statement_context;

            // address of function 'main' will always evaluate to 'true'
            semantics_analysis_result = analyze_statement(statement1, context, function);

            if (semantics_analysis_result)
                return semantics_analysis_result;
            semantics_analysis_result = analyze_statement(statement2, context, function);

        } break;
        case NODE_TYPE_SELECTION_STATEMENT_SUBTYPE_IF_LEFT_PARENTHESIS_EXPRESSION_RIGHT_PARENTHESIS_STATEMENT: {
            //IF '(' expression ')' statement
            selection_statement->statement_context->statement_sub_type = STATEMENT_SUB_TYPE_IF;

            std::shared_ptr<ast_node> expression = selection_statement->items[2];
            std::shared_ptr<ast_node> statement = selection_statement->items[4];

            statement->statement_context = std::make_shared<tsc_statement_context>();
            statement->statement_context->parent_statement_context = selection_statement->statement_context;

            semantics_analysis_result = analyze_expression(expression, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            semantics_analysis_result = analyze_statement(statement, context, function);
        }
        case NODE_TYPE_SELECTION_STATEMENT_SUBTYPE_SWITCH_LEFT_PARENTHESIS_EXPRESSION_RIGHT_PARENTHESIS_STATEMENT: {
            //SWITCH '(' expression ')' statement
            selection_statement->statement_context->statement_sub_type = STATEMENT_SUB_TYPE_SWITCH;
            selection_statement->statement_context->switch_statement_context = std::make_shared<tsc_switch_statement_context>();

            std::shared_ptr<ast_node> expression = selection_statement->items[2];
            std::shared_ptr<ast_node> statement = selection_statement->items[4];
            statement->statement_context = std::make_shared<tsc_statement_context>();
            statement->statement_context->parent_statement_context = selection_statement->statement_context;

            semantics_analysis_result = analyze_expression(expression, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;

            semantics_analysis_result = analyze_statement(statement, context, function);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            //todo check branches
        } break;
    }

    return semantics_analysis_result;
}

/*
iteration_statement
	: WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement expression_statement ')' statement
	| FOR '(' expression_statement expression_statement expression ')' statement
	| FOR '(' declaration expression_statement ')' statement
	| FOR '(' declaration expression_statement expression ')' statement
	;
*/
int analyze_iteration_statement(std::shared_ptr<ast_node> iteration_statement, semantics_analysis_context &context,
                                std::shared_ptr<tsc_function> function) {
    int semantics_analysis_result = 0;
    iteration_statement->statement_context = std::make_shared<tsc_statement_context>();
    iteration_statement->statement_context->statement_type = STATEMENT_TYPE_ITERATION_STATEMENT;
    switch (iteration_statement->node_sub_type) {
        case NODE_TYPE_ITERATION_STATEMENT_SUBTYPE_WHILE_LEFT_PARENTHESIS_EXPRESSION_RIGHT_PARENTHESIS_EXPRESSION: {
            //WHILE '(' expression ')' statement
            std::shared_ptr<ast_node> expression = iteration_statement->items[2];
            std::shared_ptr<ast_node> statement = iteration_statement->items[4];

            statement->statement_context = std::make_shared<tsc_statement_context>();
            statement->statement_context->parent_statement_context = iteration_statement->statement_context;

            semantics_analysis_result = analyze_expression(expression, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            semantics_analysis_result = analyze_statement(statement, context, function);
            if (semantics_analysis_result)
                return semantics_analysis_result;
        } break;
        case NODE_TYPE_ITERATION_STATEMENT_SUBTYPE_DO_STATEMENT_WHILE_LEFT_PARENTHESIS_EXPRESSION_RIGHT_PARENTHESIS_SEMI_COLON: {
            //DO statement WHILE '(' expression ')' ';'
            std::shared_ptr<ast_node> statement = iteration_statement->items[1];
            std::shared_ptr<ast_node> expression = iteration_statement->items[4];

            statement->statement_context = std::make_shared<tsc_statement_context>();
            statement->statement_context->parent_statement_context = iteration_statement->statement_context;

            semantics_analysis_result = analyze_statement(statement, context, function);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            semantics_analysis_result = analyze_expression(expression, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;
        } break;
            //for循环的括号可以声明变量,需要一个新的符号表节点
        case NODE_TYPE_ITERATION_STATEMENT_SUBTYPE_FOR_LEFT_PARENTHESIS_EXPRESSION_STATEMENT_EXPRESSION_STATEMENT_RIGHT_PARENTHESIS_STATEMENT:
        case NODE_TYPE_ITERATION_STATEMENT_SUBTYPE_FOR_LEFT_PARENTHESIS_EXPRESSION_STATEMENT_EXPRESSION_STATEMENT_EXPRESSION_RIGHT_PARENTHESIS_STATEMENT:
        case NODE_TYPE_ITERATION_STATEMENT_SUBTYPE_FOR_LEFT_PARENTHESIS_DECLARATION_EXPRESSION_STATEMENT_RIGHT_PARENTHESIS_STATEMENT:
        case NODE_TYPE_ITERATION_STATEMENT_SUBTYPE_FOR_LEFT_PARENTHESIS_DECLARATION_EXPRESSION_STATEMENT_EXPRESSION_RIGHT_PARENTHESIS_STATEMENT: {

            std::shared_ptr<symbol_table_node> for_iteration_statement_symbol_table_node =
                    std::make_shared<symbol_table_node>();

            std::shared_ptr<symbol_table_node> original_symbol_table_node = context.current_symbol_table_node;
            for_iteration_statement_symbol_table_node->parent = original_symbol_table_node;
            context.current_symbol_table_node = for_iteration_statement_symbol_table_node;
            switch (iteration_statement->node_sub_type) {
                case NODE_TYPE_ITERATION_STATEMENT_SUBTYPE_FOR_LEFT_PARENTHESIS_EXPRESSION_STATEMENT_EXPRESSION_STATEMENT_RIGHT_PARENTHESIS_STATEMENT: {
                    //FOR '(' expression_statement expression_statement ')' statement
                    std::shared_ptr<ast_node> expression_statement1 = iteration_statement->items[2];
                    std::shared_ptr<ast_node> expression_statement2 = iteration_statement->items[3];
                    std::shared_ptr<ast_node> statement = iteration_statement->items[5];

                    expression_statement1->statement_context = std::make_shared<tsc_statement_context>();
                    expression_statement1->statement_context->statement_type = STATEMENT_TYPE_EXPRESSION_STATEMENT;

                    expression_statement1->statement_context->parent_statement_context = iteration_statement->statement_context;

                    expression_statement2->statement_context = std::make_shared<tsc_statement_context>();
                    expression_statement2->statement_context->statement_type = STATEMENT_TYPE_EXPRESSION_STATEMENT;
                    expression_statement2->statement_context->parent_statement_context = iteration_statement->statement_context;

                    statement->statement_context = std::make_shared<tsc_statement_context>();
                    statement->statement_context->parent_statement_context = iteration_statement->statement_context;

                    semantics_analysis_result = analyze_expression_statement(expression_statement1, context, function);
                    if (semantics_analysis_result)
                        return semantics_analysis_result;
                    semantics_analysis_result = analyze_expression_statement(expression_statement2, context, function);
                    if (semantics_analysis_result)
                        return semantics_analysis_result;
                    semantics_analysis_result = analyze_statement(statement, context, function);
                    if (semantics_analysis_result)
                        return semantics_analysis_result;
                } break;
                case NODE_TYPE_ITERATION_STATEMENT_SUBTYPE_FOR_LEFT_PARENTHESIS_EXPRESSION_STATEMENT_EXPRESSION_STATEMENT_EXPRESSION_RIGHT_PARENTHESIS_STATEMENT: {
                    //FOR '(' expression_statement expression_statement expression ')' statement
                    std::shared_ptr<ast_node> expression_statement1 = iteration_statement->items[2];
                    std::shared_ptr<ast_node> expression_statement2 = iteration_statement->items[3];
                    std::shared_ptr<ast_node> expression = iteration_statement->items[4];
                    std::shared_ptr<ast_node> statement = iteration_statement->items[6];

                    expression_statement1->statement_context = std::make_shared<tsc_statement_context>();
                    expression_statement1->statement_context->statement_type = STATEMENT_TYPE_EXPRESSION_STATEMENT;
                    expression_statement1->statement_context->parent_statement_context = iteration_statement->statement_context;

                    expression_statement2->statement_context = std::make_shared<tsc_statement_context>();
                    expression_statement2->statement_context->statement_type = STATEMENT_TYPE_EXPRESSION_STATEMENT;
                    expression_statement2->statement_context->parent_statement_context = iteration_statement->statement_context;

                    statement->statement_context = std::make_shared<tsc_statement_context>();
                    statement->statement_context->parent_statement_context = iteration_statement->statement_context;

                    semantics_analysis_result = analyze_expression_statement(expression_statement1, context, function);
                    if (semantics_analysis_result)
                        return semantics_analysis_result;
                    semantics_analysis_result = analyze_expression_statement(expression_statement2, context, function);
                    if (semantics_analysis_result)
                        return semantics_analysis_result;
                    semantics_analysis_result = analyze_expression(expression, context);
                    if (semantics_analysis_result)
                        return semantics_analysis_result;
                    semantics_analysis_result = analyze_statement(statement, context, function);
                    if (semantics_analysis_result)
                        return semantics_analysis_result;

                } break;
                case NODE_TYPE_ITERATION_STATEMENT_SUBTYPE_FOR_LEFT_PARENTHESIS_DECLARATION_EXPRESSION_STATEMENT_RIGHT_PARENTHESIS_STATEMENT: {
                    //FOR '(' declaration expression_statement ')' statement
                    std::shared_ptr<ast_node> declaration = iteration_statement->items[2];
                    std::shared_ptr<ast_node> expression_statement = iteration_statement->items[3];
                    std::shared_ptr<ast_node> statement = iteration_statement->items[5];

                    expression_statement->statement_context = std::make_shared<tsc_statement_context>();
                    expression_statement->statement_context->statement_type = STATEMENT_TYPE_EXPRESSION_STATEMENT;
                    expression_statement->statement_context->parent_statement_context = iteration_statement->statement_context;

                    statement->statement_context = std::make_shared<tsc_statement_context>();
                    statement->statement_context->parent_statement_context = iteration_statement->statement_context;

                    std::shared_ptr<tsc_symbol> symbol = std::make_shared<tsc_symbol>();
                    symbol->type = std::make_shared<tsc_type>();

                    semantics_analysis_result = analyze_declaration(expression_statement, context, symbol);
                    if (semantics_analysis_result)
                        return semantics_analysis_result;

                    semantics_analysis_result = analyze_expression_statement(expression_statement, context, function);
                    if (semantics_analysis_result)
                        return semantics_analysis_result;
                    semantics_analysis_result = analyze_statement(statement, context, function);
                    if (semantics_analysis_result)
                        return semantics_analysis_result;
                } break;
                case NODE_TYPE_ITERATION_STATEMENT_SUBTYPE_FOR_LEFT_PARENTHESIS_DECLARATION_EXPRESSION_STATEMENT_EXPRESSION_RIGHT_PARENTHESIS_STATEMENT: {
                    //FOR '(' declaration expression_statement expression ')' statement
                    std::shared_ptr<ast_node> declaration = iteration_statement->items[2];
                    std::shared_ptr<ast_node> expression_statement = iteration_statement->items[3];
                    std::shared_ptr<ast_node> expression = iteration_statement->items[4];
                    std::shared_ptr<ast_node> statement = iteration_statement->items[6];

                    expression_statement->statement_context = std::make_shared<tsc_statement_context>();
                    expression_statement->statement_context->statement_type = STATEMENT_TYPE_EXPRESSION_STATEMENT;
                    expression_statement->statement_context->parent_statement_context = iteration_statement->statement_context;

                    statement->statement_context = std::make_shared<tsc_statement_context>();
                    statement->statement_context->parent_statement_context = iteration_statement->statement_context;

                    std::shared_ptr<tsc_symbol> symbol = std::make_shared<tsc_symbol>();
                    symbol->type = std::make_shared<tsc_type>();
                    semantics_analysis_result = analyze_declaration(declaration, context, symbol);
                    if (semantics_analysis_result)
                        return semantics_analysis_result;

                    semantics_analysis_result = analyze_expression_statement(expression_statement, context, function);
                    if (semantics_analysis_result)
                        return semantics_analysis_result;
                    semantics_analysis_result = analyze_expression(expression, context);
                    if (semantics_analysis_result)
                        return semantics_analysis_result;
                    semantics_analysis_result = analyze_statement(statement, context, function);
                    if (semantics_analysis_result)
                        return semantics_analysis_result;
                } break;
            }

            context.current_symbol_table_node = original_symbol_table_node;
        } break;
    }

    return semantics_analysis_result;
}

/*
jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';'
	| BREAK ';'
	| RETURN ';'
	| RETURN expression ';'
	;
*/
int analyze_jump_statement(std::shared_ptr<ast_node> jump_statement, semantics_analysis_context &context,
                           std::shared_ptr<tsc_function> function) {
    int semantics_analysis_result = 0;

    switch (jump_statement->node_sub_type) {
        case NODE_TYPE_JUMP_STATEMENT_SUBTYPE_GOTO_IDENTIFIER_SEMI_COLON: {
        } break;
        case NODE_TYPE_JUMP_STATEMENT_SUBTYPE_CONTINUE_SEMI_COLON: {
            //continue必须在循环语句中.这里可以是多层嵌套语句
            if (!find_parent_statement_context(jump_statement->statement_context, STATEMENT_TYPE_ITERATION_STATEMENT)) {
                printf("%s:%d error:\n\t'continue' statement not in loop or switch statement\n", input_file_name.c_str(),
                       jump_statement->get_first_terminal_line_no());
                return 1;
            }
        } break;
        case NODE_TYPE_JUMP_STATEMENT_SUBTYPE_BREAK_SEMI_COLON: {
            //break必须在循环语句或者switch语句中.如果找到了一个循环语句为父语句则break与它配对.否则沿着parent找直到找到selection_statement且为switch
            if (!find_parent_statement_context(jump_statement->statement_context, STATEMENT_TYPE_ITERATION_STATEMENT)) {

                std::shared_ptr<tsc_statement_context> current_parent_statement_context =
                        jump_statement->statement_context->parent_statement_context;
                while (current_parent_statement_context &&
                       (current_parent_statement_context->statement_type != STATEMENT_TYPE_SELECTION_STATEMENT ||
                        current_parent_statement_context->statement_sub_type != STATEMENT_SUB_TYPE_SWITCH)) {
                    current_parent_statement_context = current_parent_statement_context->parent_statement_context;
                }
                if (!current_parent_statement_context) {
                    printf("%s:%d error:\n\t'break' statement not in loop or switch statement\n", input_file_name.c_str(),
                           jump_statement->get_first_terminal_line_no());
                    return 1;
                }
            }

        } break;
        case NODE_TYPE_JUMP_STATEMENT_SUBTYPE_RETURN_SEMI_COLON: {
            //function return type should be void
            std::shared_ptr<tsc_type> return_type = function->function_symbol->type->function_signature->return_type;
            if (return_type != global_types::primitive_type_void) {
                printf("%s:%d error:\n\tnon-void function '%s' should return a value\n", input_file_name.c_str(),
                       jump_statement->get_first_terminal_line_no(), function->name.c_str());
                return 1;
            }

        } break;
        case NODE_TYPE_JUMP_STATEMENT_SUBTYPE_RETURN_EXPRESSION_SEMI_COLON: {
            std::shared_ptr<tsc_type> return_type = function->function_symbol->type->function_signature->return_type;
            if (return_type == global_types::primitive_type_void) {
                printf("%s:%d error:\n\tvoid function '%s' should not return a value\n", input_file_name.c_str(),
                       jump_statement->get_first_terminal_line_no(), function->name.c_str());
                return 1;
            }
            //todo check return type compatibility
        } break;
    }
    return semantics_analysis_result;
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
                                   semantics_analysis_context &context, std::shared_ptr<tsc_symbol> &symbol,
                                   int declaration_specifiers_location) {
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

    semantics_analysis_result = check_type_specifiers(type_specifiers, context, symbol, declaration_specifiers_location);
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
                          std::shared_ptr<tsc_symbol> &symbol, int declaration_specifiers_location) {
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
                printf("%s:%d error:\n\tunsupported C99 '_Imaginary' in declaration specifiers\n", input_file_name.c_str(),
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
            int semantics_analysis_result =
                    analyze_enum_specifier(enum_specifier, context, symbol, declaration_specifiers_location);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            break;
        }

            break;
        case PRIMITIVE_TYPE_INT:
            symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_const_int
                                                                  : global_types::primitive_type_int;
            break;

        case PRIMITIVE_TYPE_UNSIGNED_INT:
            symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_const_unsigned_int
                                                                  : global_types::primitive_type_unsigned_int;
            break;
        case PRIMITIVE_TYPE_LONG:
            symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_const_long
                                                                  : global_types::primitive_type_long;
            break;
        case PRIMITIVE_TYPE_UNSIGNED_LONG:
            symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_const_unsigned_long
                                                                  : global_types::primitive_type_unsigned_long;
            break;
        case PRIMITIVE_TYPE_LONG_LONG:
            symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_const_long_long
                                                                  : global_types::primitive_type_long_long;
            break;
        case PRIMITIVE_TYPE_UNSIGNED_LONG_LONG:
            symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_const_unsigned_long_long
                                                                  : global_types::primitive_type_unsigned_long_long;
            break;
        case PRIMITIVE_TYPE_FLOAT:
            symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_const_float
                                                                  : global_types::primitive_type_float;
            break;
        case PRIMITIVE_TYPE_DOUBLE:
            symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_const_double
                                                                  : global_types::primitive_type_double;
            break;
        case PRIMITIVE_TYPE_LONG_DOUBLE:
            symbol->type = symbol->type->const_type_qualifier_set ? global_types::primitive_type_const_long_double
                                                                  : global_types::primitive_type_long_double;
            break;
        case RECORD_TYPE_STRUCT_OR_UNION: {
            symbol->type->type_id = type_id;
            std::shared_ptr<ast_node> struct_or_union_specifier = type_specifiers[0]->items[0];
            int semantics_analysis_result =
                    analyze_struct_or_union_specifier(struct_or_union_specifier, context, symbol, declaration_specifiers_location);
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
                           std::shared_ptr<tsc_symbol> &symbol, int declaration_specifiers_location) {

    std::shared_ptr<ast_node> identifier_node; // enum A{...}; identifier=A
    std::shared_ptr<ast_node> enumerator_list;
    int semantics_analysis_result = 0;
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

    if (identifier_node) {
        semantics_analysis_result = add_type_name_to_symbol_table(symbol, context, identifier_node);
        if (semantics_analysis_result)
            return semantics_analysis_result;
    } else {
        symbol->type->internal_name = std::make_shared<std::string>("enum <anonymous>");
    }
    if (symbol->type->is_complete && declaration_specifiers_location == DECLARATION_SPECIFIERS_LOCATION_PARAMETER_LIST) {
        printf("%s:%d warning:\n\t'%s' declared inside parameter list will not be visible outside of this definition or "
               "declaration\n",
               input_file_name.c_str(), enum_specifier->get_first_terminal_line_no(), symbol->type->internal_name->c_str());
    }

    if (enumerator_list)
        return analyze_enumerator_list(enumerator_list, context, symbol);
    else
        return 0;
}

int add_type_name_to_symbol_table(std::shared_ptr<tsc_symbol> &symbol, semantics_analysis_context &context,
                                  std::shared_ptr<ast_node> identifier_node) {
    std::string identifier = *identifier_node->lexeme;
    std::string type_name_prefix;
    switch (symbol->type->type_id) {
        case PRIMITIVE_TYPE_ENUM:
            type_name_prefix = "enum ";
            break;
        case RECORD_TYPE_STRUCT_OR_UNION: {
            switch (symbol->type->sub_type_id) {
                case SUB_TYPE_STRUCT:
                    type_name_prefix = "struct ";
                    break;
                case SUB_TYPE_UNION:
                    type_name_prefix = "union ";
                    break;
            }
        }
    }
    symbol->type->internal_name = std::make_shared<std::string>(type_name_prefix + identifier);
    symbol->type->name = symbol->type->internal_name;
    // 检查符号表中是否已经有同名的符号
    for (std::map<std::string, std::shared_ptr<tsc_type>>::iterator it =
            context.current_symbol_table_node->struct_union_enum_names.begin();
         it != context.current_symbol_table_node->struct_union_enum_names.end(); it++) {
        //可以多次声明但是只能定义1次.检查之前是否已经有complete的declaration.这里实际上只是检查了是否声明了其它类型的同名tag
        // 如 struct A; union A;->error
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

    return 0;
}

int add_nested_anonymous_struct_fields_to_parent_struct(std::shared_ptr<tsc_symbol> &parent_struct_symbol,
                                                        std::shared_ptr<tsc_symbol> &child_struct_symbol,
                                                        semantics_analysis_context &context) {

    int semantics_analysis_result = 0;
    for (std::shared_ptr<tsc_symbol> child_field_symbol : child_struct_symbol->type->fields) {
        std::shared_ptr<tsc_type> type = child_field_symbol->type;
        if (type->type_id == RECORD_TYPE_STRUCT_OR_UNION && !type->name) {
            semantics_analysis_result =
                    add_nested_anonymous_struct_fields_to_parent_struct(parent_struct_symbol, child_field_symbol, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;
        } else {
            parent_struct_symbol->type->fields.push_back(child_field_symbol);
        }
    }

    return semantics_analysis_result;
}

/*
struct_or_union_specifier
: struct_or_union '{' struct_declaration_list '}'
| struct_or_union IDENTIFIER '{' struct_declaration_list '}'
| struct_or_union IDENTIFIER
;
*/

int analyze_struct_or_union_specifier(std::shared_ptr<ast_node> struct_or_union_specifier,
                                      semantics_analysis_context &context, std::shared_ptr<tsc_symbol> &symbol,
                                      int declaration_specifiers_location) {
    // todo calculate size of struct
    std::shared_ptr<ast_node> struct_or_union = struct_or_union_specifier->items[0];
    std::shared_ptr<ast_node> identifier_node;
    std::shared_ptr<ast_node> struct_declaration_list;
    int semantics_analysis_result = 0;
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
        semantics_analysis_result = add_type_name_to_symbol_table(symbol, context, identifier_node);
        if (semantics_analysis_result)
            return semantics_analysis_result;
    } else {
        std::string type_name_prefix;
        switch (symbol->type->sub_type_id) {
            case SUB_TYPE_STRUCT:
                type_name_prefix = "struct ";
                break;
            case SUB_TYPE_UNION:
                type_name_prefix = "union ";
                break;
        }
        symbol->type->internal_name = std::make_shared<std::string>(type_name_prefix + "<anonymous>");
    }

    if (symbol->type->is_complete && declaration_specifiers_location == DECLARATION_SPECIFIERS_LOCATION_PARAMETER_LIST) {
        printf("%s:%d warning:\n\t'%s' declared inside parameter list will not be visible outside of this definition or "
               "declaration\n",
               input_file_name.c_str(), struct_or_union_specifier->get_first_terminal_line_no(),
               symbol->type->internal_name->c_str());
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
    // 如果这个 specifier_qualifier_list 是一个匿名的 struct_union 且没有declarator 则它的成员要加入外层struct的成员中.
    // struct A{struct{int x;}}; -> struct A has a member 'x'
    // struct A{struct{int x;}y;}; -> struct A has a member 'y' but does not have a member 'x'
    int semantics_analysis_result;
    //todo calculate offset and size of struct
    switch (struct_declaration->node_sub_type) {
        case NODE_TYPE_STRUCT_DECLARATION_SUBTYPE_SPECIFIER_QUALIFIER_LIST_SEMI_COLON: {
            std::shared_ptr<ast_node> specifier_qualifier_list = struct_declaration->items[0];

            semantics_analysis_result = analyze_specifier_qualifier_list(specifier_qualifier_list, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            std::shared_ptr<tsc_type> type = specifier_qualifier_list->symbol->type;
            if (type->type_id == RECORD_TYPE_STRUCT_OR_UNION && !type->name) {
                semantics_analysis_result =
                        add_nested_anonymous_struct_fields_to_parent_struct(symbol, specifier_qualifier_list->symbol, context);
                if (semantics_analysis_result)
                    return semantics_analysis_result;
            } else {
                printf("%s:%d warning:\n\t declaration '%s' does not declare anything\n", input_file_name.c_str(),
                       struct_declaration->get_first_terminal_line_no(), struct_declaration->get_expression().c_str());
            }

        } break;
        case NODE_TYPE_STRUCT_DECLARATION_SUBTYPE_SPECIFIER_QUALIFIER_LIST_STRUT_DECLARATOR_LIST_SEMI_COLON: {
            std::shared_ptr<ast_node> specifier_qualifier_list = struct_declaration->items[0];
            std::shared_ptr<ast_node> struct_declarator_list = struct_declaration->items[1];

            semantics_analysis_result = analyze_specifier_qualifier_list(specifier_qualifier_list, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            std::shared_ptr<tsc_symbol> field_symbol = specifier_qualifier_list->symbol;
            // non-anonymous fields registered in analyze_struct_declarator_list

            semantics_analysis_result = analyze_struct_declarator_list(struct_declarator_list, context, symbol, field_symbol);
            if (semantics_analysis_result)
                return semantics_analysis_result;

        } break;
        case NODE_TYPE_STRUCT_DECLARATION_SUBTYPE_STATIC_ASSERT_DECLARATION:
            printf("%s:%d error:\n\tstatic_assert_declaration not supported\n", input_file_name.c_str(),
                   struct_declaration->get_first_terminal_line_no());
            return 1;
    }
    //所有fields添加完毕,校验是否有重名的字段
    std::set<std::string> field_identifiers;
    for (std::shared_ptr<tsc_symbol> field_symbol : symbol->type->fields) {
        if (!field_symbol->identifier)
            continue;
        if (field_identifiers.find(*field_symbol->identifier) != field_identifiers.end()) {
            printf("%s:%d error:\n\tduplicate member '%s'\n", input_file_name.c_str(),
                   struct_declaration->get_first_terminal_line_no(), field_symbol->identifier->c_str());
            return 1;
        } else {
            field_identifiers.insert(*field_symbol->identifier);
        }
    }

    return 0;
}

/*
struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;
*/

// 参数symbol中的type表示当前struct_declarator_list所在的struct的type
int analyze_struct_declarator_list(std::shared_ptr<ast_node> struct_declarator_list,
                                   semantics_analysis_context &context, std::shared_ptr<tsc_symbol> &symbol,
                                   std::shared_ptr<tsc_symbol> &field_symbol) {
    std::vector<std::shared_ptr<ast_node>> struct_declarators;
    std::shared_ptr<ast_node> node = struct_declarator_list;
    int semantics_analysis_result;
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
    //每个 struct_declarator 应该是一个独立的symbol 如int a,b;则a,b分别是一个symbol.所以这里copy一份
    for (std::shared_ptr<ast_node> struct_declarator : struct_declarators) {
        std::shared_ptr<tsc_symbol> field = std::make_shared<tsc_symbol>();
        struct_declarator->symbol = std::make_shared<tsc_symbol>(*field_symbol);
        std::shared_ptr<ast_node> out_identifier_node;
        semantics_analysis_result = analyze_struct_declarator(struct_declarator, context, symbol, out_identifier_node);

        if (semantics_analysis_result)
            return semantics_analysis_result;
        symbol->type->fields.push_back(struct_declarator->symbol);
    }

    return 0;
}

/*
struct_declarator
	: ':' constant_expression
	| declarator ':' constant_expression
	| declarator
	;
*/

int analyze_struct_declarator(std::shared_ptr<ast_node> struct_declarator, semantics_analysis_context &context,
                              std::shared_ptr<tsc_symbol> &symbol, std::shared_ptr<ast_node> &out_identifier_node) {
    // struct_declarator->symbol的type字段已填好

    std::shared_ptr<tsc_symbol> struct_declarator_symbol = struct_declarator->symbol;
    int semantics_analysis_result;
    std::shared_ptr<ast_node> constant_expression;
    std::shared_ptr<ast_node> declarator;
    switch (struct_declarator->node_sub_type) {
        case NODE_TYPE_STRUCT_DECLARATOR_SUBTYPE_COLON_CONSTANT_EXPRESSION: {
            struct_declarator_symbol->is_anonymous = true;
            struct_declarator_symbol->is_bit_field = true;
            constant_expression = struct_declarator->items[1];

        } break;
        case NODE_TYPE_STRUCT_DECLARATOR_SUBTYPE_DECLARATOR_COLON_CONSTANT_EXPRESSION: {
            struct_declarator_symbol->is_anonymous = false;
            struct_declarator_symbol->is_bit_field = true;
            declarator = struct_declarator->items[0];
            constant_expression = struct_declarator->items[2];
        } break;
        case NODE_TYPE_STRUCT_DECLARATOR_SUBTYPE_DECLARATOR: {
            struct_declarator_symbol->is_anonymous = false;
            struct_declarator_symbol->is_bit_field = false;
            declarator = struct_declarator->items[0];

        } break;
    }

    if (constant_expression) {
        semantics_analysis_result = analyze_constant_expression(constant_expression, context);
        if (semantics_analysis_result)
            return semantics_analysis_result;
        if (!is_integer_constant(constant_expression)) {
            printf("%s:%d error:\n\tconstant_expression '%s' should be integer\n", input_file_name.c_str(),
                   constant_expression->get_first_terminal_line_no(), constant_expression->get_expression().c_str());
            return 1;
        }
        //todo check type length int a:64;->error int *a:12;->error(invalid type)
        struct_declarator_symbol->bit_field_length = constant_expression->symbol->value->int_value;
    }
    if (declarator) {
        declarator->symbol = struct_declarator_symbol;
        semantics_analysis_result = analyze_declarator(declarator, context, out_identifier_node);
        if (semantics_analysis_result)
            return semantics_analysis_result;
        struct_declarator_symbol->identifier = out_identifier_node->lexeme;
    }

    if (!check_complete_type(out_identifier_node->symbol->type)) {
        printf("%s:%d error:\n\tstorage size of '%s' isn’t known\n", input_file_name.c_str(),
               struct_declarator->get_first_terminal_line_no(), out_identifier_node->symbol->identifier->c_str());
        return 1;
    }

    return 0;
}

/*
declarator
	: pointer direct_declarator
	| direct_declarator
	;
*/

int analyze_declarator(std::shared_ptr<ast_node> declarator, semantics_analysis_context &context,
                       std::shared_ptr<ast_node> &out_identifier_node) {

    int semantics_analysis_result = 0;

    switch (declarator->node_sub_type) {
        case NODE_TYPE_DECLARATOR_SUBTYPE_POINTER_DIRECT_DECLARATOR: {
            std::shared_ptr<ast_node> pointer = declarator->items[0];
            std::shared_ptr<ast_node> direct_declarator = declarator->items[1];
            direct_declarator->symbol = std::make_shared<tsc_symbol>();
            // 举例:对于int ** const * p;的解析.首先得到 declarator symbol, expression为 ** const * direct_declarator 类型为int.
            // 其中 direct_declarator 的expression为 "p" 然后先构造direct_declarator的符号再解析pointer部分.注意pointer是右递归的
            // 得到一个 derived_declarator_symbol * const * p. 类型为pointer,underlying_type为int derived_declarator_symbol的类型(当前还未知)
            // declarator_symbol 的 operator_id 为 UNARY_OPERATOR_MUL operand 为 derived_declarator_symbol p
            // 所以构造symbol的顺序与解析type的顺序是逐层的. symbol 是 *p 则  p的type是pointer(or const pointer) to (type of *p)
            semantics_analysis_result = analyze_pointer(pointer, context, declarator->symbol, direct_declarator->symbol);
            if (semantics_analysis_result)
                return semantics_analysis_result;

            semantics_analysis_result = analyze_direct_declarator(direct_declarator, context, out_identifier_node);
            if (semantics_analysis_result)
                return semantics_analysis_result;

        } break;
        case NODE_TYPE_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR: {
            std::shared_ptr<ast_node> direct_declarator = declarator->items[0];
            direct_declarator->symbol = declarator->symbol;
            semantics_analysis_result = analyze_direct_declarator(direct_declarator, context, out_identifier_node);
            if (semantics_analysis_result)
                return semantics_analysis_result;

        } break;
    }
    return semantics_analysis_result;
}

/*
direct_declarator
	: IDENTIFIER
	| '(' declarator ')'
	| direct_declarator '[' ']'
	| direct_declarator '[' '*' ']'
	| direct_declarator '[' STATIC type_qualifier_list assignment_expression ']'
	| direct_declarator '[' STATIC assignment_expression ']'
	| direct_declarator '[' type_qualifier_list '*' ']'
	| direct_declarator '[' type_qualifier_list STATIC assignment_expression ']'
	| direct_declarator '[' type_qualifier_list assignment_expression ']'
	| direct_declarator '[' type_qualifier_list ']'
	| direct_declarator '[' assignment_expression ']'
	| direct_declarator '(' parameter_type_list ')'
	| direct_declarator '(' ')'
	| direct_declarator '(' identifier_list ')'
	;
*/

/*
identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER
	;
*/
int analyze_direct_declarator(std::shared_ptr<ast_node> direct_declarator, semantics_analysis_context &context,
                              std::shared_ptr<ast_node> &out_identifier_node) {
    //调用的时候 direct_declarator->symbol 不为 nullptr
    int semantics_analysis_result = 0;

    // 这里来解释一下 '(' declarator ')' 为了结合性.考虑 int* p[5]和int (*p)[5]的区别.
    // 第一个p是一个数组,数组元素为int* 第二个p是指向int[5]的指针
    switch (direct_declarator->node_sub_type) {
        case NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_IDENTIFIER: {
            // IDENTIFIER
            out_identifier_node = direct_declarator->items[0];
            direct_declarator->symbol->identifier = out_identifier_node->lexeme;
            out_identifier_node->symbol = direct_declarator->symbol;
        } break;
        case NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_LEFT_PARENTHESIS_DECLARATOR_RIGHT_PARENTHESIS: {
            // '(' declarator ')'
            std::shared_ptr<ast_node> declarator = direct_declarator->items[0];
            semantics_analysis_result = analyze_declarator(declarator, context, out_identifier_node);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            direct_declarator->symbol = declarator->symbol;
        } break;
        case NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_BRACKET_RIGHT_BRACKET: {
            //direct_declarator '[' ']'
            std::shared_ptr<ast_node> next_direct_declarator = direct_declarator->items[0];
            next_direct_declarator->symbol = std::make_shared<tsc_symbol>();
            next_direct_declarator->symbol->type = construct_array_of(direct_declarator->symbol->type);
            // int a[]={1,2,3} ->is_complete=true 但是现在先置为false
            next_direct_declarator->symbol->type->is_complete = false;
            next_direct_declarator->symbol->is_left_value = false;
            semantics_analysis_result = analyze_direct_declarator(next_direct_declarator, context, out_identifier_node);

        } break;
        case NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_BRACKET_MUL_RIGHT_BRACKET:
        case NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_BRACKET_STATIC_TYPE_QUALIFIER_LIST_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET:
        case NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_BRACKET_STATIC_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET:
        case NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_BRACKET_TYPE_QUALIFIER_LIST_MUL_RIGHT_BRACKET:
        case NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_BRACKET_TYPE_QUALIFIER_LIST_STATIC_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET:
        case NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_BRACKET_TYPE_QUALIFIER_LIST_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET:
            //gcc clang均不支持的语法
            printf("%s:%d error:\n\tunsupported grammar in direct_declarator '%s'\n", input_file_name.c_str(),
                   direct_declarator->get_first_terminal_line_no(), direct_declarator->get_expression().c_str());
            return 1;

        case NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_BRACKET_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET: {
            // direct_declarator '[' assignment_expression ']'
            //不支持VLA

            std::shared_ptr<ast_node> next_direct_declarator = direct_declarator->items[0];
            std::shared_ptr<ast_node> assignment_expression = direct_declarator->items[2];

            next_direct_declarator->symbol = std::make_shared<tsc_symbol>();
            next_direct_declarator->symbol->type = construct_array_of(direct_declarator->symbol->type);
            semantics_analysis_result = analyze_assignment_expression(assignment_expression, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            if (!is_integer_constant(assignment_expression)) {
                printf("%s:%d error:\n\tvla unsupported in direct_declarator '%s', expecting constant expression\n",
                       input_file_name.c_str(), direct_declarator->get_first_terminal_line_no(),
                       direct_declarator->get_expression().c_str());
                return 1;
            }
            next_direct_declarator->symbol->type->array_length =
                    std::make_shared<int>(assignment_expression->symbol->value->int_value);
            next_direct_declarator->symbol->is_left_value = false;
            semantics_analysis_result = analyze_direct_declarator(next_direct_declarator, context, out_identifier_node);

        } break;
        case NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_PARENTHESIS_PARAMETER_TYPE_LIST_RIGHT_PARENTHESIS: {
            // direct_declarator '(' parameter_type_list ')'
            std::shared_ptr<ast_node> next_direct_declarator = direct_declarator->items[0];
            std::shared_ptr<ast_node> parameter_type_list = direct_declarator->items[2];

            next_direct_declarator->symbol = std::make_shared<tsc_symbol>();
            next_direct_declarator->symbol->symbol_type = SYMBOL_TYPE_FUNCTION;
            next_direct_declarator->symbol->type = std::make_shared<tsc_type>();
            next_direct_declarator->symbol->type->type_id = TYPE_FUNCTION;
            next_direct_declarator->symbol->type->internal_name = std::make_shared<std::string>("function");
            next_direct_declarator->symbol->type->function_signature = std::make_shared<tsc_function_signature>();
            next_direct_declarator->symbol->type->function_signature->return_type = direct_declarator->symbol->type;
            next_direct_declarator->symbol->type->function_signature->has_proto = true;
            semantics_analysis_result =
                    analyze_parameter_type_list(parameter_type_list, context, next_direct_declarator->symbol);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            semantics_analysis_result = analyze_direct_declarator(next_direct_declarator, context, out_identifier_node);

            next_direct_declarator->symbol->is_left_value = false;
        } break;
        case NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_PARENTHESIS_RIGHT_PARENTHESIS: {
            //direct_declarator '(' ')'
            std::shared_ptr<ast_node> next_direct_declarator = direct_declarator->items[0];

            next_direct_declarator->symbol = std::make_shared<tsc_symbol>();
            next_direct_declarator->symbol->symbol_type = SYMBOL_TYPE_FUNCTION;
            next_direct_declarator->symbol->type = std::make_shared<tsc_type>();
            next_direct_declarator->symbol->type->type_id = TYPE_FUNCTION;
            next_direct_declarator->symbol->type->internal_name = std::make_shared<std::string>("function");
            next_direct_declarator->symbol->type->function_signature = std::make_shared<tsc_function_signature>();
            next_direct_declarator->symbol->type->function_signature->return_type = direct_declarator->symbol->type;
            next_direct_declarator->symbol->type->function_signature->has_proto = false;
            if (semantics_analysis_result)
                return semantics_analysis_result;
            //locate for future use
            next_direct_declarator->symbol->type->function_signature->parameters_symbol_table_node =
                    std::make_shared<symbol_table_node>();
            next_direct_declarator->symbol->type->function_signature->parameters_symbol_table_node->parent =
                    context.current_symbol_table_node;

            semantics_analysis_result = analyze_direct_declarator(next_direct_declarator, context, out_identifier_node);
            next_direct_declarator->symbol->is_left_value = false;
        } break;
        case NODE_TYPE_DIRECT_DECLARATOR_SUBTYPE_DIRECT_DECLARATOR_LEFT_PARENTHESIS_IDENTIFIER_LIST_RIGHT_PARENTHESIS: {
            // direct_declarator '(' identifier_list ')'
            std::shared_ptr<ast_node> next_direct_declarator = direct_declarator->items[0];
            std::shared_ptr<ast_node> identifier_list = direct_declarator->items[2];

            next_direct_declarator->symbol = std::make_shared<tsc_symbol>();
            next_direct_declarator->symbol->symbol_type = SYMBOL_TYPE_FUNCTION;
            next_direct_declarator->symbol->type = std::make_shared<tsc_type>();
            next_direct_declarator->symbol->type->type_id = TYPE_FUNCTION;
            next_direct_declarator->symbol->type->function_signature = std::make_shared<tsc_function_signature>();
            next_direct_declarator->symbol->type->function_signature->return_type = direct_declarator->symbol->type;
            next_direct_declarator->symbol->type->function_signature->has_proto = false;
            if (semantics_analysis_result)
                return semantics_analysis_result;

            std::shared_ptr<ast_node> node = identifier_list;
            std::vector<std::shared_ptr<ast_node>> identifier_nodes;

            while (node->node_type == NODE_TYPE_IDENTIFIER_LIST &&
                   node->node_sub_type == NODE_TYPE_IDENTIFIER_LIST_SUBTYPE_IDENTIFIER_LIST_COMMA_IDENTIFIER) {
                identifier_nodes.push_back(node->items[2]);
                node = node->items[0];
            }

            identifier_nodes.push_back(node->items[0]);
            identifier_nodes = std::vector<std::shared_ptr<ast_node>>(identifier_nodes.rbegin(), identifier_nodes.rend());
            identifier_list->sub_nodes = identifier_nodes;
            //identifier 不能重复
            std::set<std::string> parameter_identifiers;
            for (std::shared_ptr<ast_node> &identifier_node : identifier_nodes) {
                std::string parameter_identifier = *identifier_node->lexeme;
                if (parameter_identifiers.find(parameter_identifier) != parameter_identifiers.end()) {
                    printf("%s:%d error:\n\tredefinition of parameter '%s'in direct_declarator '%s'\n", input_file_name.c_str(),
                           direct_declarator->get_first_terminal_line_no(), parameter_identifier.c_str(),
                           direct_declarator->get_expression().c_str());
                    return 1;
                }
                parameter_identifiers.insert(parameter_identifier);

                next_direct_declarator->symbol->type->function_signature->identifiers.push_back(parameter_identifier);
            }

            next_direct_declarator->symbol->type->function_signature->parameters_symbol_table_node =
                    std::make_shared<symbol_table_node>();
            next_direct_declarator->symbol->type->function_signature->parameters_symbol_table_node->parent =
                    context.current_symbol_table_node;

            semantics_analysis_result = analyze_direct_declarator(next_direct_declarator, context, out_identifier_node);
            next_direct_declarator->symbol->is_left_value = false;
        } break;
    }

    return 0;
}

/*
pointer
	: '*' type_qualifier_list pointer
	| '*' type_qualifier_list
	| '*' pointer
	| '*'
	;
*/

int analyze_pointer(std::shared_ptr<ast_node> pointer, semantics_analysis_context &context,
                    std::shared_ptr<tsc_symbol> &derived_declarator_symbol,
                    std::shared_ptr<tsc_symbol> &direct_declarator_symbol) {
    int semantics_analysis_result = 0;
    switch (pointer->node_sub_type) {
        case NODE_TYPE_POINTER_SUBTYPE_MUL_TYPE_QUALIFIER_LIST_POINTER: {
            std::shared_ptr<ast_node> type_qualifier_list = pointer->items[1];
            std::shared_ptr<ast_node> next_pointer = pointer->items[2];
            std::shared_ptr<tsc_symbol> next_derived_declarator_symbol = std::make_shared<tsc_symbol>();
            next_derived_declarator_symbol->type = construct_pointer_to(derived_declarator_symbol->type);
            derived_declarator_symbol->operator_id = UNARY_OPERATOR_MUL;
            derived_declarator_symbol->operands.push_back(next_derived_declarator_symbol);
            semantics_analysis_result =
                    analyze_type_qualifier_list(type_qualifier_list, context, next_derived_declarator_symbol);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            semantics_analysis_result =
                    analyze_pointer(next_pointer, context, next_derived_declarator_symbol, direct_declarator_symbol);

        } break;
        case NODE_TYPE_POINTER_SUBTYPE_MUL_TYPE_QUALIFIER_LIST: {
            std::shared_ptr<ast_node> type_qualifier_list = pointer->items[1];
            derived_declarator_symbol->operator_id = UNARY_OPERATOR_MUL;
            derived_declarator_symbol->operands.push_back(direct_declarator_symbol);
            direct_declarator_symbol->type = construct_pointer_to(derived_declarator_symbol->type);
            semantics_analysis_result = analyze_type_qualifier_list(type_qualifier_list, context, direct_declarator_symbol);
        } break;
        case NODE_TYPE_POINTER_SUBTYPE_MUL_POINTER: {
            std::shared_ptr<ast_node> next_pointer = pointer->items[1];
            std::shared_ptr<tsc_symbol> next_derived_declarator_symbol = std::make_shared<tsc_symbol>();
            next_derived_declarator_symbol->type = construct_pointer_to(derived_declarator_symbol->type);
            derived_declarator_symbol->operator_id = UNARY_OPERATOR_MUL;
            derived_declarator_symbol->operands.push_back(next_derived_declarator_symbol);
            semantics_analysis_result =
                    analyze_pointer(next_pointer, context, next_derived_declarator_symbol, direct_declarator_symbol);

        } break;
        case NODE_TYPE_POINTER_SUBTYPE_MUL: {
            derived_declarator_symbol->operator_id = UNARY_OPERATOR_MUL;
            derived_declarator_symbol->operands.push_back(direct_declarator_symbol);
            direct_declarator_symbol->type = construct_pointer_to(derived_declarator_symbol->type);

        } break;
    }
    return semantics_analysis_result;
}

/*
type_qualifier_list
	: type_qualifier
	| type_qualifier_list type_qualifier
	;
*/
int analyze_type_qualifier_list(std::shared_ptr<ast_node> type_qualifier_list, semantics_analysis_context &context,
                                std::shared_ptr<tsc_symbol> &symbol) {

    int semantics_analysis_result = 0;
    std::vector<std::shared_ptr<ast_node>> type_qualifiers;
    std::shared_ptr<ast_node> node = type_qualifier_list;
    while (node->node_type == NODE_TYPE_TYPE_QUALIFIER_LIST &&
           node->node_sub_type == NODE_TYPE_TYPE_QUALIFIER_LIST_SUBTYPE_TYPE_QUALIFIER) {
        type_qualifiers.push_back(node->items[1]);
        node = node->items[0];
    }

    type_qualifiers.push_back(node->items[0]);
    type_qualifiers = std::vector<std::shared_ptr<ast_node>>(type_qualifiers.rbegin(), type_qualifiers.rend());
    type_qualifier_list->sub_nodes = type_qualifiers;
    semantics_analysis_result = check_type_qualifiers(type_qualifiers, context, symbol);
    return semantics_analysis_result;
}

/*
parameter_type_list
	: parameter_list ',' ELLIPSIS
	| parameter_list
	;

parameter_list
	: parameter_declaration
	| parameter_list ',' parameter_declaration
	;
*/

int analyze_parameter_type_list(std::shared_ptr<ast_node> parameter_type_list, semantics_analysis_context &context,
                                std::shared_ptr<tsc_symbol> &function_symbol) {
    //if one argument is void then void must be the only argument
    int semantics_analysis_result = 0;
    std::shared_ptr<ast_node> parameter_list = parameter_type_list->items[0];
    switch (parameter_type_list->node_sub_type) {
        case NODE_TYPE_PARAMETER_TYPE_LIST_SUBTYPE_PARAMETER_LIST_COMMA_ELLIPSE:
            function_symbol->type->function_signature->has_ellipse = true;
            break;
        case NODE_TYPE_PARAMETER_TYPE_LIST_SUBTYPE_PARAMETER_LIST:
            function_symbol->type->function_signature->has_ellipse = false;
            break;
    }
    // 如果在参数列表中声明complete的struct_union则在外部不可见.
    // warning: 'struct A' declared inside parameter list will not be visible outside of this definition or declaration
    // 所以参数列表需要一个单独的符号表节点
    std::shared_ptr<symbol_table_node> parameters_symbol_table_node = std::make_shared<symbol_table_node>();
    parameters_symbol_table_node->parent = context.current_symbol_table_node;
    context.current_symbol_table_node = parameters_symbol_table_node;
    function_symbol->type->function_signature->parameters_symbol_table_node = parameters_symbol_table_node;
    std::vector<std::shared_ptr<ast_node>> parameter_declarations;
    std::shared_ptr<ast_node> node = parameter_list;
    while (node->node_type == NODE_TYPE_PARAMETER_LIST &&
           node->node_sub_type == NODE_TYPE_PARAMETER_LIST_SUBTYPE_PARAMETER_LIST_COMMA_PARAMETER_DECLARATION) {
        parameter_declarations.push_back(node->items[2]);
        node = node->items[0];
    }

    parameter_declarations.push_back(node->items[0]);
    parameter_declarations =
            std::vector<std::shared_ptr<ast_node>>(parameter_declarations.rbegin(), parameter_declarations.rend());
    parameter_list->sub_nodes = parameter_declarations;
    std::set<std::string> parameter_identifiers;

    bool parameter_is_void = false;
    for (std::shared_ptr<ast_node> parameter_declaration : parameter_declarations) {
        std::shared_ptr<tsc_symbol> parameter_symbol = std::make_shared<tsc_symbol>();

        semantics_analysis_result =
                analyze_parameter_declaration(parameter_declaration, context, function_symbol, parameter_symbol);
        if (semantics_analysis_result)
            return semantics_analysis_result;
        //校验是否有重名参数
        if (parameter_symbol->identifier) {
            if (parameter_identifiers.find(*parameter_symbol->identifier) != parameter_identifiers.end()) {
                printf("%s:%d error:\n\tredefinition of parameter '%s'\n", input_file_name.c_str(),
                       parameter_declaration->get_first_terminal_line_no(), parameter_symbol->identifier->c_str());
                return 1;
            } else {
                parameter_identifiers.insert(*parameter_symbol->identifier);
                //参数可以是匿名的,函数内部访问不到
                function_symbol->type->function_signature->parameters_symbol_table_node
                        ->identifier_and_symbols[*parameter_symbol->identifier] = parameter_symbol;
            }
        }
        if (parameter_symbol->type == global_types::primitive_type_void ||
            parameter_symbol->type == global_types::primitive_type_const_void) {
            parameter_is_void = true;
        }
        function_symbol->type->function_signature->parameter_is_void = parameter_is_void;
        function_symbol->type->function_signature->parameter_symbols.push_back(parameter_symbol);
    }

    if (parameter_is_void && function_symbol->type->function_signature->parameter_symbols.size() > 1) {
        printf("%s:%d error:\n\tvoid must be the only parameter\n", input_file_name.c_str(),
               parameter_type_list->get_first_terminal_line_no());
        return 1;
    }
    //恢复符号表节点
    context.current_symbol_table_node = context.current_symbol_table_node->parent;
    return 0;
}

/*
parameter_declaration
	: declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;
*/
int analyze_parameter_declaration(std::shared_ptr<ast_node> parameter_declaration, semantics_analysis_context &context,
                                  std::shared_ptr<tsc_symbol> &function_symbol,
                                  std::shared_ptr<tsc_symbol> &out_parameter_symbol) {
    std::shared_ptr<ast_node> declaration_specifiers = parameter_declaration->items[0];
    std::shared_ptr<tsc_symbol> symbol = std::make_shared<tsc_symbol>();
    symbol->type = std::make_shared<tsc_type>();
    int semantics_analysis_result = 0;

    semantics_analysis_result = analyze_declaration_specifiers(declaration_specifiers, context, symbol,
                                                               DECLARATION_SPECIFIERS_LOCATION_PARAMETER_LIST);
    if (semantics_analysis_result)
        return semantics_analysis_result;
    switch (parameter_declaration->node_sub_type) {
        case NODE_TYPE_PARAMETER_DECLARATION_SUBTYPE_DECLARATION_SPECIFIERS_DECLARATOR: {
            std::shared_ptr<ast_node> declarator = parameter_declaration->items[1];
            declarator->symbol = symbol;
            std::shared_ptr<ast_node> out_identifier_node;
            semantics_analysis_result = analyze_declarator(declarator, context, out_identifier_node);
            out_parameter_symbol = out_identifier_node->symbol;
            out_parameter_symbol->identifier = out_identifier_node->lexeme;

        } break;
        case NODE_TYPE_PARAMETER_DECLARATION_SUBTYPE_DECLARATION_SPECIFIERS_ABSTRACT_DECLARATOR: {
            std::shared_ptr<ast_node> abstract_declarator = parameter_declaration->items[1];
            abstract_declarator->symbol = symbol;
            semantics_analysis_result = analyze_abstract_declarator(abstract_declarator, context, out_parameter_symbol);
        } break;
        case NODE_TYPE_PARAMETER_DECLARATION_SUBTYPE_DECLARATION_SPECIFIERS: {
            out_parameter_symbol = symbol;

        } break;
    }

    return semantics_analysis_result;
}

/*
abstract_declarator
	: pointer direct_abstract_declarator
	| pointer
	| direct_abstract_declarator
	;
*/
int analyze_abstract_declarator(std::shared_ptr<ast_node> abstract_declarator, semantics_analysis_context &context,
                                std::shared_ptr<tsc_symbol> &out_anonymous_symbol) {
    int semantics_analysis_result = 0;

    switch (abstract_declarator->node_sub_type) {
        case NODE_TYPE_ABSTRACT_DECLARATOR_SUBTYPE_POINTER_DIRECT_ABSTRACT_DECLARATOR: {
            std::shared_ptr<ast_node> pointer = abstract_declarator->items[0];
            std::shared_ptr<ast_node> direct_abstract_declarator = abstract_declarator->items[1];
            direct_abstract_declarator->symbol = std::make_shared<tsc_symbol>();
            semantics_analysis_result =
                    analyze_pointer(pointer, context, abstract_declarator->symbol, direct_abstract_declarator->symbol);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            semantics_analysis_result =
                    analyze_direct_abstract_declarator(direct_abstract_declarator, context, out_anonymous_symbol);

        } break;
        case NODE_TYPE_ABSTRACT_DECLARATOR_SUBTYPE_POINTER: {
            std::shared_ptr<ast_node> pointer = abstract_declarator->items[0];
            out_anonymous_symbol = std::make_shared<tsc_symbol>();
            semantics_analysis_result = analyze_pointer(pointer, context, abstract_declarator->symbol, out_anonymous_symbol);

        } break;
        case NODE_TYPE_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR: {
            std::shared_ptr<ast_node> direct_abstract_declarator = abstract_declarator->items[0];
            direct_abstract_declarator->symbol = abstract_declarator->symbol;

            semantics_analysis_result =
                    analyze_direct_abstract_declarator(direct_abstract_declarator, context, out_anonymous_symbol);

        } break;
    }
    return semantics_analysis_result;
}

/*
direct_abstract_declarator
	: '(' abstract_declarator ')'
	| '[' ']'
	| '[' '*' ']'
	| '[' STATIC type_qualifier_list assignment_expression ']'
	| '[' STATIC assignment_expression ']'
	| '[' type_qualifier_list STATIC assignment_expression ']'
	| '[' type_qualifier_list assignment_expression ']'
	| '[' type_qualifier_list ']'
	| '[' assignment_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' '*' ']'
	| direct_abstract_declarator '[' STATIC type_qualifier_list assignment_expression ']'
	| direct_abstract_declarator '[' STATIC assignment_expression ']'
	| direct_abstract_declarator '[' type_qualifier_list assignment_expression ']'
	| direct_abstract_declarator '[' type_qualifier_list STATIC assignment_expression ']'
	| direct_abstract_declarator '[' type_qualifier_list ']'
	| direct_abstract_declarator '[' assignment_expression ']'
	| '(' ')'
	| '(' parameter_type_list ')'
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_type_list ')'
	;
*/
int analyze_direct_abstract_declarator(std::shared_ptr<ast_node> direct_abstract_declarator,
                                       semantics_analysis_context &context,
                                       std::shared_ptr<tsc_symbol> &out_anonymous_symbol) {
    int semantics_analysis_result = 0;

    switch (direct_abstract_declarator->node_sub_type) {
        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_PARENTHESIS_ABSTRACT_DECLARATOR_RIGHT_PARENTHESIS: {
            // '(' abstract_declarator ')'
            std::shared_ptr<ast_node> abstract_declarator = direct_abstract_declarator->items[0];
            abstract_declarator->symbol = direct_abstract_declarator->symbol;
            std::shared_ptr<ast_node> out_identifier_node;
            semantics_analysis_result = analyze_declarator(abstract_declarator, context, out_identifier_node);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            out_anonymous_symbol = out_identifier_node->symbol;

        } break;

        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_BRACKET_RIGHT_BRACKET: {
            // '[' ']'
            out_anonymous_symbol = std::make_shared<tsc_symbol>();
            out_anonymous_symbol->type = construct_array_of(direct_abstract_declarator->symbol->type);
            out_anonymous_symbol->type->is_complete = false;

        } break;

        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_BRACKET_MUL_RIGHT_BRACKET:
        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_BRACKET_STATIC_TYPE_QUALIFIER_LIST_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET:
        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_BRACKET_STATIC_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET:
        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_BRACKET_TYPE_QUALIFIER_LIST_STATIC_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET:
        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_BRACKET_TYPE_QUALIFIER_LIST_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET:
        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_BRACKET_TYPE_QUALIFIER_LIST_RIGHT_BRACKET: {
            printf("%s:%d error:\n\tunsupported grammar in direct_abstract_declarator '%s'\n", input_file_name.c_str(),
                   direct_abstract_declarator->get_first_terminal_line_no(),
                   direct_abstract_declarator->get_expression().c_str());
            return 1;
        } break;

        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_BRACKET_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET: {
            // '[' assignment_expression ']'
            out_anonymous_symbol = std::make_shared<tsc_symbol>();
            std::shared_ptr<ast_node> assignment_expression = direct_abstract_declarator->items[1];

            out_anonymous_symbol->type = construct_array_of(direct_abstract_declarator->symbol->type);
            semantics_analysis_result = analyze_assignment_expression(assignment_expression, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            if (!is_integer_constant(assignment_expression)) {
                printf("%s:%d error:\n\tvla unsupported in direct_abstract_declarator '%s', expecting constant expression\n",
                       input_file_name.c_str(), direct_abstract_declarator->get_first_terminal_line_no(),
                       direct_abstract_declarator->get_expression().c_str());
                return 1;
            }
            out_anonymous_symbol->type->array_length = std::make_shared<int>(assignment_expression->symbol->value->int_value);
            semantics_analysis_result =
                    analyze_direct_abstract_declarator(direct_abstract_declarator, context, out_anonymous_symbol);

        } break;

        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_BRACKET_RIGHT_BRACKET: {
            //direct_abstract_declarator '[' ']'
            std::shared_ptr<ast_node> next_direct_abstract_declarator = direct_abstract_declarator->items[0];
            next_direct_abstract_declarator->symbol = std::make_shared<tsc_symbol>();
            next_direct_abstract_declarator->symbol->type = construct_array_of(direct_abstract_declarator->symbol->type);
            next_direct_abstract_declarator->symbol->type->is_complete = false;
            semantics_analysis_result =
                    analyze_direct_abstract_declarator(next_direct_abstract_declarator, context, out_anonymous_symbol);

        } break;

        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_BRACKET_MUL_RIGHT_BRACKET:
        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_BRACKET_STATIC_TYPE_QUALIFIER_LIST_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET:
        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_BRACKET_STATIC_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET:
        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_BRACKET_TYPE_QUALIFIER_LIST_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET:
        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_BRACKET_TYPE_QUALIFIER_LIST_STATIC_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET:
        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_BRACKET_TYPE_QUALIFIER_LIST_RIGHT_BRACKET: {
            printf("%s:%d error:\n\tunsupported grammar in direct_abstract_declarator '%s'\n", input_file_name.c_str(),
                   direct_abstract_declarator->get_first_terminal_line_no(),
                   direct_abstract_declarator->get_expression().c_str());
            return 1;
        } break;

        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_BRACKET_ASSIGNMENT_EXPRESSION_RIGHT_BRACKET: {
            //direct_abstract_declarator '[' assignment_expression ']'
            std::shared_ptr<ast_node> next_direct_abstract_declarator = direct_abstract_declarator->items[0];
            std::shared_ptr<ast_node> assignment_expression = direct_abstract_declarator->items[2];

            next_direct_abstract_declarator->symbol = std::make_shared<tsc_symbol>();
            next_direct_abstract_declarator->symbol->type = construct_array_of(direct_abstract_declarator->symbol->type);
            semantics_analysis_result = analyze_assignment_expression(assignment_expression, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            if (!is_integer_constant(assignment_expression)) {
                printf("%s:%d error:\n\tvla unsupported in direct_abstract_declarator '%s', expecting constant expression\n",
                       input_file_name.c_str(), direct_abstract_declarator->get_first_terminal_line_no(),
                       direct_abstract_declarator->get_expression().c_str());
                return 1;
            }
            next_direct_abstract_declarator->symbol->type->array_length =
                    std::make_shared<int>(assignment_expression->symbol->value->int_value);
            semantics_analysis_result =
                    analyze_direct_abstract_declarator(next_direct_abstract_declarator, context, out_anonymous_symbol);

        } break;

        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_PARENTHESIS_RIGHT_PARENTHESIS: {
            //'(' ')'
            out_anonymous_symbol = std::make_shared<tsc_symbol>();
            out_anonymous_symbol->symbol_type = SYMBOL_TYPE_FUNCTION;
            out_anonymous_symbol->type = std::make_shared<tsc_type>();
            out_anonymous_symbol->type->internal_name = std::make_shared<std::string>("function");
            out_anonymous_symbol->type->function_signature->return_type = direct_abstract_declarator->symbol->type;
            out_anonymous_symbol->type->function_signature->has_proto = false;
        } break;

        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_LEFT_PARENTHESIS_PARAMETER_TYPE_LIST_RIGHT_PARENTHESIS: {
            //'(' parameter_type_list ')'
            std::shared_ptr<ast_node> parameter_type_list = direct_abstract_declarator->items[1];

            out_anonymous_symbol = std::make_shared<tsc_symbol>();
            out_anonymous_symbol->symbol_type = SYMBOL_TYPE_FUNCTION;
            out_anonymous_symbol->type = std::make_shared<tsc_type>();
            out_anonymous_symbol->type->internal_name = std::make_shared<std::string>("function");
            out_anonymous_symbol->type->function_signature->return_type = direct_abstract_declarator->symbol->type;
            out_anonymous_symbol->type->function_signature->has_proto = true;
            semantics_analysis_result = analyze_parameter_type_list(parameter_type_list, context, out_anonymous_symbol);
        } break;

        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_PARENTHESIS_RIGHT_PARENTHESIS: {
            // direct_abstract_declarator '(' ')'
            std::shared_ptr<ast_node> next_direct_abstract_declarator = direct_abstract_declarator->items[0];
            next_direct_abstract_declarator->symbol = std::make_shared<tsc_symbol>();
            next_direct_abstract_declarator->symbol->symbol_type = SYMBOL_TYPE_FUNCTION;
            next_direct_abstract_declarator->symbol->type = std::make_shared<tsc_type>();
            next_direct_abstract_declarator->symbol->type->internal_name = std::make_shared<std::string>("function");
            next_direct_abstract_declarator->symbol->type->function_signature->return_type =
                    direct_abstract_declarator->symbol->type;
            next_direct_abstract_declarator->symbol->type->function_signature->has_proto = false;

            semantics_analysis_result =
                    analyze_direct_abstract_declarator(next_direct_abstract_declarator, context, out_anonymous_symbol);

        } break;

        case NODE_TYPE_DIRECT_ABSTRACT_DECLARATOR_SUBTYPE_DIRECT_ABSTRACT_DECLARATOR_LEFT_PARENTHESIS_PARAMETER_TYPE_LIST_RIGHT_PARENTHESIS: {
            // direct_abstract_declarator '(' parameter_type_list ')'

            std::shared_ptr<ast_node> next_direct_abstract_declarator = direct_abstract_declarator->items[0];
            std::shared_ptr<ast_node> parameter_type_list = direct_abstract_declarator->items[2];

            next_direct_abstract_declarator->symbol = std::make_shared<tsc_symbol>();
            next_direct_abstract_declarator->symbol->symbol_type = SYMBOL_TYPE_FUNCTION;
            next_direct_abstract_declarator->symbol->type = std::make_shared<tsc_type>();
            next_direct_abstract_declarator->symbol->type->internal_name = std::make_shared<std::string>("function");
            next_direct_abstract_declarator->symbol->type->function_signature->return_type =
                    direct_abstract_declarator->symbol->type;
            next_direct_abstract_declarator->symbol->type->function_signature->has_proto = true;

            semantics_analysis_result =
                    analyze_parameter_type_list(parameter_type_list, context, next_direct_abstract_declarator->symbol);

        } break;
    }
    return semantics_analysis_result;
}

bool check_type_compatibility(std::shared_ptr<tsc_type> type1, std::shared_ptr<tsc_type> type2) {
    if (type1->type_id != type2->type_id)
        return false;
    //至少1个是声明
    if (!type1->is_complete || !type2->is_complete)
        return true;
    return false;
}

bool check_same_type(std::shared_ptr<tsc_type> type1, std::shared_ptr<tsc_type> type2, bool consider_const) {
    if (type1->type_id != type2->type_id)
        return false;
    if (consider_const && type1->const_type_qualifier_set != type2->const_type_qualifier_set)
        return false;
    //全局变量不应有register所以这里不检查

    switch (type1->type_id) {
        case RECORD_TYPE_STRUCT_OR_UNION:
            return type1 == type2;
        case SCALAR_TYPE_POINTER:
        case SCALAR_TYPE_ARRAY:
            // 全局变量的声明中第一次可以是incomplete array(长度不定)
            return check_same_type(type1->underlying_type, type2->underlying_type, true);
        case TYPE_FUNCTION:
            //todo check function
            break;
    }
    return true;
}

bool check_complete_type(std::shared_ptr<tsc_type> type) {
    return !(type == global_types::primitive_type_void || type == global_types::primitive_type_const_void ||
             !type->is_complete);
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
                semantics_analysis_result =
                        check_common_type(expression, next_conditional_expression, conditional_expression->symbol->type);
                if (semantics_analysis_result)
                    return semantics_analysis_result;

                conditional_expression->symbol->operator_id = OPERATOR_QUESTION_COLON;
                conditional_expression->symbol->operands.push_back(logical_or_expression->symbol);
                conditional_expression->symbol->operands.push_back(expression->symbol);
                conditional_expression->symbol->operands.push_back(next_conditional_expression->symbol);
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
            if (!additive_expression->items[0]->symbol->type || !additive_expression->items[2]->symbol->type) {
                printf("%s:%d error:\n\tshould not reach here\n", __FILE__, __LINE__);
                return 1;
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

            semantics_analysis_result = analyze_type_name(type_name, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;

            std::shared_ptr<ast_node> next_cast_expression = cast_expression->items[3];
            semantics_analysis_result = analyze_cast_expression(next_cast_expression, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            cast_expression->symbol = std::make_shared<tsc_symbol>();

            // struct_union 不能强转为primitive type. 反之primitive type也不能转为 struct_union
            // 不能直接将double转为指针类型(gcc)

            if (!check_can_assign(type_name->symbol, next_cast_expression->symbol)) {
                printf("%s:%d error:\n\t operand of type '%s' cannot be cast to '%s'\n", input_file_name.c_str(),
                       cast_expression->get_first_terminal_line_no(), next_cast_expression->symbol->type->internal_name->c_str(),
                       type_name->symbol->type->internal_name->c_str());
                return 1;
            }

            cast_expression->symbol->symbol_type = SYMBOL_TYPE_TEMPORARY_VARIABLE;
            if (is_constant(next_cast_expression)) {
                if (PRIMITIVE_TYPE_CHAR <= type_name->symbol->type->type_id &&
                    type_name->symbol->type->type_id <= PRIMITIVE_TYPE_UNSIGNED_LONG_LONG)
                    cast_expression->symbol->symbol_type = SYMBOL_TYPE_ICONSTANT;
                else if (PRIMITIVE_TYPE_FLOAT <= type_name->symbol->type->type_id &&
                         type_name->symbol->type->type_id <= PRIMITIVE_TYPE_LONG_DOUBLE)
                    cast_expression->symbol->symbol_type = SYMBOL_TYPE_FCONSTANT;
                else if (type_name->symbol->type->type_id == SCALAR_TYPE_POINTER)
                    cast_expression->symbol->symbol_type = SYMBOL_TYPE_POINTER_CONSTANT;
            }

            cast_expression->symbol->type = type_name->symbol->type;
            cast_expression->symbol->operator_id = OPERATOR_CAST;
            cast_expression->symbol->operands.push_back(type_name->symbol);
            cast_expression->symbol->operands.push_back(next_cast_expression->symbol);
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
            // ++和--要求操作数为左值.可以对double ++和--. 注意前置++或者--后不再是left value

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
            if (next_unary_expression->symbol->type->const_type_qualifier_set) {
                printf("%s:%d error:\n\tcannot assign to variable '%s' with const-qualified type '%s'\n", input_file_name.c_str(),
                       unary_expression->get_first_terminal_line_no(), next_unary_expression->get_expression().c_str(),
                       next_unary_expression->symbol->type->internal_name->c_str());
                semantics_analysis_result = 1;
                return semantics_analysis_result;
            }

            int operator_id = -1;
            switch (unary_expression->node_sub_type) {
                case NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_INC_OP_UNARY_EXPRESSION:
                    operator_id = OPERATOR_PRE_INC;
                    break;
                case NODE_TYPE_UNARY_EXPRESSION_SUBTYPE_DEC_OP_UNARY_EXPRESSION:
                    operator_id = OPERATOR_PRE_DEC;
                    break;
            }
            unary_expression->symbol = std::make_shared<tsc_symbol>();
            unary_expression->symbol->symbol_type = SYMBOL_TYPE_TEMPORARY_VARIABLE;
            unary_expression->symbol->type = next_unary_expression->symbol->type;
            unary_expression->symbol->operator_id = operator_id;
            unary_expression->symbol->operands.push_back(next_unary_expression->symbol);
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
                    if (!is_array_or_pointer(cast_expression)) {
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
                    unary_expression->symbol->is_left_value = true;
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

            if (!check_complete_type(next_unary_expression->symbol->type)) {
                printf("%s:%d error:\n\tinvalid application of 'sizeof' to an incomplete type '%s' in unary_expression\n",
                       input_file_name.c_str(), unary_expression->get_first_terminal_line_no(),
                       next_unary_expression->symbol->type->internal_name->c_str());
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
            // sizeof 中可以声明类型,会加入符号表 sizeof (struct A{}); struct A a; ->OK
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
                std::string type_name_identifier = *type_out->internal_name;
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

            abstract_declarator->symbol = specifier_qualifier_list->symbol;
            std::shared_ptr<tsc_symbol> out_parameter_symbol;
            semantics_analysis_result = analyze_abstract_declarator(abstract_declarator, context, out_parameter_symbol);
            type_name->symbol = out_parameter_symbol;
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

    switch (postfix_expression->node_sub_type) {
        case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_PRIMARY_EXPRESSION: {
            std::shared_ptr<ast_node> primary_expression = postfix_expression->items[0];
            semantics_analysis_result = analyze_primary_expression(primary_expression, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            postfix_expression->symbol = primary_expression->symbol;
        }

            break;

        case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_LEFT_BRACKET_EXPRESSION_RIGHT_BRACKET: {
            // postfix_expression '[' expression ']'
            std::shared_ptr<ast_node> next_postfix_expression = postfix_expression->items[0];
            std::shared_ptr<ast_node> expression = postfix_expression->items[2];

            semantics_analysis_result = analyze_postfix_expression(next_postfix_expression, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            semantics_analysis_result = analyze_expression(expression, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;

            postfix_expression->symbol = std::make_shared<tsc_symbol>();
            postfix_expression->symbol->symbol_type = SYMBOL_TYPE_VARIABLE;
            postfix_expression->symbol->operator_id = OPERATOR_ARRAY_SUBSCRIPT;
            postfix_expression->symbol->operands.push_back(next_postfix_expression->symbol);
            postfix_expression->symbol->operands.push_back(expression->symbol);

            // vector in C is pointer to a vector of elements. like 'int*'
            // 支持类似于 3[a]的写法
            if (is_array_or_pointer(next_postfix_expression) && is_integer(expression)) {
                postfix_expression->symbol->type = next_postfix_expression->symbol->type->underlying_type;

            }

            else if (is_integer(next_postfix_expression) && is_array_or_pointer(expression)) {
                postfix_expression->symbol->type = expression->symbol->type->underlying_type;

            } else {
                printf("%s:%d error:\n\tarray subscript is not an integer\n", input_file_name.c_str(),
                       next_postfix_expression->get_first_terminal_line_no());
                return 1;
            }

        } break;
        case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_LEFT_PARENTHESIS_RIGHT_PARENTHESIS: {
            //postfix_expression '(' ')'
            std::shared_ptr<ast_node> next_postfix_expression = postfix_expression->items[0];
            postfix_expression->symbol = std::make_shared<tsc_symbol>();
            postfix_expression->symbol->symbol_type = SYMBOL_TYPE_FUNCTION;
            postfix_expression->symbol->operator_id = OPERATOR_FUNCTION_CALL;

            std::shared_ptr<tsc_type> out_function_type;
            semantics_analysis_result =
                    check_function_or_pointer_to_function(next_postfix_expression, context, out_function_type);
            //todo check arguments
            if (semantics_analysis_result)
                return semantics_analysis_result;
            postfix_expression->symbol->operands.push_back(next_postfix_expression->symbol);
            postfix_expression->symbol->type = out_function_type->function_signature->return_type;

        } break;
        case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_LEFT_PARENTHESIS_ARGUMENT_EXPRESSION_LIST_RIGHT_PARENTHESIS: {
            // postfix_expression '(' argument_expression_list ')'
            std::shared_ptr<ast_node> next_postfix_expression = postfix_expression->items[0];
            std::shared_ptr<ast_node> argument_expression_list = postfix_expression->items[2];
            postfix_expression->symbol = std::make_shared<tsc_symbol>();
            postfix_expression->symbol->symbol_type = SYMBOL_TYPE_FUNCTION;
            postfix_expression->symbol->operator_id = OPERATOR_FUNCTION_CALL;

            std::shared_ptr<tsc_type> out_function_type;
            semantics_analysis_result =
                    check_function_or_pointer_to_function(next_postfix_expression, context, out_function_type);
            if (semantics_analysis_result)
                return semantics_analysis_result;

            postfix_expression->symbol->type = out_function_type->function_signature->return_type;

            semantics_analysis_result = analyze_argument_expression_list(argument_expression_list, context, out_function_type);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            postfix_expression->symbol->type = out_function_type->function_signature->return_type;

            postfix_expression->symbol->operands.push_back(next_postfix_expression->symbol);
            //todo push other operands
        } break;
        case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_DOT_IDENTIFIER: {
            // postfix_expression '.' IDENTIFIER
            std::shared_ptr<ast_node> next_postfix_expression = postfix_expression->items[0];
            semantics_analysis_result = analyze_postfix_expression(next_postfix_expression, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;

            std::shared_ptr<tsc_type> type = next_postfix_expression->symbol->type;

            if (type->type_id != RECORD_TYPE_STRUCT_OR_UNION) {
                printf("%s:%d error:\n\tmember reference base type '%s' is not a structure or union\n", input_file_name.c_str(),
                       next_postfix_expression->get_first_terminal_line_no(),
                       next_postfix_expression->symbol->type->internal_name->c_str());
                semantics_analysis_result = 1;
                return semantics_analysis_result;
            }
            std::string identifier = *postfix_expression->items[2]->lexeme;
            std::shared_ptr<tsc_symbol> symbol;
            for (std::shared_ptr<tsc_symbol> field_symbol : type->fields) {
                if (field_symbol->identifier && *field_symbol->identifier == identifier) {
                    symbol = field_symbol;
                    break;
                }
            }
            if (!symbol) {
                printf("%s:%d error:\n\tno member named '%s' in '%s'\n", input_file_name.c_str(),
                       next_postfix_expression->get_first_terminal_line_no(), identifier.c_str(), type->internal_name->c_str());

                return 1;
            }
            postfix_expression->symbol = std::make_shared<tsc_symbol>();
            postfix_expression->symbol->symbol_type = SYMBOL_TYPE_TEMPORARY_VARIABLE;
            postfix_expression->symbol->type = symbol->type;
            postfix_expression->symbol->operator_id = OPERATOR_MEMBER_ACCESS;
            postfix_expression->symbol->operands.push_back(next_postfix_expression->symbol);
            postfix_expression->symbol->operands.push_back(symbol);

        } break;
        case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_PTR_OP_IDENTIFIER: {
            // postfix_expression PTR_OP IDENTIFIER
            std::shared_ptr<ast_node> next_postfix_expression = postfix_expression->items[0];
            std::string identifier = *postfix_expression->items[2]->lexeme;
            semantics_analysis_result = analyze_postfix_expression(next_postfix_expression, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;

            if (next_postfix_expression->symbol->type->type_id != SCALAR_TYPE_POINTER) {
                printf("%s:%d error:\n\tmember reference base type '%s' is not a pointer\n", input_file_name.c_str(),
                       next_postfix_expression->get_first_terminal_line_no(),
                       next_postfix_expression->symbol->type->internal_name->c_str());
                semantics_analysis_result = 1;
                return semantics_analysis_result;
            }

            std::shared_ptr<tsc_type> type = next_postfix_expression->symbol->type->underlying_type;

            if (type->type_id != RECORD_TYPE_STRUCT_OR_UNION) {
                printf("%s:%d error:\n\tmember reference base type '%s' is not a structure or union\n", input_file_name.c_str(),
                       next_postfix_expression->get_first_terminal_line_no(), type->internal_name->c_str());
                semantics_analysis_result = 1;
                return semantics_analysis_result;
            }
            std::shared_ptr<tsc_symbol> symbol;
            for (std::shared_ptr<tsc_symbol> field_symbol : type->fields) {
                if (field_symbol->identifier && *field_symbol->identifier == identifier) {
                    symbol = field_symbol;
                    break;
                }
            }
            if (!symbol) {
                printf("%s:%d error:\n\tno member named '%s' in '%s'\n", input_file_name.c_str(),
                       next_postfix_expression->get_first_terminal_line_no(), identifier.c_str(), type->internal_name->c_str());

                return 1;
            }
            postfix_expression->symbol = std::make_shared<tsc_symbol>();
            postfix_expression->symbol->symbol_type = SYMBOL_TYPE_TEMPORARY_VARIABLE;
            postfix_expression->symbol->type = symbol->type;
            postfix_expression->symbol->operator_id = OPERATOR_POINTER_MEMBER_ACCESS;
            postfix_expression->symbol->operands.push_back(next_postfix_expression->symbol);
            postfix_expression->symbol->operands.push_back(symbol);

        } break;
        case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_INC_OP:
        case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_DEC_OP: {
            // postfix_expression INC_OP
            // postfix_expression DEC_OP
            std::shared_ptr<ast_node> next_postfix_expression = postfix_expression->items[0];
            semantics_analysis_result = analyze_postfix_expression(next_postfix_expression, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;

            if (!next_postfix_expression->symbol->is_left_value) {
                printf("%s:%d error:\n\tlvalue expected in postfix_expression\n", input_file_name.c_str(),
                       next_postfix_expression->get_first_terminal_line_no());
                semantics_analysis_result = 1;
                return semantics_analysis_result;
            }
            if (next_postfix_expression->symbol->type->const_type_qualifier_set) {
                printf("%s:%d error:\n\tcannot assign to variable '%s' with const-qualified type '%s'\n", input_file_name.c_str(),
                       next_postfix_expression->get_first_terminal_line_no(), next_postfix_expression->get_expression().c_str(),
                       next_postfix_expression->symbol->type->internal_name->c_str());
                semantics_analysis_result = 1;
                return semantics_analysis_result;
            }

            int operator_id = -1;
            switch (postfix_expression->node_sub_type) {
                case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_INC_OP:
                    operator_id = OPERATOR_POST_INC;
                    break;
                case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_POSTFIX_EXPRESSION_DEC_OP:
                    operator_id = OPERATOR_POST_DEC;
                    break;
            }
            postfix_expression->symbol = std::make_shared<tsc_symbol>();
            postfix_expression->symbol->symbol_type = SYMBOL_TYPE_TEMPORARY_VARIABLE;
            postfix_expression->symbol->type = next_postfix_expression->symbol->type;
            postfix_expression->symbol->operator_id = operator_id;
            postfix_expression->symbol->operands.push_back(next_postfix_expression->symbol);
            postfix_expression->symbol->type = next_postfix_expression->symbol->type;

        } break;

        case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS_LEFT_BRACE_INITIALIZER_LIST_RIGHT_BRACE:
        case NODE_TYPE_POSTFIX_EXPRESSION_SUBTYPE_LEFT_PARENTHESIS_TYPE_NAME_RIGHT_PARENTHESIS_LEFT_BRACE_INITIALIZER_LIST_COMMA_RIGHT_BRACE: {

            // '(' type_name ')' '{' initializer_list '}'
            // '(' type_name ')' '{' initializer_list ',' '}'
            std::shared_ptr<ast_node> type_name = postfix_expression->items[1];
            std::shared_ptr<ast_node> initializer_list = postfix_expression->items[4];
            semantics_analysis_result = analyze_type_name(type_name, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            // type must be complete
            if (!type_name->symbol->type->is_complete) {
                printf("%s:%d error:\n\t '%s' has incomplete type '%s'\n", input_file_name.c_str(),
                       postfix_expression->get_first_terminal_line_no(), postfix_expression->get_expression().c_str(),
                       type_name->symbol->type->internal_name->c_str());
                semantics_analysis_result = 1;
                return semantics_analysis_result;
            }
            postfix_expression->symbol->type = type_name->symbol->type;

            semantics_analysis_result = analyze_initializer_list(initializer_list, context, type_name->symbol);

        }

            break;
    }

    return semantics_analysis_result;
}

int check_function_or_pointer_to_function(std::shared_ptr<ast_node> postfix_expression,
                                          semantics_analysis_context &context,
                                          std::shared_ptr<tsc_type> &out_function_type) {
    int semantics_analysis_result = 0;
    bool function_found = false;
    semantics_analysis_result = analyze_postfix_expression(postfix_expression, context);
    if (semantics_analysis_result)
        return semantics_analysis_result;

    if (postfix_expression->symbol->type->type_id == TYPE_FUNCTION) {
        out_function_type = postfix_expression->symbol->type;
        function_found = true;
    }
        // is it pointer to function?
    else if (postfix_expression->symbol->type->type_id == SCALAR_TYPE_POINTER &&
             postfix_expression->symbol->type->underlying_type->type_id == TYPE_FUNCTION) {
        function_found = true;
        out_function_type = postfix_expression->symbol->type->underlying_type;
    }

    if (!function_found) {
        printf("%s:%d error:\n\t'%s' is not function or pointer to function\n", input_file_name.c_str(),
               postfix_expression->get_first_terminal_line_no(), postfix_expression->get_expression().c_str());
        return 1;
    }
    return 0;
}

/*
argument_expression_list
: assignment_expression
| argument_expression_list ',' assignment_expression
;
 */

int analyze_argument_expression_list(std::shared_ptr<ast_node> argument_expression_list,
                                     semantics_analysis_context &context, std::shared_ptr<tsc_type> function_type) {

    int semantics_analysis_result = 0;

    std::vector<std::shared_ptr<ast_node>> assignment_expressions;
    std::shared_ptr<ast_node> node = argument_expression_list;
    while (node->node_type == NODE_TYPE_ARGUMENT_EXPRESSION_LIST &&
           node->node_sub_type ==
           NODE_TYPE_ARGUMENT_EXPRESSION_LIST_SUBTYPE_ARGUMENT_EXPRESSION_LIST_COMMA_ASSIGNMENT_EXPRESSION) {
        assignment_expressions.push_back(node->items[2]);
        node = node->items[0];
    }

    assignment_expressions.push_back(node->items[0]);
    assignment_expressions =
            std::vector<std::shared_ptr<ast_node>>(assignment_expressions.rbegin(), assignment_expressions.rend());
    argument_expression_list->sub_nodes = assignment_expressions;
    for (std::shared_ptr<ast_node> assignment_expression : assignment_expressions) {
        semantics_analysis_result = analyze_assignment_expression(assignment_expression, context);
        if (semantics_analysis_result)
            return semantics_analysis_result;
        //todo check with function symbol.注意可变参数
    }
    return 0;
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

    switch (primary_expression->node_sub_type) {
        case NODE_TYPE_PRIMARY_EXPRESSION_SUBTYPE_IDENTIFIER: {
            // 这里的identifier可能是函数名,变量名或者类型名字.
            // 这里在找不到的时候只是将primary_expression的symbol置为nullptr
            std::string symbol_identifier = *primary_expression->items[0]->lexeme;
            std::shared_ptr<tsc_symbol> symbol = lookup_symbol(context.current_symbol_table_node, symbol_identifier, true);

            if (!symbol) {
                printf("%s:%d error:\n\tidentifier '%s' not found in primary_expression\n", input_file_name.c_str(),
                       primary_expression->get_first_terminal_line_no(), primary_expression->get_expression().c_str());
                return 1;
            }
            primary_expression->symbol = symbol;
            if (symbol && !is_constant(primary_expression) && !primary_expression->symbol->memory_location)
                primary_expression->symbol->memory_location = std::make_shared<tsc_memory_location>();

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
                    std::make_shared<int>(string_node->symbol->value->string_value->length() + 1);

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

assignment_operator
	: '='
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
	| AND_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
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
            std::shared_ptr<ast_node> assignment_operator = assignment_expression->items[1];
            std::shared_ptr<ast_node> next_assignment_expression = assignment_expression->items[2];

            semantics_analysis_result = analyze_unary_expression(unary_expression, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;

            semantics_analysis_result = analyze_assignment_expression(next_assignment_expression, context);
            if (semantics_analysis_result)
                return semantics_analysis_result;

            if (!unary_expression->symbol->is_left_value) {
                printf("%s:%d error:\n\tlvalue expected in assignment_expression\n", input_file_name.c_str(),
                       unary_expression->get_first_terminal_line_no());
                semantics_analysis_result = 1;
                return semantics_analysis_result;
            }
            if (!unary_expression->symbol->type) {
                printf("no type symbol %s\n\n", unary_expression->get_expression().c_str());
            }
            if (unary_expression->symbol->type->const_type_qualifier_set) {

                printf("%s:%d error:\n\tcannot assign to variable '%s' with const-qualified type '%s'\n", input_file_name.c_str(),
                       unary_expression->get_first_terminal_line_no(), unary_expression->get_expression().c_str(),
                       unary_expression->symbol->type->internal_name->c_str());
                semantics_analysis_result = 1;
                return semantics_analysis_result;
            }

            int assignment_operator_id = -1;
            int binary_operator = -1;
            switch (assignment_operator->node_sub_type) {

                case NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_ASSIGN:
                    assignment_operator_id = OPERATOR_ASSIGN;
                    break;
                case NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_MUL_ASSIGN:
                    assignment_operator_id = OPERATOR_MUL_ASSIGN;
                    binary_operator = BINARY_OPERATOR_MUL;
                    break;
                case NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_DIV_ASSIGN:
                    assignment_operator_id = OPERATOR_DIV_ASSIGN;
                    binary_operator = BINARY_OPERATOR_DIV;
                    break;
                case NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_MOD_ASSIGN:
                    assignment_operator_id = OPERATOR_MOD_ASSIGN;
                    binary_operator = BINARY_OPERATOR_MOD;
                    break;
                case NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_ADD_ASSIGN:
                    assignment_operator_id = OPERATOR_ADD_ASSIGN;
                    binary_operator = BINARY_OPERATOR_ADD;
                    break;
                case NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_SUB_ASSIGN:
                    assignment_operator_id = OPERATOR_SUB_ASSIGN;
                    binary_operator = BINARY_OPERATOR_SUB;
                    break;
                case NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_LEFT_SHIFT_ASSIGN:
                    assignment_operator_id = OPERATOR_LEFT_SHIFT_ASSIGN;
                    binary_operator = BINARY_OPERATOR_LEFT_SHIFT;
                    break;
                case NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_RIGHT_SHIFT_ASSIGN:
                    assignment_operator_id = OPERATOR_RIGHT_SHIFT_ASSIGN;
                    binary_operator = BINARY_OPERATOR_RIGHT_SHIFT;
                    break;
                case NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_BITAND_ASSIGN:
                    assignment_operator_id = OPERATOR_BITAND_ASSIGN;
                    binary_operator = BINARY_OPERATOR_BITAND;
                    break;
                case NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_BITXOR_ASSIGN:
                    assignment_operator_id = OPERATOR_BITXOR_ASSIGN;
                    binary_operator = BINARY_OPERATOR_BITXOR;
                    break;
                case NODE_TYPE_ASSIGNMENT_OPERATOR_SUBTYPE_BITOR_ASSIGN:
                    assignment_operator_id = OPERATOR_BITOR_ASSIGN;
                    binary_operator = BINARY_OPERATOR_BITOR;
                    break;
                    break;
            }
            if (binary_operator != -1) {
                // check if binary_operator is valid
                std::shared_ptr<ast_node> virtual_parent = std::make_shared<ast_node>();
                virtual_parent->items.push_back(assignment_expression);
                construct_binary_expression_symbol(virtual_parent, binary_operator, unary_expression, next_assignment_expression);
            }
            assignment_expression->symbol = std::make_shared<tsc_symbol>();
            assignment_expression->symbol->symbol_type = SYMBOL_TYPE_TEMPORARY_VARIABLE;
            assignment_expression->symbol->operator_id = assignment_operator_id;
            assignment_expression->symbol->operands.push_back(unary_expression->symbol);
            assignment_expression->symbol->operands.push_back(next_assignment_expression->symbol);
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
            //todo support __func__
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
    int semantics_analysis_result = 0;
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
    return is_integer_constant(node) || is_floating_constant(node) ||
           node->symbol->symbol_type == SYMBOL_TYPE_POINTER_CONSTANT;
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

bool is_scalar_type(const std::shared_ptr<tsc_type> &type) {
    return type->type_id == SCALAR_TYPE_POINTER ||
           (PRIMITIVE_TYPE_CHAR <= type->type_id && type->type_id <= PRIMITIVE_TYPE_LONG_DOUBLE);
}

bool is_integer(const std::shared_ptr<ast_node> &node) {
    return PRIMITIVE_TYPE_CHAR <= node->symbol->type->type_id &&
           node->symbol->type->type_id <= PRIMITIVE_TYPE_UNSIGNED_LONG_LONG;
}

bool is_floating_number(const std::shared_ptr<ast_node> &node) {
    return PRIMITIVE_TYPE_FLOAT <= node->symbol->type->type_id &&
           node->symbol->type->type_id <= PRIMITIVE_TYPE_LONG_DOUBLE;
}

bool is_number(const std::shared_ptr<ast_node> &node) { return is_integer(node) || is_floating_number(node); }

//根据左右子表达式类型以及运算符表达式进行校验.如果是常量表达式则会求值
int construct_binary_expression_symbol(std::shared_ptr<ast_node> parent, int binary_operator,
                                       std::shared_ptr<ast_node> left, std::shared_ptr<ast_node> right) {
    std::shared_ptr<tsc_symbol> symbol = std::make_shared<tsc_symbol>();
    int semantics_analysis_result = 0;
    parent->symbol = symbol;
    parent->symbol->operator_id = binary_operator;
    parent->symbol->operands.push_back(left->symbol);
    parent->symbol->operands.push_back(right->symbol);
    symbol->value = std::make_shared<expression_value>();

    symbol->is_left_value = false;

    semantics_analysis_result = construct_binary_expression_symbol_type(parent, binary_operator, left, right);
    if (semantics_analysis_result)
        return semantics_analysis_result;

    //对于整数除法和mod检查分母为0.对于gcc如果分子可能不为0只会给warning
    // int f(int i){ return i/0;} warning: division by zero [-Wdiv-by-zero]
    //如果是constant expression但是left是浮点型的,结果会提升为浮点型,此时不需要warning或者error

    if (is_integer(left) && right->symbol->symbol_type == SYMBOL_TYPE_ICONSTANT &&
        right->symbol->value->unsigned_long_long_value == 0ull) {
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

    // todo 浮点型运算(包括整数与浮点数运算)
    // gcc & clang: void*p; p+1; ->OK. offset is 1

    return 0;
}

int construct_binary_expression_symbol_type(std::shared_ptr<ast_node> parent, int binary_operator,
                                            std::shared_ptr<ast_node> left, std::shared_ptr<ast_node> right) {
    int semantics_analysis_result = 0;
    //如果left right都是数值型(包括enum)则结果也是数值型且type_id是left right中type_id较大那个.
    //如果left right较大的是enum结果处理为int.例外是如果运算符是&&,||,>,>=,<,<=,==,!=结果类型为int
    int result_type_id;
    std::shared_ptr<tsc_symbol> symbol = parent->symbol;

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
    //下面保证left,right都是数字,指针或者数组
    if (is_number(left) && is_number(right)) {
        symbol->is_left_value = false;
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
        //如果左操作数是数组或者指针,右操作数是数字则operator必须是加减.如果右操作数也是数组或者指针,underlying type必须一样且operand只能是减
    else if (is_array_or_pointer(left)) {
        if (is_array_or_pointer(right)) {
            if (!check_same_type(left->symbol->type, right->symbol->type, false)) {
                printf("%s:%d error:\n\t'%s' and '%s' are not pointers to compatible types\n", input_file_name.c_str(),
                       left->get_first_terminal_line_no(), left->symbol->type->internal_name->c_str(),
                       right->symbol->type->internal_name->c_str());
                return 1;
            }
            //operand必须为减
            if (binary_operator != BINARY_OPERATOR_SUB) {
                printf("%s:%d error:\n\tinvalid operands to binary expression ('%s' and '%s')\n", input_file_name.c_str(),
                       left->get_first_terminal_line_no(), left->symbol->type->internal_name->c_str(),
                       right->symbol->type->internal_name->c_str());
                return 1;
            }
            //根据规范,两指针相减的结果类型为ptrdiff_t,此类型在 stddef.h 中定义.一般是与当前cpu位数相同的有符号整型(long)
            symbol->type = global_types::primitive_type_ptrdiff_t;
            symbol->is_left_value = false;
        } else {
            //right不是指针或者数组,则必须是整数.
            if (!is_integer(right)) {
                printf("%s:%d error:\n\tinvalid operands to binary expression ('%s' and '%s')\n", input_file_name.c_str(),
                       left->get_first_terminal_line_no(), left->symbol->type->internal_name->c_str(),
                       right->symbol->type->internal_name->c_str());
                return 1;
            } else {
                //left是指针,right是整数,结果类型与left相同,但是没有const qualifier
                symbol->type = construct_pointer_to(left->symbol->type->underlying_type);
                symbol->is_left_value = false;
            }
        }
    } else if (is_array_or_pointer(right)) {
        //left不是指针or数组,right是指针或者数组,则left必须为整数
        if (!is_integer(left)) {
            printf("%s:%d error:\n\tinvalid operands to binary expression ('%s' and '%s')\n", input_file_name.c_str(),
                   left->get_first_terminal_line_no(), left->symbol->type->internal_name->c_str(),
                   right->symbol->type->internal_name->c_str());
            return 1;
        } else {
            symbol->type = construct_pointer_to(right->symbol->type->underlying_type);
            symbol->is_left_value = false;
        }

    } else {
        printf("should not reach here %s:%d\n", __FILE__, __LINE__);
        return 1;
    }

    return semantics_analysis_result;
}

int construct_unary_expression_symbol(std::shared_ptr<ast_node> parent, int unary_operator,
                                      std::shared_ptr<ast_node> operand) {
    //先置为临时变量.如果发现是 constant expression 再进行修改
    parent->symbol->symbol_type = SYMBOL_TYPE_TEMPORARY_VARIABLE;
    parent->symbol->type = operand->symbol->type;
    parent->symbol->value = std::make_shared<expression_value>();
    //todo 根据类型进行运算
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
int analyze_init_declarator_list(std::shared_ptr<ast_node> init_declarator_list, semantics_analysis_context &context,
                                 std::shared_ptr<tsc_symbol> &symbol) {

    int semantics_analysis_result = 0;

    std::shared_ptr<ast_node> node = init_declarator_list;
    std::vector<std::shared_ptr<ast_node>> init_declarators;

    while (node->node_type == NODE_TYPE_INIT_DECLARATOR_LIST &&
           node->node_sub_type == NODE_TYPE_INIT_DECLARATOR_LIST_SUBTYPE_INIT_DECLARATOR_LIST_INIT_DECLARATOR) {
        init_declarators.push_back(node->items[2]);
        node = node->items[0];
    }

    init_declarators.push_back(node->items[0]);
    init_declarators = std::vector<std::shared_ptr<ast_node>>(init_declarators.rbegin(), init_declarators.rend());
    init_declarator_list->sub_nodes = init_declarators;
    for (std::shared_ptr<ast_node> &init_declarator : init_declarators) {
        std::shared_ptr<ast_node> out_identifier_node;
        semantics_analysis_result = analyze_init_declarator(init_declarator, context, symbol, out_identifier_node);
        if (semantics_analysis_result)
            return semantics_analysis_result;
        init_declarator_list->declarator_identifier_nodes.push_back(out_identifier_node);
    }

    return semantics_analysis_result;
}

/*
init_declarator
	: declarator '=' initializer
	| declarator
	;
 */
int analyze_init_declarator(std::shared_ptr<ast_node> init_declarator, semantics_analysis_context &context,
                            std::shared_ptr<tsc_symbol> &symbol, std::shared_ptr<ast_node> &out_identifier_node) {
    int semantics_analysis_result = 0;
    switch (init_declarator->node_sub_type) {
        case NODE_TYPE_INIT_DECLARATOR_SUBTYPE_DECLARATOR_ASSIGN_INITIALIZER: {
            std::shared_ptr<ast_node> declarator = init_declarator->items[0];
            std::shared_ptr<ast_node> initializer = init_declarator->items[2];
            declarator->symbol = std::make_shared<tsc_symbol>(*symbol);
            semantics_analysis_result = analyze_declarator(declarator, context, out_identifier_node);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            // extern 与 initializer 不能同时存在
            if (out_identifier_node->symbol->is_extern) {
                printf("%s:%d error:\n\t'%s' has both 'extern' and initializer\n", input_file_name.c_str(),
                       init_declarator->get_first_terminal_line_no(), out_identifier_node->symbol->identifier->c_str());
                return 1;
            }
            // after initialize array become complete
            if (!check_complete_type(out_identifier_node->symbol->type) &&
                out_identifier_node->symbol->type->type_id != SCALAR_TYPE_ARRAY) {
                printf("%s:%d error:\n\tstorage size of '%s' isn’t known\n", input_file_name.c_str(),
                       init_declarator->get_first_terminal_line_no(), out_identifier_node->symbol->identifier->c_str());
                return 1;
            }

            //检查当前作用域是否已经定义了同名符号.然后在解析initializer就需要加入符号表以允许形如 int a=a^a;的初始化(等价于int a=0)
            semantics_analysis_result =
                    add_declarator_identifier_to_symbol_table(init_declarator, context, out_identifier_node);
            if (semantics_analysis_result)
                return semantics_analysis_result;

            semantics_analysis_result = analyze_initializer(initializer, context, out_identifier_node->symbol);
            //todo check declarator->symbol and initializer symbol compatibility
        }

            break;
        case NODE_TYPE_INIT_DECLARATOR_SUBTYPE_DECLARATOR: {
            std::shared_ptr<ast_node> declarator = init_declarator->items[0];
            declarator->symbol = std::make_shared<tsc_symbol>(*symbol);
            semantics_analysis_result = analyze_declarator(declarator, context, out_identifier_node);
            if (semantics_analysis_result)
                return semantics_analysis_result;
            // for global int a[];->Ok, but not true inside function.

            if (!context.current_symbol_table_node->parent && out_identifier_node->symbol->type->type_id == SCALAR_TYPE_ARRAY &&
                !out_identifier_node->symbol->type->is_complete) {
                printf("%s:%d error:\n\tdefinition of variable with array type needs an explicit size or an initializer",
                       input_file_name.c_str(), init_declarator->get_first_terminal_line_no());
                return 1;
            } else if (!check_complete_type(out_identifier_node->symbol->type)) {
                printf("%s:%d error:\n\tstorage size of '%s' isn’t known\n", input_file_name.c_str(),
                       init_declarator->get_first_terminal_line_no(), out_identifier_node->symbol->identifier->c_str());
                return 1;
            }
            semantics_analysis_result =
                    add_declarator_identifier_to_symbol_table(init_declarator, context, out_identifier_node);
            if (semantics_analysis_result)
                return semantics_analysis_result;

        } break;
    }

    return semantics_analysis_result;
}

/*
initializer
: '{' initializer_list '}'
| '{' initializer_list ',' '}'
| assignment_expression
;
 */

int analyze_initializer(std::shared_ptr<ast_node> initializer, semantics_analysis_context &context,
                        std::shared_ptr<tsc_symbol> symbol_to_initialize) {
    int semantics_analysis_result = 0;
    //symbol_to_initialize could be nullptr when no need to assign
    switch (initializer->node_sub_type) {
        case NODE_TYPE_INITIALIZER_SUBTYPE_LEFT_BRACE_INITIALIZER__LIST_RIGHT_BRACE:
        case NODE_TYPE_INITIALIZER_SUBTYPE_LEFT_BRACE_INITIALIZER__LIST_COMMA_RIGHT_BRACE: {
            std::shared_ptr<ast_node> initializer_list = initializer->items[1];
            semantics_analysis_result = analyze_initializer_list(initializer_list, context, symbol_to_initialize);

        } break;
        case NODE_TYPE_INITIALIZER_SUBTYPE_ASSIGNMENT_EXPRESSION: {
            std::shared_ptr<ast_node> assignment_expression = initializer->items[0];
            semantics_analysis_result = analyze_assignment_expression(assignment_expression, context);

            if (symbol_to_initialize && !check_can_assign(symbol_to_initialize, assignment_expression->symbol)) {
                printf("%s:%d error:\n\tinitializing '%s' with an expression of incompatible type '%s'\n",
                       input_file_name.c_str(), initializer->get_first_terminal_line_no(),
                       initializer->symbol->type->internal_name->c_str(),
                       assignment_expression->symbol->type->internal_name->c_str());
                return 1;
            }
        }
    }

    return 0;
}

bool check_can_assign(const std::shared_ptr<tsc_symbol> &left, const std::shared_ptr<tsc_symbol> &right) {
    //todo
    return true;
}

int check_common_type(const std::shared_ptr<ast_node> &first, const std::shared_ptr<ast_node> &second,
                      std::shared_ptr<tsc_type> &out_type) {
    int semantics_analysis_result = 0;
    int result_type_id;
    if (is_number(first) && is_number(second)) {
        result_type_id = std::max(first->symbol->type->type_id, second->symbol->type->type_id);

        switch (result_type_id) {
            case PRIMITIVE_TYPE_CHAR:
                out_type = global_types::primitive_type_char;
                break;
            case PRIMITIVE_TYPE_UNSIGNED_CHAR:
                out_type = global_types::primitive_type_unsigned_char;
                break;
            case PRIMITIVE_TYPE_SHORT:
                out_type = global_types::primitive_type_short;
                break;
            case PRIMITIVE_TYPE_UNSIGNED_SHORT:
                out_type = global_types::primitive_type_unsigned_short;
                break;
            case PRIMITIVE_TYPE_ENUM:
                out_type = global_types::primitive_type_int;
                break;
            case PRIMITIVE_TYPE_INT:
                out_type = global_types::primitive_type_int;
                break;
            case PRIMITIVE_TYPE_UNSIGNED_INT:
                out_type = global_types::primitive_type_unsigned_int;
                break;
            case PRIMITIVE_TYPE_LONG:
                out_type = global_types::primitive_type_long;
                break;
            case PRIMITIVE_TYPE_UNSIGNED_LONG:
                out_type = global_types::primitive_type_unsigned_long;
                break;
            case PRIMITIVE_TYPE_LONG_LONG:
                out_type = global_types::primitive_type_long_long;
                break;
            case PRIMITIVE_TYPE_UNSIGNED_LONG_LONG:
                out_type = global_types::primitive_type_unsigned_long_long;
                break;
            case PRIMITIVE_TYPE_FLOAT:
                out_type = global_types::primitive_type_float;
                break;
            case PRIMITIVE_TYPE_DOUBLE:
                out_type = global_types::primitive_type_double;
                break;
            case PRIMITIVE_TYPE_LONG_DOUBLE:
                out_type = global_types::primitive_type_long_double;
                break;
            default:
                printf("should not reach here %s:%d\n", __FILE__, __LINE__);
                return 1;
        }
    }
    //todo 至少一个不是数字

    return semantics_analysis_result;
}

int add_declarator_identifier_to_symbol_table(std::shared_ptr<ast_node> init_declarator_or_declarator,
                                              semantics_analysis_context &context,
                                              std::shared_ptr<ast_node> declarator_identifier_node) {
    // 对于 global variable 可以重复声明.多次声明的时候类型必须一致如
    // int a; int a; ->OK
    // extern int a; int a; ->OK 根据gcc的测试此时实际上生效的是extern int a; 汇编文件中没有'a'这个 global 符号
    // const int a; int a; ->error
    // 但是局部变量不行 redeclaration of 'a' with no linkage
    std::shared_ptr<tsc_symbol> symbol = declarator_identifier_node->symbol;
    std::string identifier = *symbol->identifier;
    bool found_same_global_identifier = false;
    for (std::map<std::string, std::shared_ptr<tsc_symbol>>::iterator it =
            context.current_symbol_table_node->identifier_and_symbols.begin();
         it != context.current_symbol_table_node->identifier_and_symbols.end(); it++) {
        //可以多次声明但是只能定义1次.检查之前是否已经有complete的declaration.这里实际上只是检查了是否声明了其它类型的同名tag
        // 如 struct A; union A;->error
        if (it->first == identifier) {
            if (!context.current_symbol_table_node->parent) {
                //全局变量的声明允许相同. type qualifier必须一致. storage class specifier以首次声明为准.另外类型必须一致
                //全局变量不能是register的.这里的报错信息参考gcc
                found_same_global_identifier = true;
                if (symbol->is_register) {
                    printf("%s:%d error:\n\tregister internal_name not specified for '%s'\n", input_file_name.c_str(),
                           init_declarator_or_declarator->get_first_terminal_line_no(), identifier.c_str());
                    return 1;
                }
                //restrict则必须是指针
                if (symbol->type->restrict_type_qualifier_set && symbol->type->type_id != SCALAR_TYPE_POINTER) {
                    printf("%s:%d error:\n\tinvalid use of restrict for '%s' d\n", input_file_name.c_str(),
                           init_declarator_or_declarator->get_first_terminal_line_no(), identifier.c_str());
                    return 1;
                }

                if (!check_same_type(it->second->type, symbol->type, true)) {
                    printf("%s:%d error:\n\tconflicting types for '%s'\n", input_file_name.c_str(),
                           init_declarator_or_declarator->get_first_terminal_line_no(), identifier.c_str());
                    return 1;
                }
            } else {
                // 局部变量不可同名.实际上gcc对于两个同名变量的报错更精细一些.考虑是否有一个是extern的.
                // todo 局部变量中的函数可以重复声明
                printf("%s:%d error:\n\tredeclaration of identifier '%s'\n", input_file_name.c_str(),
                       init_declarator_or_declarator->get_first_terminal_line_no(), identifier.c_str());
                return 1;
            }
        }
    }

    if (!context.current_symbol_table_node->parent) {
        //全局变量且未找到同名的,将当前符号加入符号表
        if (!found_same_global_identifier)
            context.current_symbol_table_node->identifier_and_symbols[identifier] = symbol;

    } else {
        //局部变量之前已经检查过不存在同名因此直接加入
        context.current_symbol_table_node->identifier_and_symbols[identifier] = symbol;
    }
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
    semantics_analysis_result =
            check_type_specifiers(type_specifiers, context, symbol, DECLARATION_SPECIFIERS_LOCATION_NONE);
    if (semantics_analysis_result)
        return semantics_analysis_result;

    semantics_analysis_result = check_type_qualifiers(type_qualifiers, context, symbol);
    if (semantics_analysis_result)
        return semantics_analysis_result;
    specifier_qualifier_list->symbol = symbol;
    return 0;
}

/*
initializer_list
	: designation initializer
	| initializer
	| initializer_list ',' designation initializer
	| initializer_list ',' initializer
	;
 */
int analyze_initializer_list(std::shared_ptr<ast_node> initializer_list, semantics_analysis_context &context,
                             std::shared_ptr<tsc_symbol> &symbol_to_initialize) {
    // https://gcc.gnu.org/onlinedocs/gcc/Designated-Inits.html
    // Omitted field members are implicitly initialized the same as objects that have static storage duration
    int semantics_analysis_result = 0;
    // symbol_to_initialize could be nullptr
    std::vector<std::vector<std::shared_ptr<ast_node>>> &initializer_or_designation_initializers =
            initializer_list->initializer_or_designation_initializers;
    std::shared_ptr<ast_node> node = initializer_list;
    while (true) {
        bool has_next = true;
        switch (node->node_sub_type) {
            case NODE_TYPE_INITIALIZER_LIST_SUBTYPE_DESIGNATION_INITIALIZER:
                initializer_or_designation_initializers.push_back({node->items[0], node->items[1]});
                has_next = false;
                break;
            case NODE_TYPE_INITIALIZER_LIST_SUBTYPE_INITIALIZER:
                initializer_or_designation_initializers.push_back({node->items[0]});
                has_next = false;
                break;
            case NODE_TYPE_INITIALIZER_LIST_SUBTYPE_INITIALIZER_LIST_COMMA_DESIGNATION_INITIALIZER:
                initializer_or_designation_initializers.push_back({node->items[2], node->items[3]});
                node = node->items[0];
                break;
            case NODE_TYPE_INITIALIZER_LIST_SUBTYPE_INITIALIZER_LIST_COMMA_INITIALIZER:
                initializer_or_designation_initializers.push_back({node->items[2]});
                node = node->items[0];
                break;
        }
        if (!has_next)
            break;
    }

    initializer_or_designation_initializers = std::vector<std::vector<std::shared_ptr<ast_node>>>(
            initializer_or_designation_initializers.rbegin(), initializer_or_designation_initializers.rend());

    // int i={0}; ->OK
    // int i={0,1} or struct A{} a; int i={0,a}->OK. warning: excess elements in scalar initializer
    // if identifier 'a' is not defined int i={0,a}->error
    // int a[1]={0,1} ->OK. warning: excess elements in array initializer
    // int b={[0]=2}; ->error designator in initializer for scalar type 'int'

    bool is_scalar = symbol_to_initialize && is_scalar_type(symbol_to_initialize->type);
    if (is_scalar && initializer_or_designation_initializers.size() > 1) {
        printf("%s:%d warning:\n\texcess elements in scalar '%s' initializer\n", input_file_name.c_str(),
               initializer_list->get_first_terminal_line_no(), symbol_to_initialize->type->internal_name->c_str());
    }
    //array or struct or union
    int initialize_index = 0;
    //for array
    int max_initialize_index = 0;
    for (size_t index = 0; index < initializer_or_designation_initializers.size(); index++) {
        const std::vector<std::shared_ptr<ast_node>> &initializer_or_designation_initializer =
                initializer_or_designation_initializers[index];

        std::shared_ptr<ast_node> initializer;
        switch (initializer_or_designation_initializer.size()) {
            case 1: {
                initializer = initializer_or_designation_initializer[0];
            } break;
            case 2: {
                std::shared_ptr<ast_node> designation = initializer_or_designation_initializer[0];
                initializer = initializer_or_designation_initializer[1];

                if (is_scalar) {
                    printf("%s:%d error:\n\tdesignator in initializer for scalar type '%s'\n", input_file_name.c_str(),
                           initializer_list->get_first_terminal_line_no(), symbol_to_initialize->type->internal_name->c_str());
                    semantics_analysis_result = 1;
                    return semantics_analysis_result;
                }
                //this function may change designated_symbol_to_initialize
                std::shared_ptr<tsc_symbol> designated_symbol_to_initialize = symbol_to_initialize;
                semantics_analysis_result = analyze_designation(designation, context, designated_symbol_to_initialize);
                if (semantics_analysis_result)
                    return semantics_analysis_result;
            } break;
        }

        semantics_analysis_result = analyze_initializer(initializer, context, symbol_to_initialize);
        if (semantics_analysis_result)
            return semantics_analysis_result;
        if (symbol_to_initialize) {

            if (is_scalar) {
                if (index == 0) {
                    semantics_analysis_result = analyze_initializer(initializer, context, symbol_to_initialize);
                } else {
                    // no symbol to initialize, but still need to analyze_initializer
                    semantics_analysis_result = analyze_initializer(initializer, context, std::shared_ptr<tsc_symbol>());
                }
                if (semantics_analysis_result)
                    return semantics_analysis_result;
            } else {
                //not scalar, could be array or struct_union
                std::shared_ptr<tsc_symbol> next_symbol_to_initialize = std::make_shared<tsc_symbol>();
                //todo construct next_symbol_to_initialize
                switch (symbol_to_initialize->type->type_id) {
                    case SCALAR_TYPE_ARRAY: {
                        // int a1[1]={1,2}; int a1[2]={[1]=2,2}; ->warning: excess elements in array initializer
                        if (symbol_to_initialize->type->is_complete && *symbol_to_initialize->type->array_length < initialize_index) {
                            printf("%s:%d warning:\n\texcess elements in array initializer\n", input_file_name.c_str(),
                                   initializer_list->get_first_terminal_line_no());
                        } else {

                            semantics_analysis_result = analyze_initializer(initializer, context, next_symbol_to_initialize);
                            if (semantics_analysis_result)
                                return semantics_analysis_result;
                        }
                        max_initialize_index = std::max(max_initialize_index, initialize_index);
                    } break;
                    case RECORD_TYPE_STRUCT_OR_UNION: {
                        if (initialize_index >= symbol_to_initialize->type->fields.size()) {
                            printf("%s:%d warning:\n\texcess elements in '%s' initializer\n", input_file_name.c_str(),
                                   initializer_list->get_first_terminal_line_no(), symbol_to_initialize->type->internal_name->c_str());

                        } else {
                            semantics_analysis_result = analyze_initializer(initializer, context, next_symbol_to_initialize);
                            if (semantics_analysis_result)
                                return semantics_analysis_result;
                        }

                    } break;
                    default:
                        printf("%s:%d error:\n\tshould not reach here\n", input_file_name.c_str(),
                               initializer_list->get_first_terminal_line_no());
                        return 1;
                }
                initialize_index++;
            }
        } else {
            // here symbol_to_initialize = nullptr
            semantics_analysis_result = analyze_initializer(initializer, context, symbol_to_initialize);
            if (semantics_analysis_result)
                return semantics_analysis_result;
        }
    }
    // after assign, array type is complete
    // int a[]={[10]=2,[2]=10}; -> sizeof(a)=44
    if (symbol_to_initialize && symbol_to_initialize->type->type_id == SCALAR_TYPE_ARRAY &&
        !symbol_to_initialize->type->is_complete) {
        symbol_to_initialize->type->is_complete = true;
        symbol_to_initialize->type->array_length = std::make_shared<int>(max_initialize_index + 1);
    }

    return semantics_analysis_result;
}

/*
designation
	: designator_list '='
	;

designator_list
	: designator
	| designator_list designator
	;
*/

int analyze_designation(std::shared_ptr<ast_node> designation, semantics_analysis_context &context,
                        std::shared_ptr<tsc_symbol> &designated_symbol_to_initialize) {

    std::vector<std::shared_ptr<ast_node>> designators;
    std::shared_ptr<ast_node> designator_list = designation->items[0];
    std::shared_ptr<ast_node> node = designator_list;
    int semantics_analysis_result = 0;

    while (node->node_type == NODE_TYPE_DESIGNATOR_LIST &&
           node->node_sub_type == NODE_TYPE_DESIGNATOR_LIST_SUBTYPE_DESIGNATOR_LIST_DESIGNATOR) {
        designators.push_back(node->items[1]);
        node = node->items[0];
    }

    designators.push_back(node->items[0]);

    designators = std::vector<std::shared_ptr<ast_node>>(designators.rbegin(), designators.rend());
    designator_list->sub_nodes = designators;
    for (std::shared_ptr<ast_node> designator : designators) {
        semantics_analysis_result = analyze_designator(designator, context, designated_symbol_to_initialize);
        if (semantics_analysis_result)
            return semantics_analysis_result;
    }
    return semantics_analysis_result;
}

/*
designator
	: '[' constant_expression ']'
	| '.' IDENTIFIER
	;
*/
int analyze_designator(std::shared_ptr<ast_node> designator, semantics_analysis_context &context,
                       std::shared_ptr<tsc_symbol> &designated_symbol_to_initialize) {
    int semantics_analysis_result = 0;

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
    global_types::primitive_type_void->internal_name = std::make_shared<std::string>("void");

    global_types::primitive_type_char->type_id = PRIMITIVE_TYPE_CHAR;
    global_types::primitive_type_char->internal_name = std::make_shared<std::string>("char");
    global_types::primitive_type_char->type_size = sizeof(char);

    global_types::primitive_type_unsigned_char->type_id = PRIMITIVE_TYPE_UNSIGNED_CHAR;
    global_types::primitive_type_unsigned_char->internal_name = std::make_shared<std::string>("unsigned char");
    global_types::primitive_type_unsigned_char->type_size = sizeof(unsigned char);

    global_types::primitive_type_short->type_id = PRIMITIVE_TYPE_SHORT;
    global_types::primitive_type_short->internal_name = std::make_shared<std::string>("short");
    global_types::primitive_type_short->type_size = sizeof(short);

    global_types::primitive_type_unsigned_short->type_id = PRIMITIVE_TYPE_UNSIGNED_SHORT;
    global_types::primitive_type_unsigned_short->internal_name = std::make_shared<std::string>("unsigned short");
    global_types::primitive_type_unsigned_short->type_size = sizeof(unsigned short);

    global_types::primitive_type_int->type_id = PRIMITIVE_TYPE_INT;
    global_types::primitive_type_int->internal_name = std::make_shared<std::string>("int");
    global_types::primitive_type_int->type_size = sizeof(int);

    global_types::primitive_type_unsigned_int->type_id = PRIMITIVE_TYPE_UNSIGNED_INT;
    global_types::primitive_type_unsigned_int->internal_name = std::make_shared<std::string>("unsigned int");
    global_types::primitive_type_unsigned_int->type_size = sizeof(unsigned int);

    global_types::primitive_type_long->type_id = PRIMITIVE_TYPE_LONG;
    global_types::primitive_type_long->internal_name = std::make_shared<std::string>("long");
    global_types::primitive_type_long->type_size = sizeof(long);

    global_types::primitive_type_unsigned_long->type_id = PRIMITIVE_TYPE_UNSIGNED_LONG;
    global_types::primitive_type_unsigned_long->internal_name = std::make_shared<std::string>("unsigned long");
    global_types::primitive_type_unsigned_long->type_size = sizeof(unsigned long);

    global_types::primitive_type_long_long->type_id = PRIMITIVE_TYPE_LONG_LONG;
    global_types::primitive_type_long_long->internal_name = std::make_shared<std::string>("long long");
    global_types::primitive_type_long_long->type_size = sizeof(long long);

    global_types::primitive_type_unsigned_long_long->type_id = PRIMITIVE_TYPE_UNSIGNED_LONG_LONG;
    global_types::primitive_type_unsigned_long_long->internal_name = std::make_shared<std::string>("unsigned long long");
    global_types::primitive_type_unsigned_long_long->type_size = sizeof(unsigned long long);

    global_types::primitive_type_float->type_id = PRIMITIVE_TYPE_FLOAT;
    global_types::primitive_type_float->internal_name = std::make_shared<std::string>("float");
    global_types::primitive_type_float->type_size = sizeof(float);

    global_types::primitive_type_double->type_id = PRIMITIVE_TYPE_DOUBLE;
    global_types::primitive_type_double->internal_name = std::make_shared<std::string>("double");
    global_types::primitive_type_double->type_size = sizeof(double);

    global_types::primitive_type_long_double->type_id = PRIMITIVE_TYPE_LONG_DOUBLE;
    global_types::primitive_type_long_double->internal_name = std::make_shared<std::string>("long double");
    global_types::primitive_type_long_double->type_size = sizeof(long double);

    global_types::primitive_type_const_void->type_id = PRIMITIVE_TYPE_VOID;
    global_types::primitive_type_const_void->internal_name = std::make_shared<std::string>("void");
    //gcc extension sizeof(void)=1 clang says it's error

    global_types::primitive_type_const_char->type_id = PRIMITIVE_TYPE_CHAR;
    global_types::primitive_type_const_char->internal_name = std::make_shared<std::string>("char");
    global_types::primitive_type_const_char->type_size = sizeof(char);

    global_types::primitive_type_const_unsigned_char->type_id = PRIMITIVE_TYPE_UNSIGNED_CHAR;
    global_types::primitive_type_const_unsigned_char->internal_name = std::make_shared<std::string>("unsigned char");
    global_types::primitive_type_const_unsigned_char->type_size = sizeof(unsigned char);

    global_types::primitive_type_const_short->type_id = PRIMITIVE_TYPE_SHORT;
    global_types::primitive_type_const_short->internal_name = std::make_shared<std::string>("short");
    global_types::primitive_type_const_short->type_size = sizeof(short);

    global_types::primitive_type_const_unsigned_short->type_id = PRIMITIVE_TYPE_UNSIGNED_SHORT;
    global_types::primitive_type_const_unsigned_short->internal_name = std::make_shared<std::string>("unsigned short");
    global_types::primitive_type_const_unsigned_short->type_size = sizeof(unsigned short);

    global_types::primitive_type_const_int->type_id = PRIMITIVE_TYPE_INT;
    global_types::primitive_type_const_int->internal_name = std::make_shared<std::string>("int");
    global_types::primitive_type_const_int->type_size = sizeof(int);

    global_types::primitive_type_const_unsigned_int->type_id = PRIMITIVE_TYPE_UNSIGNED_INT;
    global_types::primitive_type_void->internal_name = std::make_shared<std::string>("unsigned int");
    global_types::primitive_type_const_unsigned_int->type_size = sizeof(unsigned int);

    global_types::primitive_type_const_long->type_id = PRIMITIVE_TYPE_LONG;
    global_types::primitive_type_void->internal_name = std::make_shared<std::string>("void");
    global_types::primitive_type_const_long->type_size = sizeof(long);

    global_types::primitive_type_const_unsigned_long->type_id = PRIMITIVE_TYPE_UNSIGNED_LONG;
    global_types::primitive_type_const_unsigned_long->internal_name = std::make_shared<std::string>("unsigned long");
    global_types::primitive_type_const_unsigned_long->type_size = sizeof(unsigned long);

    global_types::primitive_type_const_long_long->type_id = PRIMITIVE_TYPE_LONG_LONG;
    global_types::primitive_type_const_long_long->internal_name = std::make_shared<std::string>("long long");
    global_types::primitive_type_const_long_long->type_size = sizeof(long long);

    global_types::primitive_type_const_unsigned_long_long->type_id = PRIMITIVE_TYPE_UNSIGNED_LONG_LONG;
    global_types::primitive_type_const_unsigned_long_long->internal_name =
            std::make_shared<std::string>("unsigned long long");
    global_types::primitive_type_const_unsigned_long_long->type_size = sizeof(unsigned long long);

    global_types::primitive_type_const_float->type_id = PRIMITIVE_TYPE_FLOAT;
    global_types::primitive_type_const_float->internal_name = std::make_shared<std::string>("float");
    global_types::primitive_type_const_float->type_size = sizeof(float);

    global_types::primitive_type_const_double->type_id = PRIMITIVE_TYPE_DOUBLE;
    global_types::primitive_type_const_double->internal_name = std::make_shared<std::string>("double");
    global_types::primitive_type_const_double->type_size = sizeof(double);

    global_types::primitive_type_const_long_double->type_id = PRIMITIVE_TYPE_LONG_DOUBLE;
    global_types::primitive_type_const_long_double->internal_name = std::make_shared<std::string>("long double");
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

    switch (sizeof(std::ptrdiff_t)) {
        case 4:
            global_types::primitive_type_ptrdiff_t = global_types::primitive_type_int;
            break;
        case 8:
        default:
            global_types::primitive_type_ptrdiff_t = global_types::primitive_type_long;
            break;
    }
}

std::shared_ptr<tsc_type> construct_pointer_to(std::shared_ptr<tsc_type> type) {
    std::shared_ptr<tsc_type> pointer = std::make_shared<tsc_type>();
    pointer->type_id = SCALAR_TYPE_POINTER;
    pointer->underlying_type = type;
    pointer->type_size = sizeof(void *);
    std::string internal_name = "pointer";
    if (type->internal_name) {
        internal_name += " to ";
        internal_name += (*type->internal_name);
    }

    pointer->internal_name = std::make_shared<std::string>(internal_name);
    return pointer;
}

std::shared_ptr<tsc_type> construct_array_of(std::shared_ptr<tsc_type> type) {
    std::shared_ptr<tsc_type> array = std::make_shared<tsc_type>();
    array->type_id = SCALAR_TYPE_ARRAY;
    array->underlying_type = type;
    array->internal_name = std::make_shared<std::string>("array");
    return array;
}

std::shared_ptr<tsc_symbol> lookup_symbol(std::shared_ptr<symbol_table_node> symbol_table_node,
                                          const std::string &symbol_identifier, bool search_outer) {

    if (symbol_table_node->identifier_and_symbols.find(symbol_identifier) !=
        symbol_table_node->identifier_and_symbols.end())
        return symbol_table_node->identifier_and_symbols[symbol_identifier];
    if (search_outer && symbol_table_node->parent)
        return lookup_symbol(symbol_table_node->parent, symbol_identifier, search_outer);
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

std::shared_ptr<tsc_statement_context>
find_parent_statement_context(const std::shared_ptr<tsc_statement_context> &current_statement_context,
                              int parent_statement_type_to_find) {
    if (!current_statement_context)
        return std::shared_ptr<tsc_statement_context>();
    std::shared_ptr<tsc_statement_context> current_parent_statement_context =
            current_statement_context->parent_statement_context;
    while (current_parent_statement_context) {
        if (current_parent_statement_context->statement_type == parent_statement_type_to_find)
            return current_parent_statement_context;
        else
            return find_parent_statement_context(current_parent_statement_context->parent_statement_context,
                                                 parent_statement_type_to_find);
    }
    return std::shared_ptr<tsc_statement_context>();
}
