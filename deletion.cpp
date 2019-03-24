#include "instance.h"
#include <algorithm>

size_t Instance::greedy_request_deletion(vector<size_t> request_bank) {
	double best_costs_saving = numeric_limits<double>::max();
	size_t best_request = 0;
	for (size_t i = 0; i < request_amount; i++) {
	    if (find(request_bank.begin(), request_bank.end(), i) != request_bank.end()) {

	    } else {
            double candidate = costs_of_removing_request(i);
            if (candidate < best_costs_saving) {
                best_costs_saving = candidate;
                best_request = i;
            }
	    }
	}
	// Found the request to be removed, now remove it
	if (pickup_vehicle[best_request] != delivery_vehicle[best_request]) {
		// Transferred request
		for (size_t i = 1; i < routes[pickup_vehicle[best_request]].route.size() - 1; i++) {
			if (routes[pickup_vehicle[best_request]].route[i].request_idx == best_request) {
				routes[pickup_vehicle[best_request]].remove_node(i);
			}
		}
		for (size_t i = 1; i < routes[delivery_vehicle[best_request]].route.size() - 1; i++) {
			if (routes[delivery_vehicle[best_request]].route[i].request_idx == best_request) {
				routes[delivery_vehicle[best_request]].remove_node(i);
			}
		}
	} else {
		// Non transfer request
		for (size_t i = 1; i < routes[pickup_vehicle[best_request]].route.size() - 1; i++) {
			if (routes[pickup_vehicle[best_request]].route[i].index == best_request) {
				routes[pickup_vehicle[best_request]].remove_node(i);
				i--;
			}
		}
	}
	return best_request;
}

size_t Instance::random_request_deletion(vector<size_t> request_bank) {
	size_t request = rand() % (request_amount);
	if(!request_bank.empty()){
        while (find(request_bank.begin(), request_bank.end(), request) != request_bank.end()) {
            request = rand() % (request_amount);
        }
	}
	if (pickup_vehicle[request] != delivery_vehicle[request]) {
		// Transferred request
		for (size_t i = 1; i < routes[pickup_vehicle[request]].route.size() - 1; i++) {
			if (routes[pickup_vehicle[request]].route[i].request_idx == request) {
				routes[pickup_vehicle[request]].remove_node(i);
			}
		}
		for (size_t i = 1; i < routes[delivery_vehicle[request]].route.size() - 1; i++) {
			if (routes[delivery_vehicle[request]].route[i].request_idx == request) {
				routes[delivery_vehicle[request]].remove_node(i);
			}
		}
	} else {
		// Non transfer request
		for (size_t i = 1; i < routes[pickup_vehicle[request]].route.size() - 1; i++) {
			if (routes[pickup_vehicle[request]].route[i].index == request) {
				routes[pickup_vehicle[request]].remove_node(i);
				i--;
			}
		}
	}
	return request;
}

bool compareFunc(pair<size_t, double> &a, pair<size_t, double> &b)
{
    return a.second > b.second;
}

