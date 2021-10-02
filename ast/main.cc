#include <stdio.h>
#include "parser.h"
#include "lex.yy.h"
#include "semantics_analysis.h"

int main() {
    setup_type_system();
    input_file_name = "../test_sources/test3.c";
    FILE *file = fopen(input_file_name.c_str(), "r+");
    if (!file) {
        printf("no such file\n");
        return 1;
    }
    yyset_in(file);
    yyset_out(stdout);
    int parse_result = yyparse();
    if (parse_result) {
        fclose(file);
        return parse_result;
    }
    std::string input_file = translation_unit->get_expression();
    printf("%s\n", input_file.c_str());
    int semantics_analysis_result = semantics_analysis(translation_unit);
    if (semantics_analysis_result) {
        fclose(file);
        return semantics_analysis_result;
    }
    fclose(file);
    return 0;
}