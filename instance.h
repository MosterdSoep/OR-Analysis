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
	public:
	int index=0, request_amount=0, transfer_location_amount=0, depot_amount=0, node_amount=0, travel_cost=0, vehicle_capacity=0;

	vector<int> ride_times;

	// Solution variables
	vector<Vehicle> routes = {};
	double obj_val = 0;

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
