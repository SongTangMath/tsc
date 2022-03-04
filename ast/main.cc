#include <stdio.h>
#include "parser.h"
#include "lex.yy.h"
#include "semantics_analysis.h"

int main() {
  input_file_name = "test_source.c";
  FILE *file = fopen(input_file_name.c_str(), "r+");
  yyset_in(file);
  yyset_out(stdout);
  int parse_result = yyparse();
  if (parse_result) {
    fclose(file);
    return parse_result;
  }
  printf("%s\n", translation_unit->get_expression().c_str());
  int semantics_analysis_result = semantics_analysis(translation_unit);
  if (semantics_analysis_result) {
    fclose(file);
    return semantics_analysis_result;
  }
  fclose(file);
  return 0;
}