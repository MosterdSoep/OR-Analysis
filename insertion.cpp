#include "instance.h"

void Instance::greedy_request_insertion(size_t request) {
	size_t best_vehicle = 0;
	size_t best_pickup_location = 0;
	size_t best_delivery_location = 0;
	double best_costs = numeric_limits<double>::max();

	for (size_t v = 0; v < routes.size(); v++) {
		for (size_t p = 1; p < routes[v].route.size() - 1; p++) {
			for (size_t d = p; d < routes[v].route.size(); d++) {
				double candidate_costs = costs_of_inserting_request(this->routes[v], p, d, request);
				if (candidate_costs < best_costs) {
					best_costs = candidate_costs;
					best_vehicle = v;
					best_pickup_location = p;
					best_delivery_location = d;
				}
			}
		}
	}
	// Now insert the request
	routes[best_vehicle].add_node(best_pickup_location, pickup_nodes[request]);
	routes[best_vehicle].add_node(best_delivery_location + 1, delivery_nodes[request]);
}

void Instance::random_request_insertion() {

}

void Instance::greedy_route_insertion() {

}

void Instance::random_route_insertion() {
	
}

double Instance::costs_of_inserting_request(Vehicle v, size_t p, size_t d, size_t request) {
	double arc_lengths = 0.0;
	vector<double> new_arc_durations = v.arc_durations;

	// Pickup insertion
	arc_lengths += arcs[v.route[p-1].gen_idx][pickup_nodes[request].gen_idx] + arcs[pickup_nodes[request].gen_idx][v.route[p].gen_idx] - new_arc_durations[p-1];
	new_arc_durations.erase(new_arc_durations.begin() + p-1);
	new_arc_durations.insert(new_arc_durations.begin() + p-1, arcs[pickup_nodes[request].gen_idx][v.route[p].gen_idx]);
	new_arc_durations.insert(new_arc_durations.begin() + p-1, arcs[v.route[p-1].gen_idx][pickup_nodes[request].gen_idx]);

	// Delivery insertion
	arc_lengths += arcs[v.route[d-1].gen_idx][delivery_nodes[request].gen_idx] + arcs[delivery_nodes[request].gen_idx][v.route[d-1].gen_idx] - new_arc_durations[d];
	new_arc_durations.erase(new_arc_durations.begin() + d);
	new_arc_durations.insert(new_arc_durations.begin() + d, arcs[delivery_nodes[request].gen_idx][v.route[d-1].gen_idx]);
	new_arc_durations.insert(new_arc_durations.begin() + d, arcs[v.route[d-1].gen_idx][delivery_nodes[request].gen_idx]);

	return travel_cost*arc_lengths;
}
