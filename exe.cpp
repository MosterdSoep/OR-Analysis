#include <iostream>
#include <vector>
#include <fstream>
#include "vehicle.h"
using namespace std;

/*
struct node {
	size_t idx, x, y, service_time;
};

struct time_window {
	double node_idx, lower_bound, upper_bound;
};
*/

// General variables
String location = "";

// Model variables
/*
vector<size_t> facility_costs;
vector<node> pickup_nodes;
vector<node> delivery_nodes;
vector<node> transfer_nodes;
vector<node> depot_nodes;
vector<time_window> pickup_time_windows;
vector<time_window> delivery_time_windows;
vector<size_t> ride_times;
vector<size_t> service_times;
*/

// Solution variables
vector<Vehicle> routes;
double ojb_val;

int main() {
	read_csv();
	double init_solution = create_init_solution();
	double best_solution = ALNS(init_solution);
	cout << best_solution;
    return 0;
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

void read_csv() {
	ifstream ip(location);
	while (ip.good) {
		size_t index, request_amount, transfer_location_amount, depot_amount, vehicle_capacity, travel_cost;
		
		getline(ip,index,',');
		getline(ip,request_amount,',');
		getline(ip,transfer_location_amount,',');
		getline(ip,depot_amount,',');
		getline(ip,vehicle_capacity,',');
		getline(ip,travel_cost,',');
		
		// Facility costs
		for (int i = 0; i < transfer_location_amount; i++) {
			size_t facility_cost;
			getline(ip,facility_cost,',');
			facility_costs.push_back(facility_cost);
		}
		// Pickup nodes
		for (int i = 0; i < request_amount; i++) {
			pickup_nodes.push_back(node());
			pickup_nodes[i].idx = i;
			getline(ip,pickup_nodes[i].x,',');
			getline(ip,pickup_nodes[i].y,',');
		}
		// Delivery nodes
		for (int i = 0; i < request_amount; i++) {
			delivery_nodes.push_back(node());
			delivery_nodes[i].idx = i;
			getline(ip,delivery_nodes[i].x,',');
			getline(ip,delivery_nodes[i].y,',');
		}
		// Transfer nodes
		for (int i = 0; i < transfer_location_amount; i++) {
			transfer_nodes.push_back(node());
			transfer_nodes[i].idx = i;
			getline(ip,transfer_nodes[i].x,',');
			getline(ip,transfer_nodes[i].y,',');
		}
		// Depot nodes
		for (int i = 0; i < depot_amount; i++) {
			depot_nodes.push_back(node());
			depot_nodes[i].idx = i;
			getline(ip,depot_nodes[i].x,',');
			getline(ip,depot_nodes[i].y,',');
		}
		// Pickup nodes time windows
		for (int i = 0; i < request_amount; i++) {
			pickup_time_windows.push_back(time_window());
			pickup_time_windows[i].node_idx = i;
			getline(ip,pickup_time_windows[i].lower_bound,',');
			getline(ip,pickup_time_windows[i].upper_bound,',');
		}
		// Delivery nodes time windows
		for (int i = 0; i < request_amount; i++) {
			delivery_time_windows.push_back(time_window());
			delivery_time_windows[i].node_idx = i;
			getline(ip,delivery_time_windows[i].lower_bound,',');
			getline(ip,delivery_time_windows[i].upper_bound,',');
		}
		// Maximum ride times
		for (int i = 0; i < request_amount; i++) {
			size_t ride_time;
			getline(ip,ride_time,',');
			ride_times.push_back(ride_time);
		}
		// Service times pickup nodes
		for (int i = 0; i < request_amount; i++) {
			getline(ip,pickup_nodes[i].service_time,',');
		}
		// Service times delivery nodes
		for (int i = 0; i < request_amount; i++) {
			getline(ip,pickup_nodes[i].delivery_nodes,',');
		}
		// Service times transfer nodes
		for (int i = 0; i < transfer_location_amount; i++) {
			getline(ip,transfer_nodes[i].service_time,',');
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

void destroy(double s) {
	const int range_from = 0;
	const int range_to = 100;
	random_device rand_dev;
	mt19937 generator(rand_dev());
	uniform_int_distribution<int> distr(range_from, range_to);

	random_number = distr(generator);
	
	if (random_number <= 100) { worst_removal(s); }
	
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

