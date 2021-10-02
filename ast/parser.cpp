#include <algorithm>
#include "node.h"

ast_node::ast_node() { node_type_id = 0; }
ast_node::ast_node(int node_type_id, std::shared_ptr<ast_token> token) : node_type_id(node_type_id), token(token) {}

int main(){
  return 0;
}