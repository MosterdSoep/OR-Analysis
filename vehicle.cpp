#include "vehicle.h"
#include "instance.h"
#include <algorithm>

#include <limits>

using namespace std;

//defined here so vehicles can reach this global variable
vector<vector<double>> arcs = {};
vector<Pickup_Node> pickup_nodes = {};
vector<Delivery_Node> delivery_nodes = {};
vector<Transfer_Node> transfer_nodes = {};
vector<Depot_Node> depot_nodes = {};
vector<Node> all_nodes = {};
vector<size_t> pickup_vehicle = {};
vector<size_t> delivery_vehicle = {};
vector<double> ride_times = {};
size_t vehicle_capacity = 0;
vector<size_t> nearest_depot_gen_idx_p = {};
vector<size_t> nearest_depot_gen_idx_d = {};
vector<size_t> nearest_depot_gen_idx_t = {};
vector<double> nearest_depot_insertion_cost = {};

Vehicle::Vehicle(){ //vehicles get initialized with a random depot node
    route.push_back(depot_nodes[0]);
    route.push_back(depot_nodes[0]);
}

bool Vehicle::maximum_ride_time_correct() {
	vector<pair <size_t, double>> pickup_times = {};
	vector<pair <size_t, double>> delivery_times = {};

	for (size_t i = 1; i < route.size() - 1; i++) {
		if (route[i].type == 'p' || (route[i].type == 't' && route[i].pickup)) {
			pair<size_t,double> p (route[i].request_idx, time_at_node[i]);
			pickup_times.push_back(p);
		} else if (route[i].type == 'd' || (route[i].type == 't' && route[i].delivery)) {
			pair<size_t,double> d (route[i].request_idx, time_at_node[i]);
			delivery_times.push_back(d);
		}
	}

	for (pair <size_t, double> p : pickup_times) {
		for (pair <size_t, double> d : delivery_times) {
			if (p.first == d.first) {
				if (d.second - p.second > ride_times[p.first]) {
					// Infeasible
					return false;
				}
			}
		}
	}
	return true;
}

bool Vehicle::time_windows_correct() {
	for (size_t idx = 1; idx < route.size() - 1; idx++){
		if (time_at_node[idx] + waiting_times[idx] > route[idx].upper_bound){
			return false;
		} else if (time_at_node[idx] + waiting_times[idx] < route[idx].lower_bound - 0.00001){
			return false;
		}
    }
	return true;
}

bool Vehicle::current_capacity_correct() {
	if (*max_element(current_capacity.begin(), current_capacity.end()) > vehicle_capacity) {
		return false;
    }
	return true;
}

void Vehicle::change_first_depot(){ //automatically change first depot to optimal one
    route.erase(route.begin());
    size_t min_idx = 0;
    double min_val = numeric_limits<double>::max();
    for(size_t idx = 0; idx < depot_nodes.size(); idx++){
        if(min_val > arcs[depot_nodes[idx].gen_idx][route[0].gen_idx]){
            min_val = arcs[depot_nodes[idx].gen_idx][route[0].gen_idx];
            min_idx = idx;
        }
    }
    route.insert(route.begin(), depot_nodes[min_idx]);
}

void Vehicle::change_last_depot(){ //automatically changes last depot to optimal one
    route.erase(route.end());
    size_t min_idx = 0;
    double min_val = numeric_limits<double>::max();
    for(size_t idx = 0; idx < depot_nodes.size(); idx++){
        if(min_val > arcs[route[route.size() - 1].gen_idx][depot_nodes[idx].gen_idx]){
            min_val = arcs[route[route.size() - 1].gen_idx][depot_nodes[idx].gen_idx];
            min_idx = idx;
        }
    }
    route.insert(route.end(), depot_nodes[min_idx]);
}

void Vehicle::add_node(size_t location, Pickup_Node &node){
    route.insert(route.begin() + location, node);
    if (location == 1){
        change_first_depot();
    }
    if (location == route.size() - 2){
        change_last_depot();
    }

    current_capacity.insert(current_capacity.begin() + location, current_capacity[location - 1]);
    waiting_times.insert(waiting_times.begin() + location, 0);
    time_at_node.insert(time_at_node.begin() + location, 0);
    slack_at_node.insert(slack_at_node.begin() + location, 0);
    for(size_t idx = location; idx < route.size(); idx++){
        current_capacity[idx]++;
        time_at_node[idx] = time_at_node[idx - 1]
							+ arcs[route[idx-1].gen_idx][route[idx].gen_idx]
							+ route[idx - 1].service_time
							+ waiting_times[idx - 1];
		if (time_at_node[idx] < route[idx].lower_bound) {
            waiting_times[idx] = route[idx].lower_bound - time_at_node[idx];
        }else{
        waiting_times[idx]  = 0;
        }
        slack_at_node[idx] = route[idx].upper_bound - time_at_node[idx];
    }
	pickup_vehicle[node.index] = v_index;
}

