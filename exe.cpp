#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>
#include "vehicle.h"
#include "instance.h"
#include "node.h"
#include "time_window.h"
using namespace std;

// General variables
string location = "C://Users//Hp//Desktop//Master//Blok3//ORACS//instances.csv";
vector<Instance> instances;

// Model variables
vector<int> facility_costs;
vector<Node> pickup_nodes;
vector<Node> delivery_nodes;
vector<Node> transfer_nodes;
vector<Node> depot_nodes;
vector<Time_window> pickup_time_windows;
vector<Time_window> delivery_time_windows;
vector<int> ride_times;
vector<int> service_times;
double **arcs;

// Solution variables
vector<Vehicle> routes;
double ojb_val;


void read_csv() {
	ifstream ip(location);
	if(!ip.is_open()) cout << "ERROR: File Open" << '\n';

	while (ip.good()) {
		int index, request_amount, transfer_location_amount, depot_amount, vehicle_capacity, travel_cost;
		string index_str, request_amount_str, transfer_location_amount_str, depot_amount_str, vehicle_capacity_str, travel_cost_str;
		
		getline(ip,index_str,','); 						index = atoi(index_str.c_str());
		getline(ip,request_amount_str,','); 			request_amount = atoi(request_amount_str.c_str());
		getline(ip,transfer_location_amount_str,','); 	transfer_location_amount = atoi(transfer_location_amount_str.c_str());
		getline(ip,depot_amount_str,','); 				depot_amount = atoi(depot_amount_str.c_str());
		getline(ip,vehicle_capacity_str,','); 			vehicle_capacity = atoi(vehicle_capacity_str.c_str());
		getline(ip,travel_cost_str,','); 				travel_cost = atoi(travel_cost_str.c_str());
		
		// Facility costs
		for (int i = 0; i < transfer_location_amount; i++) {
			int facility_cost;
			string facility_cost_str;
			getline(ip,facility_cost_str,',');
			facility_cost = atoi(facility_cost_str.c_str());
			facility_costs.push_back(facility_cost);
		}
		// Pickup nodes
		for (int i = 0; i < request_amount; i++) {
			string x,y;
			pickup_nodes.push_back(Node());
			pickup_nodes[i].index = i;
			getline(ip,x,',');
			getline(ip,y,',');
			pickup_nodes[i].x = atoi(x.c_str());
			pickup_nodes[i].y = atoi(y.c_str());
		}
		// Delivery nodes
		for (int i = 0; i < request_amount; i++) {
			string x,y;
			delivery_nodes.push_back(Node());
			delivery_nodes[i].index = i;
			getline(ip,x,',');
			getline(ip,y,',');
			delivery_nodes[i].x = atoi(x.c_str());
			delivery_nodes[i].y = atoi(y.c_str());
		}
		// Transfer nodes
		for (int i = 0; i < transfer_location_amount; i++) {
			string x,y;
			transfer_nodes.push_back(Node());
			transfer_nodes[i].index = i;
			getline(ip,x,',');
			getline(ip,y,',');
			transfer_nodes[i].x = atoi(x.c_str());
			transfer_nodes[i].y = atoi(y.c_str());
		}
		// Depot nodes
		for (int i = 0; i < depot_amount; i++) {
			string x,y;
			depot_nodes.push_back(Node());
			depot_nodes[i].index = i;
			getline(ip,x,',');
			getline(ip,y,',');
			depot_nodes[i].x = atoi(x.c_str());
			depot_nodes[i].y = atoi(y.c_str());
		}
		// Pickup nodes time windows
		for (int i = 0; i < request_amount; i++) {
			string lower_bound, upper_bound;
			pickup_time_windows.push_back(Time_window());
			pickup_time_windows[i].node_index = i;
			getline(ip,lower_bound,',');
			getline(ip,upper_bound,',');
			pickup_time_windows[i].lower_bound = atoi(lower_bound.c_str());
			pickup_time_windows[i].upper_bound = atoi(upper_bound.c_str());
		}
		// Delivery nodes time windows
		for (int i = 0; i < request_amount; i++) {
			string lower_bound, upper_bound;
			delivery_time_windows.push_back(Time_window());
			delivery_time_windows[i].node_index = i;
			getline(ip,lower_bound,',');
			getline(ip,upper_bound,',');
			delivery_time_windows[i].lower_bound = atoi(lower_bound.c_str());
			delivery_time_windows[i].upper_bound = atoi(upper_bound.c_str());
		}
		// Maximum ride times
		for (int i = 0; i < request_amount; i++) {
			size_t ride_time;
			string ride_time_str;
			getline(ip,ride_time_str,',');
			ride_time = atoi(ride_time_str.c_str());
			ride_times.push_back(ride_time);
		}
		// Service times pickup nodes
		for (int i = 0; i < request_amount; i++) {
			string service_time_str;
			getline(ip,service_time_str,',');
			pickup_nodes[i].service_time = atoi(service_time_str.c_str());
		}
		// Service times delivery nodes
		for (int i = 0; i < request_amount; i++) {
			string service_time_str;
			getline(ip,service_time_str,',');
			delivery_nodes[i].service_time = atoi(service_time_str.c_str());
		}
		// Service times transfer nodes
		int i;
		for (i = 0; i < transfer_location_amount-1; i++) {
			string service_time_str;
			getline(ip,service_time_str,',');
			transfer_nodes[i].service_time = atoi(service_time_str.c_str());
		}
		string service_time_str;
		getline(ip,service_time_str,'\n');
		transfer_nodes[i].service_time = atoi(service_time_str.c_str());
	}
	ip.close();
}

void calculate_arcs() {
	arcs = malloc((2*request_amount+transfer_location_amount+depot_amount)*sizeof(double*));
	for (i = 0; i < request_amount; i++) {
		arcs[i] = (double*)malloc(rows*sizeof(double));
		for (j = 0; j < request_amount; j++) {
			arcs[i][j] = sqrt(pow(pickup_nodes[i].x-pickup_nodes[j].x,2)+pow(pickup_nodes[i].y-pickup_nodes[j].y,2));
			arcs[i+request_amount][j+request_amount] = sqrt(pow(delivery_nodes[i].x-delivery_nodes[j].x,2)+pow(delivery_nodes[i].y-delivery_nodes[j].y,2));
			arcs[i+2*request_amount][j+2*request_amount] = sqrt(pow(transfer_nodes[i].x-transfer_nodes[j].x,2)+pow(transfer_nodes[i].y-transfer_nodes[j].y,2));
			arcs[i+2*request_amount][j+2*request_amount] = sqrt(pow(depot_nodes[i].x-depot_nodes[j].x,2)+pow(depot_nodes[i].y-depot_nodes[j].y,2));
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
	vector<Vehicle> vehicle_removed_node;
	for (Vehicle v : routes) {
		for (int i : v.route) {
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
	calculate_arcs();
	double init_solution = create_init_solution();
	double best_solution = ALNS(init_solution);
	cout << best_solution;
    return 0;
}