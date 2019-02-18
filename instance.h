#ifndef INSTANCE_H
#define INSTANCE_H

#include "vehicle.h"
#include "instance.h"
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
	vector<vector<double>> arcs;
	
	// Solution variables
	vector<Vehicle> routes;
	double obj_val;
	
	public:
		Instance(){};
		void create_instance(vector<vector<int>> &input_data, int ins);
		void preprocess();
		void calculate_arcs();
		void calculate_obj_val();
		double get_arc(size_t row, size_t column);
		void initial_solution();
};

void Instance::calculate_obj_val() {
	double total_distance = 0;
	for (Vehicle v : routes) {
		total_distance += accumulate(v.arc_durations.begin(),v.arc_durations.end(),0);
	}
	double facility_costs = 0;
	for (Transfer_Node node : transfer_nodes) {
		if (node.open) {
			facility_costs += node.costs;
		}
	}
	obj_val = travel_cost*total_distance + facility_costs;
}

double Instance::get_arc(size_t row, size_t column) {
	return arcs[row][column];
}

#endif