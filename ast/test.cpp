#include "token.h"
#include <string.h>

int main(){
    const char* file_buffer="  extern int a;";
    ast_parse_context context;
    context.file_buffer=file_buffer;
    context.file_length=strlen(file_buffer);
    context.line_number=0;
    context.parsed_char_count=0;
    std::shared_ptr<ast_token> token=get_next_token(context);
    printf("%d\n",token->token_type_id);//2=extern
    token=get_next_token(context);
    printf("%d\n",token->token_type_id);//13=int
    token=get_next_token(context);
    printf("%d\n",token->token_type_id);//33=TOKEN_TYPE_IDENTIFIER
    return 0;
}