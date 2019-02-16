#ifndef NODE_H
#define NODE_H

#include <iostream>
using namespace std;

class Node {
  public:
	double index, x, y, service_time, gen_idx;
	Node(double index, double x, double y, double st, double g_idx);
	Node(){};
};

class Pickup_Node: public Node{
  public:
    using Node::Node;
    double lower_bound = 0;
    double upper_bound = 0;
};

class Delivery_Node: public Node{
  public:
    using Node::Node;
    double lower_bound = 0;
    double upper_bound = 0;
};

class Transfer_Node: public Node{
  public:
    using Node::Node;
    bool open;
    bool pickup; //1 if pickup, 0 if delivery
    size_t vehicle_idx;
    size_t request_idx;
	double costs;
};

class Depot_Node: public Node{
  public:
    using Node::Node;
};

// Constructor with all attributes
Node::Node(double index, double x, double y, double st, double g_idx){
  index = idx;
  x = a;
  y = b;
  service_time = st;
  gen_idx = g_idx;
}

#endif
