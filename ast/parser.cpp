#include <algorithm>
#include <stdio.h>
#include "parser.h"
#include "lex.yy.h"

std::shared_ptr<ast_node> translation_unit;

int main(){
  FILE *file = fopen("test_source.c", "r+");
  yyset_in(file);
  yyset_out(stdout);
  yyparse();
  fclose(file);
  return 0;
}