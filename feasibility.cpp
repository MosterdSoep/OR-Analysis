#include "instance.h"
#include <algorithm>

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
		capacity_feasible()){
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
	vector<double> pickup_time(request_amount, 0);
	vector<double> delivery_time(request_amount, 0);
	vector<double> trans_pickup_time(request_amount, numeric_limits<double>::max());
	vector<double> trans_delivery_time(request_amount, 0);


	//might want to change this, based on request data
	bool transfered = false;
    for (Vehicle v : routes) {
		for (size_t idx = 0; idx < v.route.size(); idx++){
            if (v.route[idx].type == 'p') {
				pickup_time[v.route[idx].index] = v.time_at_node[idx] + v.route[idx].service_time + v.waiting_times[idx];
			} else if (v.route[idx].type == 'd') {
				delivery_time[v.route[idx].index] = v.time_at_node[idx] + v.waiting_times[idx];
			} else if (v.route[idx].type == 't' && v.route[idx].pickup == 1){
                trans_pickup_time[v.route[idx].request_idx] = v.time_at_node[idx] + v.waiting_times[idx];
			} else if (v.route[idx].type == 't' && v.route[idx].pickup == 0){
                trans_delivery_time[v.route[idx].request_idx] = v.time_at_node[idx] + v.waiting_times[idx] + v.route[idx].service_time;
            }
		}
	}
    for(size_t idx = 0; idx < request_amount; idx++){
        if(delivery_time[idx] - pickup_time[idx] > ride_times[idx]){
            return false;
        }
        if(trans_pickup_time[idx] < trans_delivery_time[idx]){
            return false;
        }
	}
	return true;
}

bool Instance::time_windows_met() {
    for (Vehicle v : routes) {
        for (size_t idx = 1; idx < v.route.size() - 1; idx++){
            if (v.time_at_node[idx] + v.waiting_times[idx] > v.route[idx].upper_bound){
                return false;
            } else if (v.time_at_node[idx] + v.waiting_times[idx] < v.route[idx].lower_bound - 0.00001){
                return false;
            }
        }
    }
	return true;
}

bool Instance::capacity_feasible() {
	// Loop over every vehicle and see what the capacity is at the node
	for (Vehicle v : routes) {
		if (*max_element(v.current_capacity.begin(), v.current_capacity.end()) > vehicle_capacity)
        {
            return false;
        }
	}
	return true;
}

