#ifndef VEHICLE_H
#define VEHICLE_H

#include "node.h"
#include <iostream>
#include <vector>
using namespace std;

class Vehicle {
  public:
	size_t index;
	vector<double> arc_durations;
	vector<double> waiting_times;
	vector<Node> route;
	vector<size_t> current_capacity;
	vector<double> time_at_node; //mist nog implementatie
	
	void remove_node(size_t location);
	void add_node(size_t location, Pickup_Node node, double arc1, double arc2);
	void add_node(size_t location, Delivery_Node node, double arc1, double arc2);
	void add_node(size_t location, Transfer_Node node, double arc1, double arc2);
	void add_node(size_t location, Depot_Node node, double arc1, double arc2);
};

#endif
