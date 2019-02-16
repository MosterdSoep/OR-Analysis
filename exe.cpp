#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <random>
#include "vehicle.h"
#include "node.h"
using namespace std;

// General variables
string location = "D://Downloads//instances.csv";
vector<vector<int>> input_data;
// Model variables
int request_amount, transfer_location_amount, depot_amount, node_amount, travel_cost, vehicle_capacity;
vector<Pickup_Node> pickup_nodes;
vector<Delivery_Node> delivery_nodes;
vector<Transfer_Node> transfer_nodes;
vector<Depot_Node> depot_nodes;
vector<Node> all_nodes;
vector<int> ride_times;
vector<int> service_times;
vector<vector<double>> arcs;

// Solution variables
vector<Vehicle> routes;
double obj_val;

void read_csv() {
	ifstream ip(location);
	if(!ip.is_open()) cout << "ERROR: File Open" << '\n';

    string line;
	while (getline(ip, line)) {
        vector<int> tmp_data;
        string input_var;
        istringstream line_stream(line);
        while(getline(line_stream, input_var, ',')){
            int csv_element = atoi(input_var.c_str());
            tmp_data.push_back(csv_element);
        }
        input_data.push_back(tmp_data);
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

/*

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

void calculate_obj_val() {
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
	obj_val = travel_cost*total_distance + facility_cost;
}

void write_output_file(size_t instance_number) {
	ofstream output_file;
	ostringstream file_name_stream;
	file_name_stream << "oracs_" << instance_number << ".csv";
	string file_name = file_name_stream.str();
	output_file.open(file_name);
	output_file << "2\n";
	output_file << instance_number << "\n";
	output_file << obj_val << "\n";
	output_file << routes.size() << "\n";
	for (Vehicle v : routes) {
		output_file << "";
	}
	output_file << "\n";
}
*/
void create_instance(int ins){
    int index = input_data[ins][0];
    request_amount = input_data[ins][1];
    transfer_location_amount = input_data[ins][2];
    depot_amount = input_data[ins][3];
    vehicle_capacity = input_data[ins][4];
    travel_cost = input_data[ins][5];
    node_amount = 2 * request_amount + transfer_location_amount + depot_amount;

    size_t transfer_cost_idx = 6;
    size_t coordinate_idx = transfer_cost_idx + transfer_location_amount;
    size_t time_window_idx = coordinate_idx + 2 * node_amount;
    size_t ride_times_idx = time_window_idx + 4 * request_amount;
    size_t service_time_idx = ride_times_idx + request_amount;

    for(size_t idx = 0; idx < request_amount; idx++){
        // Create pickup nodes
        pickup_nodes.push_back(Pickup_Node());
        pickup_nodes[idx].index = idx;
        pickup_nodes[idx].x = input_data[ins][coordinate_idx + 2 * idx];
        pickup_nodes[idx].y = input_data[ins][coordinate_idx + 2 * idx + 1];
        pickup_nodes[idx].service_time = input_data[ins][service_time_idx + idx];
        pickup_nodes[idx].gen_idx = idx;
        pickup_nodes[idx].lower_bound = input_data[ins][time_window_idx + 2* idx];
        pickup_nodes[idx].upper_bound = input_data[ins][time_window_idx + 2* idx + 1];

        // Create delivery nodes
        delivery_nodes.push_back(Delivery_Node());
        delivery_nodes[idx].index = idx;
        delivery_nodes[idx].x = input_data[ins][coordinate_idx + 2 * request_amount + 2 * idx];
        delivery_nodes[idx].y = input_data[ins][coordinate_idx + 2 * request_amount + 2 * idx + 1];
        delivery_nodes[idx].service_time = input_data[ins][service_time_idx + request_amount + idx];
        delivery_nodes[idx].gen_idx = request_amount + idx;
        delivery_nodes[idx].lower_bound = input_data[ins][time_window_idx + 2 * request_amount + 2 * idx];
        delivery_nodes[idx].upper_bound = input_data[ins][time_window_idx + 2 * request_amount + 2 * idx + 1];
    }

    for(size_t idx = 0; idx < transfer_location_amount; idx++){
        //Create transfer nodes
        transfer_nodes.push_back(Transfer_Node());
        transfer_nodes[idx].index = idx;
        transfer_nodes[idx].x = input_data[ins][coordinate_idx + 4 * request_amount + 2 * idx];
        transfer_nodes[idx].y = input_data[ins][coordinate_idx + 4 * request_amount + 2 * idx + 1];
        transfer_nodes[idx].service_time = input_data[ins][service_time_idx + 2 * request_amount + idx];
        transfer_nodes[idx].gen_idx = 2 * request_amount + idx;
        transfer_nodes[idx].costs = input_data[ins][transfer_cost_idx + idx];
    }

    for(size_t idx = 0; idx < depot_amount; idx++){
        //Create depot nodes
        depot_nodes.push_back(Depot_Node());
        depot_nodes[idx].index = idx;
        depot_nodes[idx].x = input_data[ins][coordinate_idx + 4 * request_amount + 2 * transfer_location_amount + 2 * idx];
        depot_nodes[idx].y = input_data[ins][coordinate_idx + 4 * request_amount + 2 * transfer_location_amount + 2 * idx + 1];
        depot_nodes[idx].service_time = 0;
        depot_nodes[idx].gen_idx = 2 * request_amount + transfer_location_amount + idx;
    }

    for(size_t idx = 0; idx < request_amount; idx++){
        ride_times.push_back(input_data[ins][ride_times_idx + idx]);
    }

    all_nodes.insert(all_nodes.end(), pickup_nodes.begin(), pickup_nodes.end());
    all_nodes.insert(all_nodes.end(), delivery_nodes.begin(), delivery_nodes.end());
    all_nodes.insert(all_nodes.end(), transfer_nodes.begin(), transfer_nodes.end());
    all_nodes.insert(all_nodes.end(), depot_nodes.begin(), depot_nodes.end());
}

void preprocess(){
    for(size_t idx = 0; idx < request_amount; idx++){
        if(delivery_nodes[idx].lower_bound - pickup_nodes[idx].lower_bound - pickup_nodes[idx].service_time < arcs[pickup_nodes[idx].gen_idx][delivery_nodes[idx].gen_idx])
            cout << idx << '\n';
    }
}

void solve_instance(int ins){
    calculate_arcs();
    preprocess();
}

void solve_all_instances(){
    for(size_t idx = 0; idx < input_data.size(); idx++){
        create_instance(idx);
        solve_instance(idx);
    }
}

int main() {
	read_csv();

	bool eol = 0;
	while(!eol){ // Create instance based on user input
	    char response[20];
	    cout << "Which instances do you want to solve?\nType a number for a specific instance (starting with 0), type A for all instances, type B to abort.\n";
        cin >> response;
        switch(response[0])
        {
        case 'A':
            solve_all_instances();
            break;
        case 'B':
            eol = 1;
            break;
        default :
            if(isdigit(response[0])){
                if(atoi(response) >= input_data.size()){
                    cout << "Number too large, please try again\n";
                }else{
                    create_instance(atoi(response));
                    solve_instance(atoi(response));
                }
            }else{
            cout << "Not a number, please try again\n";
            }
        }
	}

	/*calculate_arcs();
	cout << pickup_nodes[0].x << '\n';
	double init_solution = create_init_solution();
	/*double best_solution = ALNS(init_solution);
	cout << best_solution;*/
	//write_output_file();
    return 0;
}
