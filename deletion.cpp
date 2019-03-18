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

void Instance::greedy_route_deletion(vector<size_t> request_bank) {

}

void Instance::random_route_deletion(vector<size_t> request_bank) {

}

double Instance::costs_of_removing_request(size_t request) {
	cout << "Costs\n";
	size_t pu_vehicle = pickup_vehicle[request];
	size_t de_vehicle = delivery_vehicle[request];
	double arc_lengths = 0.0;
	
	size_t p = -1, d = -1, td = -1, tp = -1;
	for (size_t i = 0; i < routes[pu_vehicle].route.size(); i++) {
		if (routes[pu_vehicle].route[i].request_idx == request) {
			if (p == -1) p = i;
			else td = i;
		}
	}
	for (size_t i = 0; i < routes[de_vehicle].route.size(); i++) {
		if (routes[de_vehicle].route[i].request_idx == request) {
			if (tp == -1) tp = i;
			else d = i;
		}
	}

	if (pu_vehicle != de_vehicle) {
		
	} else {
		cout << "Correctly no transfer\n";
		if (d == p + 1) {
			cout << "here right\n";
			arc_lengths -= (arcs[routes[de_vehicle].route[p].gen_idx][routes[de_vehicle].route[d].gen_idx]
							+ arcs[routes[de_vehicle].route[p-1].gen_idx][routes[de_vehicle].route[p].gen_idx]
							+ arcs[routes[de_vehicle].route[d-1].gen_idx][routes[de_vehicle].route[d].gen_idx]);
		} else {
			cout << "here right 2\n";
			arc_lengths -= (arcs[routes[de_vehicle].route[p].gen_idx][routes[de_vehicle].route[p+1].gen_idx]
							+ arcs[routes[de_vehicle].route[d-1].gen_idx][routes[de_vehicle].route[d].gen_idx]
							+ arcs[routes[de_vehicle].route[p-1].gen_idx][routes[de_vehicle].route[p].gen_idx]
							+ arcs[routes[de_vehicle].route[d-1].gen_idx][routes[de_vehicle].route[d].gen_idx]);
		}
		cout << "Test1\n";
		if (p > 1 && d < routes[de_vehicle].route.size() - 2) {
			cout << "Test2\n";
			arc_lengths += arcs[routes[de_vehicle].route[p-1].gen_idx][routes[de_vehicle].route[p+1].gen_idx] 
							+ arcs[routes[de_vehicle].route[d-1].gen_idx][routes[de_vehicle].route[d+1].gen_idx];
		} else if (p > 1 && d == routes[de_vehicle].route.size() - 2) {
			cout << "Test3\n";
			// Change ending depot then add
			arc_lengths += arcs[routes[de_vehicle].route[p-1].gen_idx][routes[de_vehicle].route[p+1].gen_idx];
			
			double min_val = numeric_limits<double>::max();
			for(size_t idx = 0; idx < depot_nodes.size(); idx++){
				if(min_val > arcs[routes[de_vehicle].route[routes[de_vehicle].route.size() - 2].gen_idx][depot_nodes[idx].gen_idx]){
					min_val = arcs[routes[de_vehicle].route[routes[de_vehicle].route.size() - 2].gen_idx][depot_nodes[idx].gen_idx];
				}
			}
			arc_lengths += min_val;
		} else if (p == 1 && d < routes[de_vehicle].route.size() - 1) {
			cout << "Test4\n";
			// Change beginning depot then add arcs
			arc_lengths += arcs[routes[de_vehicle].route[d-1].gen_idx][routes[de_vehicle].route[d+1].gen_idx];
			
			double min_val = numeric_limits<double>::max();
			for(size_t idx = 0; idx < depot_nodes.size(); idx++){
				if(min_val > arcs[depot_nodes[idx].gen_idx][routes[de_vehicle].route[p+1].gen_idx]){
					min_val = arcs[depot_nodes[idx].gen_idx][routes[de_vehicle].route[p+1].gen_idx];
				}
			}
		} else if (p == 1 && d == routes[de_vehicle].route.size() - 1) {
			cout << "Test5\n";
			// Change both depots then add arcs
			
			double min_val_start = numeric_limits<double>::max();
			double min_val_end = numeric_limits<double>::max();
			for(size_t idx = 0; idx < depot_nodes.size(); idx++){
				if(min_val_end > arcs[routes[de_vehicle].route[routes[de_vehicle].route.size() - 2].gen_idx][depot_nodes[idx].gen_idx]){
					min_val_end = arcs[routes[de_vehicle].route[routes[de_vehicle].route.size() - 2].gen_idx][depot_nodes[idx].gen_idx];
				}
				if(min_val_start > arcs[depot_nodes[idx].gen_idx][routes[de_vehicle].route[p+1].gen_idx]){
					min_val_start = arcs[depot_nodes[idx].gen_idx][routes[de_vehicle].route[p+1].gen_idx];
				}
			}
			arc_lengths += min_val_start + min_val_end;
		} else {
			cout << "Error calculating costs for transfer insertion!\n";
		}
		cout << "End of if statement\n";
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