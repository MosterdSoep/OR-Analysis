#ifndef VEHICLE_H
#define VEHICLE_H

#include "node.h"
#include <iostream>
#include <vector>

using namespace std;

extern vector<vector<double>> arcs;
extern vector<Pickup_Node> pickup_nodes;
extern vector<Delivery_Node> delivery_nodes;
extern vector<Transfer_Node> transfer_nodes;
extern vector<Depot_Node> depot_nodes;
extern vector<Node> all_nodes;
extern vector<size_t> pickup_vehicle;
extern vector<size_t> delivery_vehicle;
extern vector<double> ride_times;
extern size_t vehicle_capacity;

class Vehicle {
  public:
	size_t v_index = 0;
	vector<Node> route = {};
	vector<double> arc_durations = {0};			// Length of every arc that the vehicle traverses
	vector<double> waiting_times = {0,0};		// The amount of time the vehicle waits at every node
	vector<size_t> current_capacity = {0,0};	// The amount of requests in the vehicle at every node
	vector<double> time_at_node = {0,0};		// The arrival time and consequently the begin of service time at every node
	vector<double> slack_at_node = {0,0};	
	
	void remove_node(size_t location);
	void add_node(size_t location, Pickup_Node &node);
	void add_node(size_t location, Delivery_Node &node);
    double add_delivery_transfer(size_t location, Transfer_Node &node, size_t r);
    void add_pickup_transfer(size_t location, Transfer_Node &node, double min_time, size_t r);
	bool maximum_ride_time_correct();
	bool time_windows_correct();
	bool current_capacity_correct();

	void change_first_depot();
	void change_last_depot();
	Vehicle();
};

#endif
