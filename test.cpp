#include "ast/node.h"
#include "ast/lex.yy.h"
#include <stdio.h>

void test1()
{
  const char file_buffer[] = "  volatile int a=123;";
  ast_parse_context context;
  context.file_buffer = file_buffer;
  context.file_length = strlen(file_buffer);
  context.line_number = 0;
  context.parsed_char_count = 0;
  context.unexpected_eof = false;
  std::shared_ptr<ast_token> token = get_next_token(context);
  while (token)
  {
    printf("%d %s\n", token->token_type_id, token->value.c_str());
    token = get_next_token(context);
  }
  printf("eof\n");
}



int main()
{
  return 0;
}