#include "vehicle.h"
#include "instance.h"

using namespace std;
vector<vector<double>> arcs = {};

void Vehicle::remove_node(size_t location){
	if (route.size() == 1) {
		// Only 1 node in the route hence the route is cleared
		route.clear();
		arc_durations.clear();
		waiting_times.clear();
		current_capacity.clear();
		time_at_node.clear();
	} else {
		if (location == 0) {
			route.erase(route.begin());
			arc_durations.erase(arc_durations.begin());
			waiting_times.erase(waiting_times.begin());
			current_capacity.erase(current_capacity.begin());
			//time_at_node.erase(route.begin());
		} else if (location == route.size() - 1) {
			route.erase(route.end());
			arc_durations.erase(arc_durations.end());
			waiting_times.erase(waiting_times.end());
			current_capacity.erase(current_capacity.end());
			//time_at_node.erase(route.end());
		} else {
			arc_durations.erase(arc_durations.begin() + location - 1);
			arc_durations[location] = arcs[route[location - 1].gen_idx][route[location + 1].gen_idx];
			waiting_times.erase(waiting_times.begin() + location); // Waiting times not optimal, could possibly wait less in other nodes but that may change feasibility
			
			// Changing the capacity of the vehicle depends on the node that is being removed
			if (route[location].type == 'p') {
				// Pickup will be removed
				for (size_t i = location + 1; i < current_capacity.size(); i++) {
					current_capacity[i]++;
				}
			} else if (route[location].type == 'd' || (!route[location].pickup && route[location].type == 't')) {
				// Delivery will be removed
				for (size_t i = location + 1; i < current_capacity.size(); i++) {
					current_capacity[i]--;
				}
			} else if (route[location].pickup && route[location].type == 't'){
			        for (size_t i = location + 1; i < current_capacity.size(); i++) {
					current_capacity[i]++;
				}
			}
			else {
				cout << "ERROR: Inserting non pickup, non delivery and non transfer node!\n";
			}
			route.erase(route.begin() + location);
		}
		
		
		// Calculating time_at_node
		for (size_t i = location + 1; i < time_at_node.size(); i++) {
			if (location == 0) {
				// time_at_node[i] is now the first node so there is no previous arrival time
				time_at_node[i] = 0 + arc_durations[i - 1];
			} else {
				time_at_node[i] = time_at_node[i - 1] + arc_durations[i - 1];
			}
		}
	}
};

// Add transfer node
void Vehicle::add_node(size_t location, Transfer_Node node){
	if (location == 0) {
		arc_durations.insert(arc_durations.begin(), arcs[node.gen_idx][route[0].gen_idx]);
		waiting_times.insert(waiting_times.begin(), 0);
		current_capacity.insert(current_capacity.begin(), current_capacity[0]);
		route.insert(route.begin(), node);
	} else if (location == route.size() - 1) {
		arc_durations.insert(arc_durations.end(), arcs[node.gen_idx][route[route.size() - 1].gen_idx]);
		waiting_times.insert(waiting_times.end(), 0);
		
		// Capacity depends on what happens at the transfer facility
		if (node.pickup && node.delivery) {
			current_capacity.insert(current_capacity.begin(), current_capacity[route.size() - 1]);
		} else if (node.pickup) {
			current_capacity.insert(current_capacity.begin(), current_capacity[route.size()] + 1);
		} else if (node.delivery) {
			current_capacity.insert(current_capacity.begin(), current_capacity[route.size()] - 1);
		} else {
			cout << "ERROR: Non pickup, non delivery transfer node!\n";
		}
		route.insert(route.end(), node);
	} else {
		arc_durations.erase(arc_durations.begin() + location - 1);
		arc_durations.insert(arc_durations.begin() + location - 1, arcs[route[location - 1].gen_idx][node.gen_idx]);
		arc_durations.insert(arc_durations.begin() + location, arcs[node.gen_idx][route[location].gen_idx]);
		waiting_times.insert(waiting_times.begin() + location, 0);
		current_capacity.insert(current_capacity.begin() + location, current_capacity[location - 1] + 1);
		for (size_t i = location + 1; i < current_capacity.size(); i++) {
			current_capacity[i]++;
		}
		route.insert(route.begin() + location, node);
	}
};

// Add pickup node
void Vehicle::add_node(size_t location, Pickup_Node node){
	if (location == 0) {
		arc_durations.insert(arc_durations.begin(), arcs[node.gen_idx][route[0].gen_idx]);
		waiting_times.insert(waiting_times.begin(), 0);
		current_capacity.insert(current_capacity.begin(), current_capacity[0]);
		route.insert(route.begin(), node);
	} else if (location == route.size()) {
		arc_durations.insert(arc_durations.end(), arcs[node.gen_idx][route[route.size()].gen_idx]);
		waiting_times.insert(waiting_times.end(), 0);
		current_capacity.insert(current_capacity.begin(), current_capacity[route.size()] + 1);
		route.insert(route.end(), node);
	} else {
		arc_durations.erase(arc_durations.begin() + location - 1);
		arc_durations.insert(arc_durations.begin() + location - 1, arcs[route[location - 1].gen_idx][node.gen_idx]);
		arc_durations.insert(arc_durations.begin() + location, arcs[node.gen_idx][route[location].gen_idx]);
		waiting_times.insert(waiting_times.begin() + location, 0);
		current_capacity.insert(current_capacity.begin() + location, current_capacity[location - 1] + 1);
		for (size_t i = location + 1; i < current_capacity.size(); i++) {
			current_capacity[i]++;
		}
		route.insert(route.begin() + location, node);
	}
};

// Add delivery node
void Vehicle::add_node(size_t location, Delivery_Node node){
	if (location == 0) {
		arc_durations.insert(arc_durations.begin(), arcs[node.gen_idx][route[0].gen_idx]);
		waiting_times.insert(waiting_times.begin(), 0);
		current_capacity.insert(current_capacity.begin(), current_capacity[0]);
		route.insert(route.begin(), node);
	} else if (location == route.size()) {
		arc_durations.insert(arc_durations.end(), arcs[node.gen_idx][route[route.size()-1].gen_idx]);
		waiting_times.insert(waiting_times.end(), 0);
		current_capacity.insert(current_capacity.begin(), current_capacity[route.size()] - 1);
		route.insert(route.end(), node);
	} else {
		arc_durations.erase(arc_durations.begin() + location - 1);
		arc_durations.insert(arc_durations.begin() + location - 1, arcs[route[location].gen_idx][node.gen_idx]);
		arc_durations.insert(arc_durations.begin() + location, arcs[node.gen_idx][route[location -1].gen_idx]);
		waiting_times.insert(waiting_times.begin() + location, 0);
		current_capacity.insert(current_capacity.begin() + location, current_capacity[location -1] - 1);
		for (size_t i = location + 1; i < current_capacity.size(); i++) {
			current_capacity[i]--;
		}
		route.insert(route.begin() + location, node);
	}
};

// Add depot node
void Vehicle::add_node(size_t location, Depot_Node node){
	this->arc_durations.insert(arc_durations.begin() + location, arcs[node.gen_idx][route[location -1].gen_idx] );
	this->arc_durations.insert(arc_durations.begin() + location + 1, arcs[route[location -1].gen_idx][node.gen_idx]);
	this->route.insert(route.begin() + location, node);
	
	// Waiting times and current capacity may need to be changed according
	// to the specific node that is added
	this->waiting_times.insert(waiting_times.begin() + location, 2, waiting_times[location]);
	this->current_capacity.insert(current_capacity.begin() + location, 2, current_capacity[location]);
};
