#include "instance.h"
#include <limits>

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
					vector<Vehicle> old_routes = routes;
					vector<size_t> old_pickup_vehicle = pickup_vehicle;
					vector<size_t> old_delivery_vehicle = delivery_vehicle;
					
					cout << "Before node addition\n";
					routes[best_vehicle].add_node(p, pickup_nodes[request]);
					routes[best_vehicle].add_node(d + 1, delivery_nodes[request]);
					cout << "After node addition\n";
					if (routes[v].maximum_ride_time_correct() && routes[v].time_windows_correct() && routes[v].current_capacity_correct()) {
						best_costs = candidate_costs;
						best_vehicle = v;
						best_pickup_location = p;
						best_delivery_location = d;
					}
					routes = old_routes;
					pickup_vehicle = old_pickup_vehicle;
					delivery_vehicle = old_delivery_vehicle;
				}
			}
		}
	}
	cout << "Before node addition\n";
	routes[best_vehicle].add_node(best_pickup_location, pickup_nodes[request]);
	routes[best_vehicle].add_node(best_delivery_location + 1, delivery_nodes[request]);
	cout << "After node addition\n";
}

void Instance::random_request_insertion() {

}

void Instance::greedy_route_insertion(size_t request) {
	size_t k1 = 0, k2 = 0, best_p = 0, best_d = 0, best_td = 0, best_tp = 0;
	double current_costs = 0, best_costs = numeric_limits<double>::max();
	Transfer_Node transfer_node;
	vector<Transfer_Node> open_facilities;
	
	for (Transfer_Node tn : transfer_nodes) {
		if (tn.open) { open_facilities.push_back(tn); }
	}
	
	for (Transfer_Node tn : open_facilities) {
		for (size_t v1 = 0; v1 < routes.size() - 1; v1++) {
			for (size_t v2 = v1 + 1; v2 < routes.size(); v2++) {
				// All possible pairs of vehicles
			
				// v1 as pickup, v2 as delivery
				for (size_t p = 0; p < routes[v1].route.size() - 1; p++) {
					for (size_t td = pu + 1; td < routes[v1].route.size(); td++) {
						pickup_costs = costs_of_inserting_request_with_transfer(routes[v1], p, td, request, true, tn);
						routes[v1].add_node(p, pickup_nodes[request]);
						routes[v1].add_node(td, tn);
						for (size_t tp = 0; tp < routes[v2].route.size() - 1) {
							routes[v2].add_node(tp, tn);
							if (routes[v1].time_at_node[td] + td.service_time < routes[v2].time_at_node[tp]) {
								// Only look for possible transfers, e.g. when time windows are correct for the transfer
								
								for (size_t d = tp + 1; d < routes[v2].route.size()) {
									// Check for every possible delivery
									double delivery_costs = costs_of_inserting_request_with_transfer(routes[v1], tp, d, request, false, tn);
									if (pickup_costs + delivery_costs < best_costs) {
										best_costs = pickup_costs + delivery_costs;
										k1 = v1;
										k2 = v2;
										best_p = p;
										best_d = d;
										best_td = td;
										best_tp = tp;
										transfer_node = tn;
									}
								}
							}
						}
						
					}
				}
				
				// v1 as delivery, v2 as pickup
				
				
				
			}
		}
	}
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

double Instance::costs_of_inserting_request_with_transfer(Vehicle v, size_t p, size_t d, size_t request, bool pickup, Transfer_Node tn) {
	double arc_lengths = 0.0;
	vector<double> new_arc_durations = v.arc_durations;
	
	if (pickup) {
		// Pickup insertion
		arc_lengths += arcs[v.route[p-1].gen_idx][pickup_nodes[request].gen_idx] + arcs[pickup_nodes[request].gen_idx][v.route[p].gen_idx] - new_arc_durations[p-1];
		new_arc_durations.erase(new_arc_durations.begin() + p-1);
		new_arc_durations.insert(new_arc_durations.begin() + p-1, arcs[pickup_nodes[request].gen_idx][v.route[p].gen_idx]);
		new_arc_durations.insert(new_arc_durations.begin() + p-1, arcs[v.route[p-1].gen_idx][pickup_nodes[request].gen_idx]);
	
		// Transfer delivery insertion
		arc_lengths += arcs[v.route[d-1].gen_idx][delivery_nodes[request].gen_idx] + arcs[delivery_nodes[request].gen_idx][v.route[d-1].gen_idx] - new_arc_durations[d];
		new_arc_durations.erase(new_arc_durations.begin() + d);
		new_arc_durations.insert(new_arc_durations.begin() + d, arcs[delivery_nodes[request].gen_idx][v.route[d-1].gen_idx]);
		new_arc_durations.insert(new_arc_durations.begin() + d, arcs[v.route[d-1].gen_idx][delivery_nodes[request].gen_idx]);
	} else {
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
	}

	return travel_cost*arc_lengths;
}