vector<size_t> Instance::shaw_deletion(size_t amount){
    vector<size_t> request_bank = {};
    request_bank.push_back(rand() % request_amount);
    vector<pair<size_t, double>> relatedness(request_amount, {0,0});

    double alpha = 1, beta = 1;
    double tmp = 0, tmp_d = 0;
    double time_at_node_p = 0, time_at_node_d = 0;

    //determine arrival times for the initial request
    size_t v = pickup_vehicle[request_bank[0]];
    for(size_t adx = 1; adx < routes[v].route.size()-1; adx++){
        if(routes[v].route[adx].request_idx == request_bank[0]){
            time_at_node_p = routes[v].time_at_node[adx];
            adx = routes[v].route.size();
        }
    }

    v = delivery_vehicle[request_bank[0]];
    for(size_t adx = 1; adx < routes[v].route.size()-1; adx++){
        if(routes[v].route[adx].request_idx == request_bank[0]){
            time_at_node_d = routes[v].time_at_node[adx];
        }
    }
    //determine arrival times for all other requests
    for(size_t idx = 0; idx < request_amount; idx++){
        relatedness[idx].first = idx;

        v = pickup_vehicle[idx];
        for(size_t adx = 1; adx < routes[v].route.size()-1; adx++){
            if(routes[v].route[adx].request_idx == idx){
                tmp = routes[v].time_at_node[adx];
                adx = routes[v].route.size();
            }
        }

        v = delivery_vehicle[idx];
        for(size_t adx = 1; adx < routes[v].route.size()-1; adx++){
            if(routes[v].route[adx].request_idx == idx){
                tmp_d = routes[v].time_at_node[adx];
            }
        }
        //calculate relatedness measure
        //relatedness[idx].second = alpha * (arcs[request_bank[0]][idx] + arcs[delivery_nodes[request_bank[0]].gen_idx][delivery_nodes[idx].gen_idx]);
        relatedness[idx].second = alpha * (arcs[request_bank[0]][idx] + arcs[delivery_nodes[request_bank[0]].gen_idx][delivery_nodes[idx].gen_idx]) +
                beta * ( abs(time_at_node_p - tmp) + abs(time_at_node_d - tmp_d) );
    }
    sort(relatedness.begin(), relatedness.end(), compareFunc);

    for(size_t idx = 1; idx < amount; idx++){
        request_bank.push_back(relatedness[idx].first);
    }

    for(size_t idx = 0; idx < request_bank.size(); idx++){
    if (pickup_vehicle[request_bank[idx]] != delivery_vehicle[request_bank[idx]]) {
		// Transferred request
		for (size_t i = 1; i < routes[pickup_vehicle[request_bank[idx]]].route.size() - 1; i++) {
			if (routes[pickup_vehicle[request_bank[idx]]].route[i].request_idx == request_bank[idx]) {
				routes[pickup_vehicle[request_bank[idx]]].remove_node(i);
			}
		}
		for (size_t i = 1; i < routes[delivery_vehicle[request_bank[idx]]].route.size() - 1; i++) {
			if (routes[delivery_vehicle[request_bank[idx]]].route[i].request_idx == request_bank[idx]) {
				routes[delivery_vehicle[request_bank[idx]]].remove_node(i);
			}
		}
	} else {
		// Non transfer request
		for (size_t i = 1; i < routes[pickup_vehicle[request_bank[idx]]].route.size() - 1; i++) {
			if (routes[pickup_vehicle[request_bank[idx]]].route[i].index == request_bank[idx]) {
				routes[pickup_vehicle[request_bank[idx]]].remove_node(i);
				i--;
			}
		}
	}
    }
    relatedness.clear();
    return request_bank;
}

void Instance::greedy_route_deletion(vector<size_t> request_bank) {

}

void Instance::random_route_deletion(vector<size_t> request_bank) {

}

