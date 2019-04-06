#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>
#include <limits>
#include <assert.h>
using namespace std;

class Node {
  public:
	int index = 0, x = 0, y = 0, gen_idx = 0;
	double service_time = 0.0;
	char type;
	bool pickup;
	bool delivery;
    size_t request_idx = 0;
    double lower_bound = 0;
    double upper_bound = numeric_limits<double>::max();
};

class Pickup_Node: public Node{
  public:
    using Node::Node;
};

class Delivery_Node: public Node{
  public:
    using Node::Node;
};

class Transfer_Node: public Node{
  public:
    using Node::Node;
    bool open;
    size_t vehicle_idx;		// Create instance based on user input
	double costs;
};

class Depot_Node: public Node{
  public:
    using Node::Node;
};

#endif