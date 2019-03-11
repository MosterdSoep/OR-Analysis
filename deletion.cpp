#include "instance.h"
#include <algorithm>

size_t Instance::greedy_request_deletion(vector<size_t> request_bank) {
	double best_costs_saving = 0;
	size_t best_request = 0;
	for (size_t i = 0; i < request_amount; i++) {
		double candidate = costs_of_removing_request(i);
		if (candidate < best_costs_saving) {
			best_costs_saving = candidate;
			best_request = i;
		}
	}
	if(!request_bank.empty()){
	while (find(request_bank.begin(), request_bank.end(), best_request) != request_bank.end()) {
		best_costs_saving = 0;
		best_request = 0;
		for (size_t i = 0; i < request_amount; i++) {
			double candidate = costs_of_removing_request(i);
			if (candidate < best_costs_saving) {
				best_costs_saving = candidate;
				best_request = i;
			}
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
		for (size_t i = 0; i < routes[pu_vehicle].route.size(); i++) {
			if (routes[pu_vehicle].route[i].index == request) {
				// Place in the vehicle/route that corresponds to the request
				arc_lengths = arc_lengths + arcs[routes[pu_vehicle].route[i-1].gen_idx][routes[pu_vehicle].route[i+1].gen_idx] - new_arc_durations[i-1] - new_arc_durations[i];
				new_arc_durations.erase(new_arc_durations.begin() + i);
				new_arc_durations.erase(new_arc_durations.begin() + i-1);
				new_arc_durations.insert(new_arc_durations.begin() + i-1, arcs[routes[pu_vehicle].route[i-1].gen_idx][routes[pu_vehicle].route[i+1].gen_idx]);
			}
		}
		// Remove delivery node and pickup transfer node from delivery vehicle
		for (size_t i = 0; i < routes[de_vehicle].route.size(); i++) {
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
		for (size_t i = 1; i < new_route.size() - 1; i++) {
			// Looping through the vehicle that pickups request #request
			if (new_route[i].index == request) {
				new_route.erase(new_route.begin() + i);
				// Either pickup or delivery node of the request
				// Remove arcs (i-1, i), (i, i+1) then create arc (i-1, i+1)
				arc_lengths = arc_lengths + arcs[routes[pu_vehicle].route[i-1].gen_idx][routes[pu_vehicle].route[i+1].gen_idx] - new_arc_durations[i-1] - new_arc_durations[i];
				new_arc_durations.erase(new_arc_durations.begin() + i);
				new_arc_durations.erase(new_arc_durations.begin() + i - 1);
				new_arc_durations.insert(new_arc_durations.begin() + i - 1, arcs[routes[pu_vehicle].route[i-1].gen_idx][routes[pu_vehicle].route[i+1].gen_idx]);
			}
		}
	}
	return travel_cost*arc_lengths;
}
