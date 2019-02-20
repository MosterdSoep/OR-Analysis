#ifndef INSTANCE_H
#define INSTANCE_H

#include "vehicle.h"
#include <numeric>
#include <string>
#include <iostream>
#include <vector>
#include <limits>

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
	
	// Solution variables
	vector<Vehicle> routes;
	double obj_val;
	
	public:
		Instance(){};
		void create_instance(vector<vector<int>> &input_data, int ins);
		void preprocess();
		void calculate_arcs();
		void calculate_obj_val();
		void initial_solution();
		void write_output_file(size_t instance_number);
		
		bool is_feasible();
		bool is_request_bank_empty();
		bool maximum_ride_time_not_exceeded();
		bool time_windows_met();
		bool capacity_feasible();
		bool correct_routes();
};


#endif
