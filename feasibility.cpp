#include "instance.h"

bool Instance::is_feasible() {
	// Things to check:
	// 1. Request related:
	//  - Request bank empty
	// 	- Maximum ride time not exceeded
	//  - Pickup and delivery time windows met
	
	// 2. Vehicle/route related:
	//	- Capacity constraint
	//  - Beginning and ending at a depot
	
	if (is_request_bank_empty() &&
		maximum_ride_time_not_exceeded() &&
		time_windows_met() &&
		capacity_feasible() &&
		correct_routes()) {
		return true;
	} else {
		return false;
	}
}

bool Instance::is_request_bank_empty() {
	// Do we check this or do we make sure the heuristic clears the request bank?
	return true;
}

bool Instance::maximum_ride_time_not_exceeded() {
	double pickup_time = 0;
	double delivery_time = 0;
	double transfer_delivery_time = 0;
	double transfer_pickup_time = 0;
	bool transfered = false;
	for (size_t req_idx = 0; idx < request_amount; req_idx++) {
		for (Vehicle v : routes) {
			for (Node n : v.route) {
				if (n.index == req_idx) {
					if (n.type == 'p') {
						pickup_time = v.time_at_node[];
					} else if (n.type == 'd') {
						delivery_time = v.time_at_node[];
					} else if (n.type == 't') {
						
					}
				}
			}
		}
	}
}

bool Instance::time_windows_met() {
	
	
}

bool Instance::capacity_feasible() {
	// Loop over every vehicle and see what the capacity is at the node
	for (Vehicle v : routes) {
		for (size_t c : v.current_capacity) {
			if (c > vehicle_capacity) return false;
		}
	}
	return true;
}

bool Instance::correct_routes() {
	// Check whether routes start and end at a depot
	
}