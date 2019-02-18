#ifndef INSTANCE_H
#define INSTANCE_H

#include "vehicle.h"
#include <string>
#include <iostream>
#include <vector>

double euclidian_distance(Node a, Node b);

class Instance{
	// Model variables
	int index, request_amount, transfer_location_amount, depot_amount, node_amount, travel_cost, vehicle_capacity;
	vector<Pickup_Node> pickup_nodes;
	vector<Delivery_Node> delivery_nodes;
	vector<Transfer_Node> transfer_nodes;
	vector<Depot_Node> depot_nodes;
	vector<Node> all_nodes;
	vector<int> ride_times;
	vector<int> service_times;
	vector<vector<double>> arcs;
	
	vector<Vehicle> routes;
	
	public:
		instance(){};
		void create_instance(vector<vector<int>> &input_data, int ins);
		void preprocess();
		void calculate_arcs();
		void initial::solution();
};
#endif