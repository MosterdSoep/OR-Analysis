#ifndef VEHICLE_H
#define VEHICLE_H

#include <iostream>
#include <vector>
#include "node.h"
using namespace std;

class Vehicle {
  public:
	size_t index;
	vector<double> arc_durations;
	vector<double> waiting_times;
	vector<Node> route;
	vector<size_t> current_capacity;
	
	void remove_node(size_t location);
	void add_node(size_t location, Pickup_Node node, double arc1, double arc2);
	void add_node(size_t location, Delivery_Node node, double arc1, double arc2);
	void add_node(size_t location, Transfer_Node node, double arc1, double arc2);
	void add_node(size_t location, Depot_Node node, double arc1, double arc2);
};

void Vehicle::remove_node(size_t location){
	this->arc_durations.erase(arc_durations.begin() + location);
	this->waiting_times.erase(waiting_times.begin() + location);
	this->route.erase(route.begin() + location);
	this->current_capacity.erase(current_capacity.begin() + location);
};

// Add transfer node
void Vehicle::add_node(size_t location, Transfer_Node node, double arc1, double arc2){
	this->arc_durations.insert(arc_durations.begin() + location, arc1);
	this->arc_durations.insert(arc_durations.begin() + location + 1, arc2);
	this->route.insert(route.begin() + location, node);
	
	// Waiting times and current capacity may need to be changed according
	// to the specific node that is added
	this->waiting_times.insert(waiting_times.begin() + location, 2, waiting_times[location]);
	this->current_capacity.insert(current_capacity.begin() + location, 2, current_capacity[location]);
};

// Add pickup node
void Vehicle::add_node(size_t location, Pickup_Node node, double arc1, double arc2){
	this->arc_durations.insert(arc_durations.begin() + location, arc1);
	this->arc_durations.insert(arc_durations.begin() + location + 1, arc2);
	this->route.insert(route.begin() + location, node);
	
	// Waiting times and current capacity may need to be changed according
	// to the specific node that is added
	this->waiting_times.insert(waiting_times.begin() + location, 2, waiting_times[location]);
	this->current_capacity.insert(current_capacity.begin() + location, 2, current_capacity[location]);
};

// Add delivery node
void Vehicle::add_node(size_t location, Delivery_Node node, double arc1, double arc2){
	this->arc_durations.insert(arc_durations.begin() + location, arc1);
	this->arc_durations.insert(arc_durations.begin() + location + 1, arc2);
	this->route.insert(route.begin() + location, node);
	
	// Waiting times and current capacity may need to be changed according
	// to the specific node that is added
	this->waiting_times.insert(waiting_times.begin() + location, 2, waiting_times[location]);
	this->current_capacity.insert(current_capacity.begin() + location, 2, current_capacity[location]);
};

// Add depot node
void Vehicle::add_node(size_t location, Depot_Node node, double arc1, double arc2){
	this->arc_durations.insert(arc_durations.begin() + location, arc1);
	this->arc_durations.insert(arc_durations.begin() + location + 1, arc2);
	this->route.insert(route.begin() + location, node);
	
	// Waiting times and current capacity may need to be changed according
	// to the specific node that is added
	this->waiting_times.insert(waiting_times.begin() + location, 2, waiting_times[location]);
	this->current_capacity.insert(current_capacity.begin() + location, 2, current_capacity[location]);
};

#endif
