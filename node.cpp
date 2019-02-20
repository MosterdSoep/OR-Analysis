#include "node.h"

// Constructor with all attributes
Node::Node(int idx, int a, int b, int st, int g_idx){
  index = idx;
  x = a;
  y = b;
  service_time = st;
  gen_idx = g_idx;
}
