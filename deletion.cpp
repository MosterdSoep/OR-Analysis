#include "instance.h"
#include <algorithm>

size_t Instance::greedy_request_deletion(vector<size_t> request_bank) {
	double best_costs_saving = numeric_limits<double>::max();
	size_t best_request = 0;
	for (size_t i = 0; i < request_amount; i++) {
	    if(find(request_bank.begin(), request_bank.end(), i) != request_bank.end()){

	    }else{
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
	size_t pu_vehicle = pickup_vehicle[request];
	size_t de_vehicle = delivery_vehicle[request];
	double arc_lengths = 0.0;
	vector<double> new_arc_durations = routes[pu_vehicle].arc_durations;
	vector<Node> new_route = routes[pu_vehicle].route;
	vector<double> new_arc_durations_de = routes[de_vehicle].arc_durations;


	if (pu_vehicle != de_vehicle) {
		// Request is transfered
		// Remove pickup node and delivery transfer node from pickup vehicle
		for (size_t i = routes[pu_vehicle].route.size()-2; i > 0; i--) {
			if (routes[pu_vehicle].route[i].index == request) {
				// Place in the vehicle/route that corresponds to the request
				arc_lengths = arc_lengths + arcs[routes[pu_vehicle].route[i-1].gen_idx][routes[pu_vehicle].route[i+1].gen_idx] - new_arc_durations[i-1] - new_arc_durations[i];
				new_arc_durations.erase(new_arc_durations.begin() + i);
				new_arc_durations.erase(new_arc_durations.begin() + i-1);
				new_arc_durations.insert(new_arc_durations.begin() + i-1, arcs[routes[pu_vehicle].route[i-1].gen_idx][routes[pu_vehicle].route[i+1].gen_idx]);
			}
		}
		// Remove delivery node and pickup transfer node from delivery vehicle
		for (size_t i = routes[de_vehicle].route.size()-2; i > 0; i--) {
			if (routes[de_vehicle].route[i].index == request) {
				// Place in the vehicle/route that corresponds to the request
				arc_lengths = arc_lengths + arcs[routes[de_vehicle].route[i-1].gen_idx][routes[de_vehicle].route[i+1].gen_idx] - new_arc_durations_de[i-1] - new_arc_durations_de[i];
				new_arc_durations_de.erase(new_arc_durations_de.begin() + i);
				new_arc_durations_de.erase(new_arc_durations_de.begin() + i-1);
				new_arc_durations_de.insert(new_arc_durations_de.begin() + i-1, arcs[routes[de_vehicle].route[i-1].gen_idx][routes[de_vehicle].route[i+1].gen_idx]);
			}
		}
	} else {
		// Pickup and delivery is done by the same vehicle
		// Gain in costs of removing pickup
		if (new_route[new_route.size() - 2].index = request){
				// Either pickup or delivery node of the request
				// Remove arcs (i-1, i), (i, i+1) then create arc (i-1, i+1)
            arc_lengths += arcs[new_route[new_route.size() - 3].gen_idx][nearest_depot_gen_idx_d[new_route[new_route.size()-3].index]] -
                        new_arc_durations[new_route.size()-3] - new_arc_durations[new_route.size()-2];
			new_arc_durations.erase(new_arc_durations.end()-1);
			new_arc_durations.erase(new_arc_durations.end()-2);
			new_arc_durations.push_back(arcs[new_route[new_route.size()-3].gen_idx][nearest_depot_gen_idx_d[new_route[new_route.size()-3].index]]);
            new_route.erase(new_route.end() - 2);
		}
		// -3, as we do not need to check the depot at size-1, or the last node, which is done before
		for (size_t i = new_route.size()-3; i > 1; i--) {
			// Looping through the vehicle that pickups request #request
			if (new_route[i].index == request) {
				new_route.erase(new_route.begin() + i);
				// Either pickup or delivery node of the request
				// Remove arcs (i-1, i), (i, i+1) then create arc (i-1, i+1)
				arc_lengths += arcs[new_route[i-1].gen_idx][new_route[i].gen_idx] - new_arc_durations[i-1] - new_arc_durations[i];
				new_arc_durations.erase(new_arc_durations.begin() + i);
				new_arc_durations.erase(new_arc_durations.begin() + i - 1);
				new_arc_durations.insert(new_arc_durations.begin() + i - 1, arcs[new_route[i-1].gen_idx][new_route[i].gen_idx]);
			}
		}
		if (new_route[1].index = request){
            new_route.erase(new_route.begin() + 1);
            arc_lengths += arcs[nearest_depot_gen_idx_p[new_route[1].index]][new_route[1].gen_idx] - new_arc_durations[0] - new_arc_durations[1];
            new_arc_durations.erase(new_arc_durations.begin());
            new_arc_durations.erase(new_arc_durations.begin());
            new_arc_durations.insert(new_arc_durations.begin(), arcs[nearest_depot_gen_idx_p[new_route[1].index]][new_route[1].gen_idx]);
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