void Vehicle::add_node(size_t location, Delivery_Node &node){
    route.insert(route.begin() + location, node);
    if (location == 1){
        change_first_depot();
    }
    if (location == route.size() - 2){
        change_last_depot();
    }

    current_capacity.insert(current_capacity.begin() + location, current_capacity[location - 1]);
    waiting_times.insert(waiting_times.begin() + location, 0);
    time_at_node.insert(time_at_node.begin() + location, 0);
    slack_at_node.insert(slack_at_node.begin() + location, 0);
    for(size_t idx = location; idx < route.size(); idx++){
        current_capacity[idx]--;
        time_at_node[idx] = time_at_node[idx - 1]
							+ arcs[route[idx-1].gen_idx][route[idx].gen_idx]
							+ route[idx - 1].service_time
							+ waiting_times[idx - 1];
        if (time_at_node[idx] < route[idx].lower_bound) {
            waiting_times[idx] = route[idx].lower_bound - time_at_node[idx];
        }else{
        waiting_times[idx]  = 0;
        }
        slack_at_node[idx] = route[idx].upper_bound - time_at_node[idx];
    }
	delivery_vehicle[node.index] = v_index;
}

double Vehicle::add_delivery_transfer(size_t location, Transfer_Node &node, size_t r){
    //Creates fictional new node
    Transfer_Node fictional_node = node;
    fictional_node.pickup = 0;
    fictional_node.request_idx = r;
	fictional_node.gen_idx = node.gen_idx;

    route.insert(route.begin() + location, fictional_node);

    if (location == 1){
        change_first_depot();
    }
    if (location == route.size() - 2){
        change_last_depot();
    }

    current_capacity.insert(current_capacity.begin() + location, current_capacity[location - 1]);
    waiting_times.insert(waiting_times.begin() + location, 0);
    time_at_node.insert(time_at_node.begin() + location, 0);
    slack_at_node.insert(slack_at_node.begin() + location, 0);
    for(size_t idx = location; idx < route.size(); idx++){
        current_capacity[idx]--;
        time_at_node[idx] = time_at_node[idx - 1]
							+ arcs[route[idx-1].gen_idx][route[idx].gen_idx]
							+ route[idx - 1].service_time
							+ waiting_times[idx - 1];
        if(time_at_node[idx] < route[idx].lower_bound){
            waiting_times[idx] = route[idx].lower_bound - time_at_node[idx];
        }else{
        waiting_times[idx]  = 0;
        }
        slack_at_node[idx] = route[idx].upper_bound - time_at_node[idx];
    }

	// Returns the time at which pickup can start
    return time_at_node[location] + node.service_time;
}

void Vehicle::add_pickup_transfer(size_t location, Transfer_Node &node, double min_time, size_t r){
    //Creates fictional new node
    Transfer_Node fictional_node = node;
    fictional_node.pickup = 1;
    fictional_node.request_idx = r;
    fictional_node.lower_bound = min_time; //update the lower bound for pickup to the time at which the transfer is ready
	fictional_node.gen_idx = node.gen_idx;

    route.insert(route.begin() + location, fictional_node);

    if (location == 1){
        change_first_depot();
    }
    if (location == route.size() - 2){
        change_last_depot();
    }

    current_capacity.insert(current_capacity.begin() + location, current_capacity[location - 1]);
    waiting_times.insert(waiting_times.begin() + location, 0);
    time_at_node.insert(time_at_node.begin() + location, 0);
    slack_at_node.insert(slack_at_node.begin() + location, 0);
    for(size_t idx = location; idx < route.size(); idx++){
        current_capacity[idx]++;
        time_at_node[idx] = time_at_node[idx - 1]
							+ arcs[route[idx-1].gen_idx][route[idx].gen_idx]
							+ route[idx - 1].service_time
							+ waiting_times[idx - 1];
        if(time_at_node[idx] < route[idx].lower_bound){
            waiting_times[idx] = route[idx].lower_bound - time_at_node[idx];
        }else{
        waiting_times[idx]  = 0;
        }
        slack_at_node[idx] = route[idx].upper_bound - time_at_node[idx];
    }
    route[location].lower_bound = 0;
}

void Vehicle::remove_node(size_t location){
	waiting_times.erase(waiting_times.begin() + location); // Waiting times not optimal, could possibly wait less in other nodes but that may change feasibility

	// Changing the capacity of the vehicle depends on the node that is being removed
	if (route[location].type == 'p' || (route[location].pickup && route[location].type == 't')) {
		// Pickup will be removed
        for (size_t i = location + 1; i < current_capacity.size(); i++) {
			current_capacity[i]--;
		}
    } else if (route[location].type == 'd' || (!route[location].pickup && route[location].type == 't')) {
		// Delivery will be removed
		for (size_t i = location + 1; i < current_capacity.size(); i++) {
			current_capacity[i]++;
		}
	} else if (route[location].type == 'o') {
        cout << "ERROR: Removing depot node!\n";
	} else {
		cout << "ERROR: Removing non pickup, non delivery and non transfer node!\n";
	}
	route.erase(route.begin() + location);

    if (location == 1){
        change_first_depot();
    }
    if (location == route.size() - 2){
        change_last_depot();
    }

    for(size_t idx = location; idx < route.size(); idx++){
        time_at_node[idx] = time_at_node[idx - 1]
							+ arcs[route[idx-1].gen_idx][route[idx].gen_idx]
							+ route[idx - 1].service_time
							+ waiting_times[idx - 1];
        if(time_at_node[idx] < route[idx].lower_bound){
            waiting_times[idx] = route[idx].lower_bound - time_at_node[idx];
        }else{
            waiting_times[idx] = 0;
        }
        slack_at_node[idx] = route[idx].upper_bound - time_at_node[idx];
    }
};
