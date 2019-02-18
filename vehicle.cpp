#include "vehicle.h"

void Vehicle::remove_node(size_t location){
	if (location == 0) {
		// Removing starting depot
		this->arc_durations.erase(arc_durations.begin());
		this->route.erase(route.begin());
		this->waiting_times.erase(route.begin());
	} else if (location == this.route.size()) {
		// Removing ending depot
		this->arc_durations.erase(arc_durations.end());
		this->route.erase(route.end());
		this->waiting_times.erase(route.end());
	} else {
		// Removing pickup, delivery or transfer node
		this->arc_durations.erase(arc_durations.begin() + location - 1);
		this->arc_durations[arc_durations.begin() + location] = get_arc(route[route.begin() + location - 1].gen_idx,route[route.begin() + location + 1].gen_idx);
		this->route.erase(route.begin() + location);
		this->waiting_times.erase(route.begin() + location);
		
		// Changing of current_capacity
		// Dependent on the type of the node
		
	}
};

// Add transfer node
void Vehicle::add_node(size_t location, Transfer_Node node, double arc1, double arc2){
	this->arc_durations.insert(arc_durations.begin() + location, arc1);
	this->arc_durations.insert(arc_durations.begin() + location + 1, arc2);
	this->route.insert(route.begin() + location, node);
	
	// Waiting times and current capacity may need to be changed according
	// to the specific node that is added
	this->waiting_times.insert(waiting_times.begin() + location, 2, waiting_times[location]);
	this->current_capacity.insert(current_capacity.begin() + location, 2, current_capacity[location]);
};

// Add pickup node
void Vehicle::add_node(size_t location, Pickup_Node node, double arc1, double arc2){
	this->arc_durations.insert(arc_durations.begin() + location, arc1);
	this->arc_durations.insert(arc_durations.begin() + location + 1, arc2);
	this->route.insert(route.begin() + location, node);
	
	// Waiting times and current capacity may need to be changed according
	// to the specific node that is added
	this->waiting_times.insert(waiting_times.begin() + location, 2, waiting_times[location]);
	this->current_capacity.insert(current_capacity.begin() + location, 2, current_capacity[location]);
};

// Add delivery node
void Vehicle::add_node(size_t location, Delivery_Node node, double arc1, double arc2){
	this->arc_durations.insert(arc_durations.begin() + location, arc1);
	this->arc_durations.insert(arc_durations.begin() + location + 1, arc2);
	this->route.insert(route.begin() + location, node);
	
	// Waiting times and current capacity may need to be changed according
	// to the specific node that is added
	this->waiting_times.insert(waiting_times.begin() + location, 2, waiting_times[location]);
	this->current_capacity.insert(current_capacity.begin() + location, 2, current_capacity[location]);
};

// Add depot node
void Vehicle::add_node(size_t location, Depot_Node node, double arc1, double arc2){
	this->arc_durations.insert(arc_durations.begin() + location, arc1);
	this->arc_durations.insert(arc_durations.begin() + location + 1, arc2);
	this->route.insert(route.begin() + location, node);
	
	// Waiting times and current capacity may need to be changed according
	// to the specific node that is added
	this->waiting_times.insert(waiting_times.begin() + location, 2, waiting_times[location]);
	this->current_capacity.insert(current_capacity.begin() + location, 2, current_capacity[location]);
};
