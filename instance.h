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
	size_t index = 0, request_amount = 0, transfer_location_amount = 0, depot_amount = 0, node_amount = 0, travel_cost = 0;

	//vector<int> ride_times;

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
		size_t greedy_request_deletion(vector<size_t> &request_bank);
		size_t random_request_deletion(vector<size_t> &request_bank);
		void greedy_route_deletion(vector<size_t> &request_bank);
		void random_route_deletion(vector<size_t> &request_bank);
		vector<size_t> cluster_deletion(vector<size_t> &request_bank, size_t k);
		vector<size_t> transfer_swap(vector<size_t> &request_bank);
		double costs_of_removing_request(size_t &request);
		void remove_empty_vehicle();
		vector<size_t> shaw_deletion(size_t &amount);

		// Insertion
		size_t greedy_request_insertion(vector<size_t> &request_bank);
		size_t regret_2_insertion(vector<size_t> &request_bank);
		size_t random_request_greedy_insertion(vector<size_t> &request_bank);
		size_t greedy_route_insertion(vector<size_t> &request_bank);
		double costs_of_inserting_request(Vehicle &v, size_t p, size_t d, size_t request, vector<double> &information);
		double costs_of_inserting_request_with_transfer_pickup(Vehicle &v, size_t p, size_t d, size_t request, Transfer_Node &tn, vector<double> &information);
		double costs_of_inserting_request_with_transfer_delivery(Vehicle &v, size_t p, size_t d, size_t request, Transfer_Node &tn, vector<double> &information);

		// Feasibility
		bool is_feasible();
		bool is_request_bank_empty();
		bool maximum_ride_time_not_exceeded();
		bool time_windows_met();
		bool capacity_feasible();

		double pickup_feasible(Vehicle &v, size_t &p, size_t &request, Node &pick);
		double delivery_feasible(Vehicle &v, size_t p, size_t d, size_t request, double p_delay, double &added_time, Node &pick, Node &del);
		bool check_slack_times(Vehicle &v, size_t first, size_t last, double delay);
		bool insertion_ride_times_feasible(Vehicle &v, size_t p, double delay);
		bool insertion_delivery_times_feasible(Vehicle &v, size_t p, size_t d, double delay_p, double delay_d);


		void print_routes();
};

inline void Instance::print_routes(){
    for(size_t idx = 0; idx < routes.size(); idx++)
    {
        for(size_t adx = 0; adx < routes[idx].route.size(); adx++)
        {
            if(routes[idx].route[adx].type == 't'){
            cout << 't' << routes[idx].route[adx].gen_idx << "  ";
            }else{
            cout << routes[idx].route[adx].gen_idx << "  ";
            }
        }
        cout << '\n';
    }
    cout << "\n\n";
}

#endif
