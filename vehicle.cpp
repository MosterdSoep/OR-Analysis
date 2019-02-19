#include "vehicle.h"

void Vehicle::remove_node(size_t location){
	if (route.size() == 1) }
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
			waiting_times.erase(route.begin());
			current_capacity.erase(route.begin());
			//time_at_node.erase(route.begin());
		} else if (location == route.size()) {
			route.erase(route.end());
			arc_durations.erase(arc_durations.end());
			waiting_times.erase(route.end());
			current_capacity.erase(route.end());
			//time_at_node.erase(route.end());
		} else {
			arc_durations.erase(arc_durations.begin() + location - 1);
			arc_durations[arc_durations.begin() + location] = get_arc(route[route.begin() + location - 1].gen_idx, route[route.begin() + location + 1].gen_idx);
			waiting_times.erase(waiting_times.begin() + location); // Waiting times not optimal, could possibly wait less in other nodes but that may change feasibility
			
			// Changing the capacity of the vehicle depends on the node that is being removed
			if (route[location].type == 'p' || (route[location].pickup && route[location].type == 't')) {
				// Pickup will be removed
				for (size_t index = location + 1; index < current_capacity.size(); index++) {
					current_capacity[index]--;
				}
			} else if (route[location].type == 'd' || (!route[location].pickup && route[location].type == 't')) {
				// Delivery will be removed
				for (size_t index = location + 1; index < current_capacity.size(); index++) {
					current_capacity[index]++;
				}
			} else {
				cout << "ERROR: Inserting non pickup, non delivery and non transfer node!\n";
			}
			route.erase(route.begin() + location);
		}
		// Calculating time_at_node
		for (size_t index = location + 1; index < time_at_node.size(); index++) {
			if (location == 0) {
				// time_at_node[index] is now the first node so there is no previous arrival time
				if (route[index].type == 'p' || route[index].type == 'd') {
					time_at_node[index] = 0 + waiting_times[index] + route[index].service_time;
				} else if (route[index].type == 't') {
					if (route[index].pickup && route[index].delivery) {
						time_at_node[index] = 0 + waiting_times[index] + route[index].service_time + route[index].service_time;
					} else if (route[index].pickup || route[index].delivery) {
						time_at_node[index] = 0 + waiting_times[index] + route[index].service_time;
					} else {
						cout << "ERROR: Non pickup and non delivery transfer node!\n";
					}
				}
			} else {
				if (route[index].type == 'p' || route[index].type == 'd') {
					time_at_node[index] = time_at_node[index - 1] + waiting_times[index] + route[index].service_time;
				} else if (route[index].type == 't') {
					if (route[index].pickup && route[index].delivery) {
						time_at_node[index] = time_at_node[index - 1] + waiting_times[index] + route[index].service_time + route[index].service_time;
					} else if (route[index].pickup || route[index].delivery) {
						time_at_node[index] = time_at_node[index - 1] + waiting_times[index] + route[index].service_time;
					} else {
						cout << "ERROR: Non pickup and non delivery transfer node!\n";
					}
				}
			}
		}
	}
};

// Add transfer node
void Vehicle::add_node(size_t location, Transfer_Node node){
	this->arc_durations.insert(arc_durations.begin() + location, arc1);
	this->arc_durations.insert(arc_durations.begin() + location + 1, arc2);
	this->route.insert(route.begin() + location, node);
	
	// Waiting times and current capacity may need to be changed according
	// to the specific node that is added
	this->waiting_times.insert(waiting_times.begin() + location, 2, waiting_times[location]);
	this->current_capacity.insert(current_capacity.begin() + location, 2, current_capacity[location]);
};

// Add pickup node
void Vehicle::add_node(size_t location, Pickup_Node node){
	this->arc_durations.insert(arc_durations.begin() + location, arc1);
	this->arc_durations.insert(arc_durations.begin() + location + 1, arc2);
	this->route.insert(route.begin() + location, node);
	
	// Waiting times and current capacity may need to be changed according
	// to the specific node that is added
	this->waiting_times.insert(waiting_times.begin() + location, 2, waiting_times[location]);
	this->current_capacity.insert(current_capacity.begin() + location, 2, current_capacity[location]);
};

// Add delivery node
void Vehicle::add_node(size_t location, Delivery_Node node){
	this->arc_durations.insert(arc_durations.begin() + location, arc1);
	this->arc_durations.insert(arc_durations.begin() + location + 1, arc2);
	this->route.insert(route.begin() + location, node);
	
	// Waiting times and current capacity may need to be changed according
	// to the specific node that is added
	this->waiting_times.insert(waiting_times.begin() + location, 2, waiting_times[location]);
	this->current_capacity.insert(current_capacity.begin() + location, 2, current_capacity[location]);
};

// Add depot node
void Vehicle::add_node(size_t location, Depot_Node node){
	this->arc_durations.insert(arc_durations.begin() + location, arc1);
	this->arc_durations.insert(arc_durations.begin() + location + 1, arc2);
	this->route.insert(route.begin() + location, node);
	
	// Waiting times and current capacity may need to be changed according
	// to the specific node that is added
	this->waiting_times.insert(waiting_times.begin() + location, 2, waiting_times[location]);
	this->current_capacity.insert(current_capacity.begin() + location, 2, current_capacity[location]);
};