double Instance::costs_of_removing_request(size_t request) {
	size_t pu_vehicle = pickup_vehicle[request];
	size_t de_vehicle = delivery_vehicle[request];
	double arc_lengths = 0.0;

	int p = -1, d = -1, td = -1, tp = -1;
	if (pu_vehicle == de_vehicle) {
		for (size_t i = 1; i < routes[pu_vehicle].route.size() - 1; i++) {
			if (routes[pu_vehicle].route[i].index == request) {
				if (p == -1) p = i;
				else d = i;
			}
		}
	} else {
		for (size_t i = 1; i < routes[pu_vehicle].route.size() - 1; i++) {
			if (routes[pu_vehicle].route[i].index == request) {
				if (p == -1) p = i;
				else td = i;
			}
		}
		for (size_t i = 1; i < routes[de_vehicle].route.size() - 1; i++) {
			if (routes[de_vehicle].route[i].index == request) {
				if (tp == -1) tp = i;
				else d = i;
			}
		}
	}

	if (pu_vehicle != de_vehicle) {
		// Pickup change in arcs
		if (td == p + 1) {
			arc_lengths = arc_lengths - arcs[routes[pu_vehicle].route[p].gen_idx][routes[pu_vehicle].route[td].gen_idx]
							- arcs[routes[pu_vehicle].route[p-1].gen_idx][routes[pu_vehicle].route[p].gen_idx]
							- arcs[routes[pu_vehicle].route[td].gen_idx][routes[pu_vehicle].route[td-1].gen_idx];
		} else {
			arc_lengths = arc_lengths - arcs[routes[pu_vehicle].route[p].gen_idx][routes[pu_vehicle].route[p+1].gen_idx]
							- arcs[routes[pu_vehicle].route[td-1].gen_idx][routes[pu_vehicle].route[td].gen_idx]
							- arcs[routes[pu_vehicle].route[p-1].gen_idx][routes[pu_vehicle].route[p].gen_idx]
							- arcs[routes[pu_vehicle].route[td].gen_idx][routes[pu_vehicle].route[td+1].gen_idx];
		}
		if (p > 1 && td < routes[pu_vehicle].route.size() - 2) {
			if (td == p + 1) {
				arc_lengths += arcs[routes[pu_vehicle].route[p-1].gen_idx][routes[pu_vehicle].route[td+1].gen_idx];
			} else {
				arc_lengths += arcs[routes[pu_vehicle].route[p-1].gen_idx][routes[pu_vehicle].route[p+1].gen_idx]
								+ arcs[routes[pu_vehicle].route[td-1].gen_idx][routes[pu_vehicle].route[td+1].gen_idx];
			}
		} else if (p > 1 && td == routes[pu_vehicle].route.size() - 2) {
			if (td == p + 1) {
				arc_lengths += arcs[nearest_depot_gen_idx_d[routes[pu_vehicle].route[p-1].index]][routes[pu_vehicle].route[p-1].gen_idx];
			} else {
				arc_lengths += arcs[routes[pu_vehicle].route[p-1].gen_idx][routes[pu_vehicle].route[p+1].gen_idx]
								+ arcs[nearest_depot_gen_idx_d[routes[pu_vehicle].route[td-1].index]][routes[pu_vehicle].route[td-1].gen_idx];
			}
		} else if (p == 1 && td < routes[pu_vehicle].route.size() - 2) {
			if (td == p + 1) {
				arc_lengths += arcs[routes[pu_vehicle].route[td+1].gen_idx][nearest_depot_gen_idx_p[routes[pu_vehicle].route[td+1].index]];
			} else {
				arc_lengths += arcs[nearest_depot_gen_idx_p[routes[pu_vehicle].route[p+1].index]][routes[pu_vehicle].route[p+1].gen_idx]
								+ arcs[routes[pu_vehicle].route[td-1].gen_idx][routes[pu_vehicle].route[td+1].gen_idx];
			}
		} else if (p == 1 && td == routes[pu_vehicle].route.size() - 2) {
			if (td == p + 1) {
				arc_lengths += 0;
			} else {
				arc_lengths += arcs[routes[pu_vehicle].route[p].gen_idx][nearest_depot_gen_idx_p[request]]
								+ arcs[routes[pu_vehicle].route[td].gen_idx][nearest_depot_gen_idx_d[request]];
			}
		} else {
			cout << "Error calculating costs for transfer insertion!\n";
		}

		// Delivery change in arcs
		if (d == tp + 1) {
			arc_lengths = arc_lengths - arcs[routes[de_vehicle].route[tp].gen_idx][routes[de_vehicle].route[d].gen_idx]
							- arcs[routes[de_vehicle].route[tp-1].gen_idx][routes[de_vehicle].route[tp].gen_idx]
							- arcs[routes[de_vehicle].route[d].gen_idx][routes[de_vehicle].route[d-1].gen_idx];
		} else {
			arc_lengths = arc_lengths - arcs[routes[de_vehicle].route[tp].gen_idx][routes[de_vehicle].route[tp+1].gen_idx]
							- arcs[routes[de_vehicle].route[d-1].gen_idx][routes[de_vehicle].route[d].gen_idx]
							- arcs[routes[de_vehicle].route[tp-1].gen_idx][routes[de_vehicle].route[tp].gen_idx]
							- arcs[routes[de_vehicle].route[d].gen_idx][routes[de_vehicle].route[d+1].gen_idx];
		}

		if (tp > 1 && d < routes[de_vehicle].route.size() - 2) {
			if (d == tp + 1) {
				arc_lengths += arcs[routes[de_vehicle].route[tp-1].gen_idx][routes[de_vehicle].route[d+1].gen_idx];
			} else {
				arc_lengths += arcs[routes[de_vehicle].route[tp-1].gen_idx][routes[de_vehicle].route[tp+1].gen_idx]
								+ arcs[routes[de_vehicle].route[d-1].gen_idx][routes[de_vehicle].route[d+1].gen_idx];
			}
		} else if (tp > 1 && d == routes[de_vehicle].route.size() - 2) {
			if (d == tp + 1) {
				arc_lengths += arcs[nearest_depot_gen_idx_d[routes[de_vehicle].route[tp-1].index]][routes[de_vehicle].route[tp-1].gen_idx];
			} else {
				arc_lengths += arcs[routes[de_vehicle].route[tp-1].gen_idx][routes[de_vehicle].route[tp+1].gen_idx]
								+ arcs[nearest_depot_gen_idx_d[routes[de_vehicle].route[d-1].index]][routes[de_vehicle].route[d-1].gen_idx];
			}
		} else if (tp == 1 && d < routes[de_vehicle].route.size() - 2) {
			if (d == tp + 1) {
				arc_lengths += arcs[routes[de_vehicle].route[d+1].gen_idx][nearest_depot_gen_idx_p[routes[de_vehicle].route[d+1].index]];
			} else {
				arc_lengths += arcs[nearest_depot_gen_idx_p[routes[de_vehicle].route[tp+1].index]][routes[de_vehicle].route[tp+1].gen_idx]
								+ arcs[routes[de_vehicle].route[d-1].gen_idx][routes[de_vehicle].route[d+1].gen_idx];
			}
		} else if (tp == 1 && d == routes[de_vehicle].route.size() - 2) {
			if (d == tp + 1) {
				arc_lengths += 0;
			} else {
				arc_lengths += arcs[routes[de_vehicle].route[tp].gen_idx][nearest_depot_gen_idx_p[request]]
								+ arcs[routes[de_vehicle].route[d].gen_idx][nearest_depot_gen_idx_d[request]];
			}
		} else {
			cout << "Error calculating costs for transfer insertion!\n";
		}
	} else {
		if (d == p + 1) {
			arc_lengths = arc_lengths - arcs[routes[de_vehicle].route[p].gen_idx][routes[de_vehicle].route[d].gen_idx]
							- arcs[routes[de_vehicle].route[p-1].gen_idx][routes[de_vehicle].route[p].gen_idx]
							- arcs[routes[de_vehicle].route[d-1].gen_idx][routes[de_vehicle].route[d].gen_idx];
		} else {
			arc_lengths = arc_lengths - arcs[routes[de_vehicle].route[p].gen_idx][routes[de_vehicle].route[p+1].gen_idx]
							- arcs[routes[de_vehicle].route[d-1].gen_idx][routes[de_vehicle].route[d].gen_idx]
							- arcs[routes[de_vehicle].route[p-1].gen_idx][routes[de_vehicle].route[p].gen_idx]
							- arcs[routes[de_vehicle].route[d].gen_idx][routes[de_vehicle].route[d+1].gen_idx];
		}

		if (p > 1 && d < routes[de_vehicle].route.size() - 2) {
			if (d == p + 1) {
				arc_lengths += arcs[routes[de_vehicle].route[p-1].gen_idx][routes[de_vehicle].route[d+1].gen_idx];
			} else {
				arc_lengths += arcs[routes[de_vehicle].route[p-1].gen_idx][routes[de_vehicle].route[p+1].gen_idx]
								+ arcs[routes[de_vehicle].route[d-1].gen_idx][routes[de_vehicle].route[d+1].gen_idx];
			}
		} else if (p > 1 && d == routes[de_vehicle].route.size() - 2) {
			if (d == p + 1) {
				arc_lengths += arcs[nearest_depot_gen_idx_d[routes[de_vehicle].route[p-1].index]][routes[de_vehicle].route[p-1].gen_idx];
			} else {
				arc_lengths += arcs[routes[de_vehicle].route[p-1].gen_idx][routes[de_vehicle].route[p+1].gen_idx]
								+ arcs[nearest_depot_gen_idx_d[routes[de_vehicle].route[d-1].index]][routes[de_vehicle].route[d-1].gen_idx];
			}
		} else if (p == 1 && d < routes[de_vehicle].route.size() - 2) {
			if (d == p + 1) {
				arc_lengths += arcs[routes[de_vehicle].route[d+1].gen_idx][nearest_depot_gen_idx_p[routes[de_vehicle].route[d+1].index]];
			} else {
				arc_lengths += arcs[nearest_depot_gen_idx_p[routes[de_vehicle].route[p+1].index]][routes[de_vehicle].route[p+1].gen_idx]
								+ arcs[routes[de_vehicle].route[d-1].gen_idx][routes[de_vehicle].route[d+1].gen_idx];
			}
		} else if (p == 1 && d == routes[de_vehicle].route.size() - 2) {
			if (d == p + 1) {
				arc_lengths += 0;
			} else {
				arc_lengths += arcs[routes[de_vehicle].route[p].gen_idx][nearest_depot_gen_idx_p[request]]
								+ arcs[routes[de_vehicle].route[d].gen_idx][nearest_depot_gen_idx_d[request]];
			}
		} else {
			cout << "Error calculating costs for transfer insertion!\n";
		}
	}
	return travel_cost*arc_lengths;
}

void Instance::remove_empty_vehicle(){
    for(size_t idx = 0; idx < routes.size(); idx++){
        if(routes[idx].route.size() == 2){
            routes.erase(routes.begin() + idx);
            for(size_t adx = 0; adx < request_amount; adx++){
                if(pickup_vehicle[adx] > idx){pickup_vehicle[adx] -= 1;}
                if(delivery_vehicle[adx] > idx){delivery_vehicle[adx] -= 1;}
            }
            idx--;
        }
    }
    for(size_t idx = 0; idx < routes.size(); idx++){
        routes[idx].v_index = idx;
    }

}
