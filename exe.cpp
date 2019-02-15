#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>
#include <random>
#include "vehicle.h"
#include "node.h"
using namespace std;

// General variables
string location = "C://Users//Hp//Desktop//Master//Blok3//ORACS//instances.csv";

// Model variables
int request_amount, transfer_location_amount, depot_amount, node_amount, travel_cost, vehicle_capacity;
vector<Pickup_Node> pickup_nodes;
vector<Delivery_Node> delivery_nodes;
vector<Transfer_Node> transfer_nodes;
vector<Depot_Node> depot_nodes;
vector<Node> all_nodes;
vector<int> ride_times;
vector<int> service_times;
vector<vector<int>> arcs;

// Solution variables
vector<Vehicle> routes;
double ojb_val;

void read_csv() {
	ifstream ip(location);
	if(!ip.is_open()) cout << "ERROR: File Open" << '\n';

	while (ip.good()) {
		int index;
		string index_str, request_amount_str, transfer_location_amount_str, depot_amount_str, vehicle_capacity_str, travel_cost_str;
		
		getline(ip,index_str,','); 						index = atoi(index_str.c_str());
		getline(ip,request_amount_str,','); 			request_amount = atoi(request_amount_str.c_str());
		getline(ip,transfer_location_amount_str,','); 	transfer_location_amount = atoi(transfer_location_amount_str.c_str());
		getline(ip,depot_amount_str,','); 				depot_amount = atoi(depot_amount_str.c_str());
		getline(ip,vehicle_capacity_str,','); 			vehicle_capacity = atoi(vehicle_capacity_str.c_str());
		getline(ip,travel_cost_str,','); 				travel_cost = atoi(travel_cost_str.c_str());
		
		node_amount = 2*request_amount + transfer_location_amount + depot_amount;
		
		// Facility costs
		vector<int> facility_costs;
		for (int i = 0; i < transfer_location_amount; i++) {
			int facility_cost;
			string facility_cost_str;
			getline(ip,facility_cost_str,',');
			facility_cost = atoi(facility_cost_str.c_str());
			facility_costs.push_back(facility_cost);
		}
		
		size_t general_index = 0;
		// Pickup nodes
		for (int i = 0; i < request_amount; i++) {
			string x,y;
			pickup_nodes.push_back(Pickup_Node());
			pickup_nodes[i].index = i;
			getline(ip,x,',');
			getline(ip,y,',');
			pickup_nodes[i].x = atoi(x.c_str());
			pickup_nodes[i].y = atoi(y.c_str());
			pickup_nodes[i].gen_idx = general_index;
			all_nodes.push_back(pickup_nodes[i]);
			general_index++;
		}
		// Delivery nodes
		for (int i = 0; i < request_amount; i++) {
			string x,y;
			delivery_nodes.push_back(Delivery_Node());
			delivery_nodes[i].index = i;
			getline(ip,x,',');
			getline(ip,y,',');
			delivery_nodes[i].x = atoi(x.c_str());
			delivery_nodes[i].y = atoi(y.c_str());
			delivery_nodes[i].gen_idx = general_index;
			all_nodes.push_back(delivery_nodes[i]);
			general_index++;
		}
		// Transfer nodes
		for (int i = 0; i < transfer_location_amount; i++) {
			string x,y;
			transfer_nodes.push_back(Transfer_Node());
			transfer_nodes[i].index = i;
			getline(ip,x,',');
			getline(ip,y,',');
			transfer_nodes[i].x = atoi(x.c_str());
			transfer_nodes[i].y = atoi(y.c_str());
			transfer_nodes[i].gen_idx = general_index;
			transfer_nodes[i].costs = facility_costs[i];
			all_nodes.push_back(transfer_nodes[i]);
			general_index++;
		}
		// Depot nodes
		for (int i = 0; i < depot_amount; i++) {
			string x,y;
			depot_nodes.push_back(Depot_Node());
			depot_nodes[i].index = i;
			getline(ip,x,',');
			getline(ip,y,',');
			depot_nodes[i].x = atoi(x.c_str());
			depot_nodes[i].y = atoi(y.c_str());
			depot_nodes[i].gen_idx = general_index;
			all_nodes.push_back(depot_nodes[i]);
			general_index++;
		}
		// Pickup nodes time windows
		for (int i = 0; i < request_amount; i++) {
			string lower_bound, upper_bound;
			getline(ip,lower_bound,',');
			getline(ip,upper_bound,',');
			pickup_nodes[i].lower_bound = atoi(lower_bound.c_str());
			pickup_nodes[i].upper_bound = atoi(upper_bound.c_str());
		}
		// Delivery nodes time windows
		for (int i = 0; i < request_amount; i++) {
			string lower_bound, upper_bound;
			getline(ip,lower_bound,',');
			getline(ip,upper_bound,',');
			delivery_nodes[i].lower_bound = atoi(lower_bound.c_str());
			delivery_nodes[i].upper_bound = atoi(upper_bound.c_str());
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

double eucledian_distance(Node a, Node b) {
	return sqrt(pow(a.x - b.x,2) + pow(a.y - b.y,2));
}

void calculate_arcs() {
	arcs.resize(node_amount);
	for (int i = 0; i < node_amount; i++) {
		arcs[i].resize(node_amount);
	}
	for (int i = 0; i < node_amount; i++) {
		for (int j = i + 1; j < node_amount; j++) {
			arcs[i][j] = eucledian_distance(all_nodes[i],all_nodes[j]);
			arcs[j][i] = arcs[i][j];
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

size_t worst_removal(int amount_removed) {
	// 1. Find the worst point in a route by looping over the vehicle vectors and calculating the difference in costs.
	// 2. Once a location has been found, remove it from the vehicle by altering all the vectors.
	
	size_t best_removal = 0;
	size_t current_removal;
	size_t index_removed_node;
	size_t removed_node_index;
	Vehicle vehicle_removed_node;
	for (Vehicle v : routes) {
		for (Node n : v.route) {
			// If first node is removed then just substract the first transportation cost
			current_removal = arcs[v.route[i-1]][v.route[i+1]] - arcs[v.route[i-1]][v.route[i]] - arcs[v.route[i]][v.route[i+1]];
			if (current_removal < best_removal) { 
				best_removal = current_removal;
				index_removed_node = i;
				removed_node_index = i;
				vehicle_removed_node = v;
			}
		}
	}
	// Remove the node from the vehicle
	vehicle_removed_node.route.erase(index_removed_node);
	return removed_node_index;
}

size_t destroy() {
	size_t destroyed_node_index = worst_removal(1);
}

void greedy_insertion(size_t node_index) {
	// 1. Find the worst point in a route by looping over the vehicle vectors and calculating the difference in costs.
	// 2. Once a location has been found, remove it from the vehicle by altering all the vectors.
	node = all_nodes[node_index];
	bool first = true;
	size_t best_insertion = 0;
	size_t current_insertion;
	size_t index_insertion;
	Vehicle vehicle_insertion;
	for (Vehicle v : routes) {
		for (int i : v.route) {
			// If first node is removed then just substract the first transportation cost
			current_removal =  arcs[v.route[i-1]][node.gen_idx] + arcs[v.route[i]][node.gen_idx] - arcs[v.route[i-1]][v.route[i]];
			if (first) { current_insertion = best_insertion; first = false; }
			if (current_insertion <= best_insertion) { 
				best_insertion = current_insertion;
				index_insertion = i;
				vehicle_insertion = v;
			}
		}
	}
	// Remove the node from the vehicle
	vehicle_insertion.route.insert(index_insertion+1);
}

void repair(size_t node_index) {
	greedy_insertion(node_index);
}

double ALNS(double init_solution) {
	double best_solution;
	double current_solution;
	
	size_t loop_count = 0;
	while(!stopping_criterion_met(loop_count)) {
		double s = current_solution;
		size_t destroyed_node_index = destroy();
		repair(destroyed_node_index);
		
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

double create_init_solution() {
	
	return 0;
}

double calculate_obj_val() {
	// Costs:
	// 1. Opening transfer facility -> add up the distance and then multiply with the cost per distance
	// 2. Travel costs
	double total_distance = 0;
	for (Vehicle v : routes) {
		total_distance += accumulate(v.arc_durations.begin(),v.arc_durations.end(),0);
	}
	double facility_costs = 0;
	for (Transfer_Node node : transfer_nodes) {
		if (node.open) {
			facility_cost += node.costs;
		}
	}
	return (travel_cost*total_distance + facility_cost);
}

void write_output_file() {
	
}

int main() {
	read_csv();
	calculate_arcs();
	cout << pickup_nodes[0].x << '\n';
	double init_solution = create_init_solution();
	/*double best_solution = ALNS(init_solution);
	cout << best_solution;*/
	write_output_file();
    return 0;
}