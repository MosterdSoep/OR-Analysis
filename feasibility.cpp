#include "instance.h"
#include <algorithm>

bool Instance::is_feasible() {
	if (time_windows_met() &&
		capacity_feasible()){
		return true;
	} else {
		//if (!time_windows_met()) { cout << "Time windows and ride times incorrect!\n"; }
		//if (!capacity_feasible()) { cout << "Capacity infeasible!\n"; }
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
            //cout << "ride times infeasible\n";
            return false;
        }
        if(trans_pickup_time[idx] < trans_delivery_time[idx]){
            //cout << "transfers infeasible\n";
            return false;
        }
	}
	return true;
}

bool Instance::time_windows_met() {
	vector<double> pickup_time(request_amount, 0);
	vector<double> delivery_time(request_amount, 0);
	vector<double> trans_pickup_time(request_amount, numeric_limits<double>::max());
	vector<double> trans_delivery_time(request_amount, 0);

	//might want to change this, based on request data
    for (Vehicle v : routes) {
        for (size_t idx = 0; idx < v.route.size(); idx++){
			// Time windows
            if (idx != 0 && idx != v.route.size() - 1) {
				if (v.time_at_node[idx] + v.waiting_times[idx] > v.route[idx].upper_bound){
					//cout << "time windows upper bound infeasible\n";
					return false;
				} else if (v.time_at_node[idx] + v.waiting_times[idx] < v.route[idx].lower_bound - 0.00001){
					//cout << "time window lower bound infeasible\n";
					return false;
				}
			}
			// Maximum ride time
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
	// Maximum ride time
    for(size_t idx = 0; idx < request_amount; idx++){
        if(delivery_time[idx] - pickup_time[idx] > ride_times[idx]){
            return false;
        }
        if(trans_pickup_time[idx] < trans_delivery_time[idx]){
        //cout << "transfers infeasible\n";
            transfer_infeasible_count += 1;
            return false;
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

double Instance::pickup_feasible(Vehicle &v, size_t &p, size_t &request, Node &pick){
    if(p==1){
        assert(pick.type =='p' || pick.type =='t');
        if(pick.type == 'p'){
            if(arcs[nearest_depot_gen_idx_p[pick.gen_idx]][pick.gen_idx] < pick.lower_bound){
                return pick.lower_bound - arcs[nearest_depot_gen_idx_p[pick.gen_idx]][pick.gen_idx];
            }else{
                return 0;
            }
        }else if(pick.type == 't'){
            if(arcs[nearest_depot_gen_idx_t[pick.index]][pick.gen_idx] < pick.lower_bound){
                return pick.lower_bound - arcs[nearest_depot_gen_idx_t[pick.index]][pick.gen_idx];
            }else{
                return 0;
            }
        }
    }else{
        if(v.time_at_node[p-1] + v.route[p-1].service_time + v.waiting_times[p-1] + arcs[v.route[p-1].gen_idx][pick.gen_idx] > pick.upper_bound){
            return -1234567;
        }else{
            if(v.time_at_node[p-1] + v.route[p-1].service_time + v.waiting_times[p-1] + arcs[v.route[p-1].gen_idx][pick.gen_idx] < pick.lower_bound){
                return pick.lower_bound - v.time_at_node[p-1] - v.route[p-1].service_time - v.waiting_times[p-1] - arcs[v.route[p-1].gen_idx][pick.gen_idx];
            }else{
                return 0;
            }
        }
    }
    return -1234567;
}

double Instance::delivery_feasible(Vehicle &v, size_t p, size_t d, size_t request, double p_delay, double &added_time, Node &pick, Node &del){
    double arr = 0;
    double sum_wait = 0;
    if(p==d-1){
        if(p==1)
        {
            if(pick.type == 'p'){
                arr = arcs[nearest_depot_gen_idx_p[pick.index]][pick.gen_idx] + pick.service_time + p_delay + arcs[pick.gen_idx][del.gen_idx];
            }else if(pick.type == 't'){
                arr = arcs[nearest_depot_gen_idx_t[pick.index]][pick.gen_idx] + pick.service_time + p_delay + arcs[pick.gen_idx][del.gen_idx];
            }
        }else{
            arr = arcs[v.route[p-1].gen_idx][pick.gen_idx] + v.time_at_node[p-1] + v.route[p-1].service_time + v.waiting_times[p-1] +
                    pick.service_time + p_delay + arcs[pick.gen_idx][del.gen_idx];
        }
    }else{
        for(size_t idx = p; idx < d-1; idx++){
            sum_wait = v.waiting_times[idx];
        }
        if(sum_wait > p_delay + added_time){
            arr = v.time_at_node[d-2] + v.waiting_times[d-2] + v.route[d-2].service_time + arcs[v.route[d-2].gen_idx][del.gen_idx] + sum_wait;
            sum_wait = p_delay + added_time;
        }else{
            arr = v.time_at_node[d-2] + v.waiting_times[d-2] + v.route[d-2].service_time + arcs[v.route[d-2].gen_idx][del.gen_idx] + p_delay + added_time;
        }
    }

    if(arr < del.lower_bound){
        return del.lower_bound - arr - sum_wait;
    }else if (arr > del.upper_bound){
        return -1234567;
    }else{
        return -sum_wait;
    }
}

bool Instance::check_slack_times(Vehicle &v, size_t first, size_t last, double delay){
    for(size_t idx = first; idx < last; idx++){
        if(delay > v.slack_at_node[idx]){
            return false;
        }
        if(delay - v.waiting_times[idx] > 0){
            delay -= v.waiting_times[idx];
        }else{
            return true;
        }
    }
    return true;
}

bool Instance::insertion_ride_times_feasible(Vehicle &v, size_t p, double delay){ //delay is edited, dont pass by reference
    for(size_t idx = 1; idx < p; idx++){
        bool found = false;
        if(v.route[idx].type == 'p'){
            for(size_t adx = 2; adx < p; adx++){
                if((v.route[adx].type == 'd' || v.route[adx].type == 't')&& v.route[idx].request_idx == v.route[adx].request_idx){
                    found = true;
                    break;
                }
            }
        }
        if(found == false){
            size_t d_vehicle = delivery_vehicle[v.route[idx].request_idx];
            if(d_vehicle == v.v_index){
                for(size_t adx = p; adx < routes[d_vehicle].route.size(); adx++){
                    double delay_p = delay;
                    if(delay_p - v.waiting_times[adx] > 0){
                    delay_p -= v.waiting_times[adx];
                    }else{delay_p  = 0;}
                    if(routes[d_vehicle].route[adx].type == 'd'  && routes[d_vehicle].route[adx].request_idx == v.route[idx].request_idx){
                        if(v.time_at_node[adx] + v.waiting_times[adx] - v.time_at_node[idx] - v.waiting_times[idx] - v.route[idx].service_time + delay_p > ride_times[v.route[idx].request_idx]){
                            return false;
                        }else{
                            continue;
                        }
                    }
                }
            }else{/*
                for(size_t adx = 1; adx < routes[d_vehicle].route.size(); adx++){
                    if(routes[d_vehicle].route[adx].type == 'd'  && routes[d_vehicle].route[adx].request_idx == v.route[idx].request_idx){
                        continue;
                    }
                }*/
            }
        }
    }
    return true;
}

bool Instance::insertion_delivery_times_feasible(Vehicle &v, size_t p, size_t d, double delay_first, double delay_second){
    for(size_t idx = 1; idx < d-1; idx++){
        bool found = false;
        if(v.route[idx].type == 'p'){
            for(size_t adx = 2; adx < d-1; adx++){
                if((v.route[adx].type == 'd' || v.route[adx].type == 't')&& v.route[idx].request_idx == v.route[adx].request_idx){
                    found = true;
                    break;
                }
            }
        }
        if(found == false){
            size_t d_vehicle = delivery_vehicle[v.route[idx].request_idx];
            if(d_vehicle == v.v_index){
                for(size_t adx = d-1; adx < routes[d_vehicle].route.size(); adx++){
                    double delay_p = 0;
                    if(idx < p){  //ride starts before p, and ends after d-1
                        delay_p = delay_first;
                        for(size_t odx = p; odx < d-1; odx++){
                          delay_p -= v.waiting_times[odx];
                        }
                        delay_p = (delay_p < 0) ? 0 : delay_p;
                        double delay_d = delay_second;
                        if(delay_p + delay_d - v.waiting_times[adx] > 0){
                          delay_d -= v.waiting_times[adx];
                        }else{
                          delay_p  = 0;
                          delay_d = 0;
                        }
                        if(routes[d_vehicle].route[adx].type == 'd'  && routes[d_vehicle].route[adx].request_idx == v.route[idx].request_idx){
                          if(v.time_at_node[adx] + v.waiting_times[adx] - v.time_at_node[idx] - v.waiting_times[idx] - v.route[idx].service_time + delay_p + delay_d > ride_times[v.route[idx].request_idx]){
                            return false;
                          }else{
                            continue;
                          }
                        }
                    }else{  // ride starts between p and d-1, but ends after d-1
                        double sum_waiting = 0;
                        for(size_t odx = idx; odx < d - 1; odx++){
                          sum_waiting += v.waiting_times[odx];
                        }
                        delay_p = delay_first;
                        for(size_t odx = p; odx < idx; odx++){
                          delay_p -= v.waiting_times[odx];
                        }
                        delay_p = (delay_p < 0) ? 0 : delay_p;
                        sum_waiting = (delay_p < sum_waiting) ? delay_p : sum_waiting;
                        for(size_t odx = idx; odx < d-1; odx++){
                          delay_p -= v.waiting_times[odx];
                        }
                        delay_p = (delay_p < 0) ? 0 : delay_p;
                        double delay_d = delay_second;
                        if(delay_p + delay_d - v.waiting_times[adx] > 0){
                          delay_d -= v.waiting_times[adx];
                        }else{
                          delay_p  = 0;
                          delay_d = 0;
                        }

                        if(routes[d_vehicle].route[adx].type == 'd'  && routes[d_vehicle].route[adx].request_idx == v.route[idx].request_idx){
                          if(v.time_at_node[adx] + v.waiting_times[adx] - v.time_at_node[idx] - v.waiting_times[idx] - v.route[idx].service_time - sum_waiting + delay_p + delay_d > ride_times[v.route[idx].request_idx]){
                            return false;
                          }else{
                            continue;
                          }
                        }
                    }
                }
            }else{

            }
        }
    }
    return true;
}
