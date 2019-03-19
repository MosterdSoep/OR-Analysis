#include "instance.h"
#include <algorithm>

bool Instance::is_feasible() {
	if (maximum_ride_time_not_exceeded() &&
		time_windows_met() &&
		capacity_feasible()){
		return true;
	} else {
	    /*
		if (!maximum_ride_time_not_exceeded()) { cout << "Maximum ride time exceeded\n";  }
		if (!time_windows_met()) { cout << "Time windows not met\n";}
		if (!capacity_feasible()) { cout << "Capacity not feasible\n";  }
		cout << "\n";*/
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
                cout << "upper bound infeasible, type : " << v.route[idx].type << "  request : " << v.route[idx].index << '\n';
                return false;
            } else if (v.time_at_node[idx] + v.waiting_times[idx] < v.route[idx].lower_bound - 0.00001){
                cout << "lower bound infeasible, type : "  << v.route[idx].type << "  request : " << v.route[idx].index << "\n";
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

double Instance::pickup_feasible(Vehicle v, size_t p, size_t request){
    if(p==1){
        if(arcs[nearest_depot_gen_idx_p[request]][request] < pickup_nodes[request].lower_bound){
            return pickup_nodes[request].lower_bound - arcs[nearest_depot_gen_idx_p[request]][request];
        }else{
            return 0;
        }
    }else{
        if(v.time_at_node[p-1] + v.route[p-1].service_time + v.waiting_times[p-1] + arcs[v.route[p-1].gen_idx][request] > pickup_nodes[request].upper_bound){
            return -1234567;
        }else{
            if(v.time_at_node[p-1] + v.route[p-1].service_time + v.waiting_times[p-1] + arcs[v.route[p-1].gen_idx][request] < pickup_nodes[request].lower_bound){
                return pickup_nodes[request].lower_bound - v.time_at_node[p-1] - v.route[p-1].service_time - v.waiting_times[p-1] - arcs[v.route[p-1].gen_idx][request];
            }else{
                return 0;
            }
        }
    }
}

double Instance::delivery_feasible(Vehicle v, size_t p, size_t d, size_t request, double p_delay){
    double arr = 0;
    double sum_wait = 0;
    if(p==d-1){
        if(p==1)
        {
            arr = arcs[nearest_depot_gen_idx_p[request]][request] + pickup_nodes[request].service_time + p_delay + arcs[request][delivery_nodes[request].gen_idx];
        }else{
            arr = arcs[v.route[p-1].gen_idx][request] + v.time_at_node[p-1] + v.route[p-1].service_time + v.waiting_times[p-1] +
                    pickup_nodes[request].service_time + p_delay + arcs[request][delivery_nodes[request].gen_idx];
        }
    }else{
        for(size_t idx = p; idx < d-1; idx++){
            sum_wait = v.waiting_times[idx];
        }
        if(sum_wait > p_delay){
            arr = v.time_at_node[d-2] + v.waiting_times[d-2] + v.route[d-2].service_time + arcs[v.route[d-2].gen_idx][delivery_nodes[request].gen_idx] + sum_wait;
            sum_wait = p_delay;
        }else{
            arr = v.time_at_node[d-2] + v.waiting_times[d-2] + v.route[d-2].service_time + arcs[v.route[d-2].gen_idx][delivery_nodes[request].gen_idx] + p_delay;
        }
    }

    if(arr < delivery_nodes[request].lower_bound){
        return delivery_nodes[request].lower_bound - arr - sum_wait;
    }else if (arr > delivery_nodes[request].upper_bound){
        return -1234567;
    }else{
        return -sum_wait;
    }
}
