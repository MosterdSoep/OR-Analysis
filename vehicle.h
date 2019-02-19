#ifndef VEHICLE_H
#define VEHICLE_H

#include "node.h"
#include <iostream>
#include <vector>
using namespace std;

class Vehicle {
  public:
	size_t index;
	vector<Node> route;
	vector<double> arc_durations;		// Length of every arc that the vehicle traverses
	vector<double> waiting_times;		// The amount of time the vehicle waits at every node
	vector<size_t> current_capacity;	// The amount of requests in the vehicle at every node
	vector<double> time_at_node;		// The arrival time and consequently the begin of service time at every node
	
	void remove_node(size_t location);
	void add_node(size_t location, Pickup_Node node);
	void add_node(size_t location, Delivery_Node node);
	void add_node(size_t location, Transfer_Node node);
	void add_node(size_t location, Depot_Node node);
};

#endif