#include <algorithm>
#include <stdio.h>
#include "parser.h"
#include "lex.yy.h"

std::shared_ptr<ast_node> translation_unit;

std::string ast_node::get_expression(){
  if(lexeme)return *lexeme;
  std::string expr;
  for(size_t i=0;i<items.size()-1;i++ ){
    expr+=items[i]->get_expression();
    expr+=" ";
  }
  expr+=items[items.size()-1]->get_expression();
  return expr;
  
}


int main(){
  FILE *file = fopen("test_source.c", "r+");
  yyset_in(file);
  yyset_out(stdout);
  int parse_result=yyparse();
  if(parse_result)return parse_result;
  std::shared_ptr<ast_node> node=translation_unit;
  for(size_t i=0;i<node->items.size();i++){
    printf("%s\n",node->items[i]->get_expression().c_str());
  }
  
  fclose(file);
  return 0;
}