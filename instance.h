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
	size_t index = 0, request_amount = 0, transfer_location_amount = 0, depot_amount = 0, node_amount = 0, travel_cost = 0, vehicle_capacity = 0;

	vector<int> ride_times;

	// Solution variables
	vector<Vehicle> routes = {};
	vector<Vehicle> old_routes = {};
	vector<size_t> old_pickup_vehicle = {};
	vector<size_t> old_delivery_vehicle = {};
	double obj_val = 0;

	public:
		Instance(){};
		void create_instance(vector<vector<int>> &input_data, int ins);
		void preprocess();
		void calculate_arcs();
		double calculate_obj_val();
		void initial_solution();
		void write_output_file(size_t instance_number);

		// Deletion
		size_t greedy_request_deletion(vector<size_t> request_bank);
		size_t random_request_deletion(vector<size_t> request_bank);
		void greedy_route_deletion();
		void random_route_deletion() ;
		double costs_of_removing_request(size_t request);

		// Insertion
		void greedy_request_insertion(size_t request);
		void random_request_insertion();
		void greedy_route_insertion();
		void random_route_insertion();
		double costs_of_inserting_request(Vehicle v, size_t p, size_t d, size_t request);

		// Feasibility
		bool is_feasible();
		bool is_request_bank_empty();
		bool maximum_ride_time_not_exceeded();
		bool time_windows_met();
		bool capacity_feasible();

		void print_routes();
};

inline void Instance::print_routes(){
    for(size_t idx = 0; idx < routes.size(); idx++)
    {
        for(size_t adx = 0; adx < routes[idx].route.size(); adx++)
        {
            cout << routes[idx].route[adx].gen_idx << "  ";
        }
        cout << '\n';
    }
    cout << "\n\n";
}

#endif
