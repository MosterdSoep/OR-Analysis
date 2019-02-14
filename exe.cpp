#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "vehicle.h"
#include "instance.h"
#include "node.h"
#include "time_window.h"
using namespace std;

// General variables
string location = "C://Users//Hp//Desktop//Master//Blok3//ORACS//instances.csv";
vector<Instance> instances;

// Model variables
vector<size_t> facility_costs;
vector<Node> pickup_nodes;
vector<Node> delivery_nodes;
vector<Node> transfer_nodes;
vector<Node> depot_nodes;
vector<Time_window> pickup_time_windows;
vector<Time_window> delivery_time_windows;
vector<size_t> ride_times;
vector<size_t> service_times;


// Solution variables
vector<Vehicle> routes;
double ojb_val;


void read_csv() {
	ifstream ip(location);
	while (ip.good()) {
		size_t index, request_amount, transfer_location_amount, depot_amount, vehicle_capacity, travel_cost;
		string index_str, request_amount_str, transfer_location_amount_str, depot_amount_str, vehicle_capacity_str, travel_cost_str;
		
		getline(ip,index_str,','); 						index = atoi(index_str);
		getline(ip,request_amount_str,','); 			request_amount = atoi(request_amount_str);
		getline(ip,transfer_location_amount_str,','); 	transfer_location_amount = atoi(transfer_location_amount_str);
		getline(ip,depot_amount_str,','); 				depot_amount = atoi(depot_amount_str);
		getline(ip,vehicle_capacity_str,','); 			vehicle_capacity = atoi(vehicle_capacity_str);
		getline(ip,travel_cost_str,','); 				travel_cost = atoi(travel_cost_str);
		
		// Facility costs
		for (int i = 0; i < transfer_location_amount; i++) {
			size_t facility_cost;
			string facility_cost_str;
			getline(ip,facility_cost,',');
			facility_cost = atoi(facility_cost_str);
			facility_costs.push_back(facility_cost);
		}
		// Pickup nodes
		for (int i = 0; i < request_amount; i++) {
			string x,y;
			pickup_nodes.push_back(node());
			pickup_nodes[i].idx = i;
			getline(ip,x,',');
			getline(ip,y,',');
			pickup_nodes[i].x = atoi(x);
			pickup_nodex[i].y = atoi(y);
		}
		// Delivery nodes
		for (int i = 0; i < request_amount; i++) {
			string x,y;
			delivery_nodes.push_back(node());
			delivery_nodes[i].idx = i;
			getline(ip,x,',');
			getline(ip,y,',');
			delivery_nodes[i].x = atoi(x);
			delivery_nodes[i].y = atoi(y);
		}
		// Transfer nodes
		for (int i = 0; i < transfer_location_amount; i++) {
			string x,y;
			transfer_nodes.push_back(node());
			transfer_nodes[i].idx = i;
			getline(ip,x,',');
			getline(ip,y,',');
			transfer_nodes[i].x = atoi(x);
			transfer_nodes[i].y = atoi(y);
		}
		// Depot nodes
		for (int i = 0; i < depot_amount; i++) {
			string x,y;
			depot_nodes.push_back(node());
			depot_nodes[i].idx = i;
			getline(ip,x,',');
			getline(ip,y,',');
			depot_nodes[i].x = atoi(x);
			depot_nodes[i].y = atoi(y);
		}
		// Pickup nodes time windows
		for (int i = 0; i < request_amount; i++) {
			string lower_bound, upper_bound;
			pickup_time_windows.push_back(time_window());
			pickup_time_windows[i].node_idx = i;
			getline(ip,lower_bound,',');
			getline(ip,upper_bound,',');
			pickup_time_windows[i].lower_bound = atoi(lower_bound);
			pickup_time_windows[i].upper_bound = atoi(upper_bound);
		}
		// Delivery nodes time windows
		for (int i = 0; i < request_amount; i++) {
			string lower_bound, upper_bound;
			delivery_time_windows.push_back(time_window());
			delivery_time_windows[i].node_idx = i;
			getline(ip,lower_bound,',');
			getline(ip,upper_bound,',');
			delivery_time_windows[i].lower_bound = atoi(lower_bound);
			delivery_time_windows[i].upper_bound = atoi(upper_bound);
		}
		// Maximum ride times
		for (int i = 0; i < request_amount; i++) {
			size_t ride_time;
			string ride_time_str;
			getline(ip,ride_time_str,',');
			ride_time = atoi(ride_time_str);
			ride_times.push_back(ride_time);
		}
		// Service times pickup nodes
		for (int i = 0; i < request_amount; i++) {
			string service_time_str;
			getline(ip,service_time_str,',');
			pickup_nodes[i].service_time = atoi(service_time_str);
		}
		// Service times delivery nodes
		for (int i = 0; i < request_amount; i++) {
			string service_time_str;
			getline(ip,service_time_str,',');
			pickup_nodes[i].delivery_nodes = atoi(service_time_str);
		}
		// Service times transfer nodes
		for (int i = 0; i < transfer_location_amount; i++) {
			string service_time_str;
			getline(ip,service_time_str,',');
			transfer_nodes[i].service_time = atoi(service_time_str);
		}
	}
}

bool stopping_criterion_met(size_t loop_count) {
	if (loop_count <= 1000) { return false; }
	else { return true; }
}

bool acceptation_criterion_met(double candid_solution, double current_solution) {
	if (current_solution - candid_solution < 0) { return false; }
	else { return true; }
}

void worst_removal(double s) {
	// 1. Find the worst point in a route by looping over the vehicle vectors and calculating the difference in costs.
	// 2. Once a location has been found, remove it from the vehicle by altering all the vectors.
	
	size_t best_removal = 0;
	size_t current_removal;
	size_t removed_node;
	vector vehicle_removed_node;
	for (vector v : routes) {
		for (size_t i : v) {
			// If first node is removed then just substract the first transportation cost
			current_removal = Arc(v(i-1),v(i+1)) - Arc(v(i-1),v(i)) - Arc(v(i),v(i+1));
			if (current_removal >= best_removal) { 
				best_removal = current_removal;
				removed_node = v(i);
				vehicle_removed_node = v;
			}
		}
	}
	// Remove the node from the vehicle
	
	return s;
}

void destroy(double s) {
	const int range_from = 0;
	const int range_to = 100;
	random_device rand_dev;
	mt19937 generator(rand_dev());
	uniform_int_distribution<int> distr(range_from, range_to);

	random_number = distr(generator);
	
	if (random_number <= 100) { worst_removal(s); }
	
}

double ALNS(double init_solution) {
	double best_solution;
	double current_solution;
	
	size_t loop_count = 0;
	while(!stopping_criterion_met(loop_count)) {
		double s = current_solution;
		destroy(s);
		repair(s);
		
		// check feasibility and compute obj_val for s
		
		if (s < best_solution) {
			best_solution = s;
			current_solution = s;
		} else {
			if (acceptation_criterion_met(s, current_solution)) {
				current_solution = s;
			}
		}
		loop_count++;
	}
	return best_solution;
}

int main() {
	read_csv();
	double init_solution = create_init_solution();
	double best_solution = ALNS(init_solution);
	cout << best_solution;
    return 0;
}