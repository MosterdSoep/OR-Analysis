#include "instance.h"
#include <limits>
#include <algorithm>

size_t Instance::greedy_request_insertion(vector<size_t> request_bank) {
    //calculate costs for a new vehicle as a base case
	size_t best_vehicle = routes.size();
	size_t best_pickup_location = 1;
	size_t best_delivery_location = 2;
	double best_costs = numeric_limits<double>::max();
	size_t best_request = 0;
    for(size_t req : request_bank){
        if(nearest_depot_insertion_cost[req] < best_costs){
            best_costs = nearest_depot_insertion_cost[req];
            best_request = req;
        }
    }
    for(size_t request : request_bank){
    //see if other insertions are better
	for (size_t v = 0; v < routes.size(); v++) {
		for (size_t p = 1; p < routes[v].route.size() - 1; p++) {
			for (size_t d = p + 1; d < routes[v].route.size(); d++) {
				double candidate_costs = costs_of_inserting_request(routes[v], p, d, request);
				if (candidate_costs < best_costs) {
					/*vector<Vehicle> old_routes = routes;
					vector<size_t> old_pickup_vehicle = pickup_vehicle;
					vector<size_t> old_delivery_vehicle = delivery_vehicle;

					routes[best_vehicle].add_node(p, pickup_nodes[request]);
					routes[best_vehicle].add_node(d + 1, delivery_nodes[request]);
					if (routes[v].maximum_ride_time_correct() && routes[v].time_windows_correct() && routes[v].current_capacity_correct()) {*/
						best_costs = candidate_costs;
						best_vehicle = v;
						best_pickup_location = p;
						best_delivery_location = d;
						best_request = request;
					/*}
					routes = old_routes;
					pickup_vehicle = old_pickup_vehicle;
					delivery_vehicle = old_delivery_vehicle;*/
				}
			}
		}
	}
    }
	if (best_vehicle < routes.size()) {
        routes[best_vehicle].add_node(best_pickup_location, pickup_nodes[best_request]);
        routes[best_vehicle].add_node(best_delivery_location + 1, delivery_nodes[best_request]);
	} else if (best_vehicle == routes.size()){
	    size_t insert_loc = routes.size();
        routes.push_back(Vehicle());
        routes[routes.size()-1].v_index = routes.size()-1;
        routes[insert_loc].add_node(1, pickup_nodes[best_request]);
        routes[insert_loc].add_node(2, delivery_nodes[best_request]);
	} else { cout << "insertion failed\n"; }

	size_t best_request_loc = 0;
	for (size_t idx = 0; idx < request_bank.size(); idx++) {
        if (request_bank[idx] == best_request) {
            best_request_loc  = idx;
        }
	}

    return best_request_loc;
}

void Instance::random_request_insertion(size_t request) {

}

void Instance::greedy_route_insertion(size_t request) {
	size_t k1 = 0, k2 = 0, best_p = 0, best_d = 0, best_td = 0, best_tp = 0;
	double best_costs = numeric_limits<double>::max();
	Transfer_Node transfer_node;
	vector<Transfer_Node> open_facilities;

	for (Transfer_Node tn : transfer_nodes) {
		if (tn.open) { open_facilities.push_back(tn); }
	}
	
	// Case 1: if there are 0 vehicles -> find best 2 vehicles and insert a request with transfer
	// Case 2: if there is 1 vehicle -> find one other vehicle and insert a request with transfer in those vehicles
	
	for (Transfer_Node tn : open_facilities) {
		routes.push_back(Vehicle());
		routes.push_back(Vehicle());
		for (size_t v1 = 0; v1 < routes.size(); v1++) {
			for (size_t v2 = 0; v2 < routes.size(); v2++) {
				if (v1 == v2) {
					continue;
				}
				for (size_t p = 1; p < routes[v1].route.size() - 1; p++) {
					for (size_t td = p + 1; td < routes[v1].route.size(); td++) {
						
						double pickup_costs = costs_of_inserting_request_with_transfer_pickup(routes[v1], p, td, request, tn);
						for (size_t tp = 1; tp < routes[v2].route.size() - 1; tp++) {
							
							double minimum_slack = *min_element(routes[v2].slack_at_node.begin(), routes[v2].slack_at_node.end());
							if (routes[v1].time_at_node[td] + tn.service_time < routes[v2].time_at_node[tp] + minimum_slack) {
								// Only look for possible transfers, e.g. when time windows are correct for the transfer

								for (size_t d = tp + 1; d < routes[v2].route.size(); d++) {
									double delivery_costs = costs_of_inserting_request_with_transfer_delivery(routes[v2], tp, d, request, tn);
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
			}
		}
	}
	routes[k1].add_node(best_p, pickup_nodes[request]);
	routes[k1].add_delivery_transfer(best_td, transfer_node, request);
	routes[k2].add_pickup_transfer(best_tp, transfer_node, 0, request);
	routes[k2].add_node(best_d, delivery_nodes[request]);
}


void Instance::random_route_insertion(size_t request) {

}

double Instance::costs_of_inserting_request(Vehicle v, size_t p, size_t d, size_t request) {
	double arc_lengths = 0.0;
	
	if (d == p + 1) {
		arc_lengths += arcs[pickup_nodes[request].gen_idx][delivery_nodes[request].gen_idx]
						- arcs[v.route[p-1].gen_idx][v.route[p].gen_idx];
	} else {
		arc_lengths += arcs[pickup_nodes[request]][v.route[p].gen_idx]
						+ arcs[v.route[d-2].gen_idx][delivery_nodes[request].gen_idx]
						- arcs[v.route[p-1].gen_idx][v.route[p].gen_idx]
						- arcs[v.route[d-2].gen_idx][v.route[d-1].gen_idx];
	}
	
	if (p > 0 && d < v.route.size()-1) {
		arc_lengths += arcs[v.route[p-1].gen_idx][pickup_nodes[request].gen_idx] + arcs[delivery_nodes[request].gen_idx][v.route[d-1].gen_idx];
	} else if (p > 1 && d == v.route.size() - 1) {
		// Change ending depot then add 
		arc_lengths += arcs[v.route[p].gen_idx][pickup_nodes[request].gen_idx];
		
		double min_val = numeric_limits<double>::max();
		for(size_t idx = 0; idx < depot_nodes.size(); idx++){
			if(min_val > arcs[v.route[v.route.size() - 1].gen_idx][depot_nodes[idx].gen_idx]){
				min_val = arcs[v.route[v.route.size() - 1].gen_idx][depot_nodes[idx].gen_idx];
			}
		}
		arc_lengths += min_val;
	} else if (p == 1 && d < v.route.size() - 1) {
		// Change beginning depot then add arcs
		arc_lengths += arcs[delivery_nodes[request].gen_idx][v.route[d-1].gen_idx];
		
		double min_val = numeric_limits<double>::max();
		for(size_t idx = 0; idx < depot_nodes.size(); idx++){
			if(min_val > arcs[depot_nodes[idx].gen_idx][v.route[p].gen_idx]){
				min_val = arcs[depot_nodes[idx].gen_idx][v.route[p].gen_idx];
			}
		}
	} else if (p == 1 && d == v.route.size() - 1) {
		// Change both depots then add arcs
		
		double min_val_start = numeric_limits<double>::max();
		double min_val_end = numeric_limits<double>::max();
		for(size_t idx = 0; idx < depot_nodes.size(); idx++){
			if(min_val_end > arcs[v.route[v.route.size() - 1].gen_idx][depot_nodes[idx].gen_idx]){
				min_val_end = arcs[v.route[v.route.size() - 1].gen_idx][depot_nodes[idx].gen_idx];
			}
			if(min_val_start > arcs[depot_nodes[idx].gen_idx][v.route[p].gen_idx]){
				min_val_start = arcs[depot_nodes[idx].gen_idx][v.route[p].gen_idx];
			}
		}
		arc_lengths += min_val_start + min_val_end;
	} else {
		cout << "Error calculating costs for transfer insertion!\n";
	}
	
	/*
	//short feasibility check
    double min_slack = *min_element(v.slack_at_node.begin() + p, v.slack_at_node.end());
	if ((arc_lengths + pickup_nodes[request].service_time > min_slack)||
       (v.time_at_node[p-1] + arcs[v.route[p-1].gen_idx][v.route[p].gen_idx] + v.route[p-1].service_time + v.waiting_times[p-1] > pickup_nodes[request].upper_bound)){
        arc_lengths = numeric_limits<double>::max()/2;
	}

	
    //short feasibility check
    min_slack = *min_element(v.slack_at_node.begin() + d, v.slack_at_node.end());
    if ((arc_lengths + pickup_nodes[request].service_time + delivery_nodes[request].service_time> min_slack) ||
       (v.time_at_node[d-1] + v.waiting_times[d-1] + v.route[d-1].service_time + arc_lengths -  arcs[delivery_nodes[request].gen_idx][v.route[d].gen_idx] > delivery_nodes[request].upper_bound)  ){
        arc_lengths = numeric_limits<double>::max()/2;
	}*/
	return travel_cost*arc_lengths;
}

double Instance::costs_of_inserting_request_with_transfer_pickup(Vehicle v, size_t p, size_t d, size_t request, Transfer_Node tn) {
	double arc_lengths = 0.0;
	
	if (d == p + 1) {
		arc_lengths += arcs[pickup_nodes[request].gen_idx][tn.gen_idx]
						- arcs[v.route[p-1].gen_idx][v.route[p].gen_idx];
	} else {
		arc_lengths += arcs[pickup_nodes[request]][v.route[p].gen_idx]
						+ arcs[v.route[d-2].gen_idx][tn.gen_idx]
						- arcs[v.route[p-1].gen_idx][v.route[p].gen_idx]
						- arcs[v.route[d-2].gen_idx][v.route[d-1].gen_idx];
	}
	
	if (p > 0 && d < v.route.size()-1) {
		arc_lengths += arcs[v.route[p-1].gen_idx][pickup_nodes[request].gen_idx] + arcs[tn.gen_idx][v.route[d-1].gen_idx];
	} else if (p > 0 && d == v.route.size() - 1) {
		// Change ending depot then add 
		arc_lengths += arcs[v.route[p].gen_idx][pickup_nodes[request].gen_idx];
		
		double min_val = numeric_limits<double>::max();
		for(size_t idx = 0; idx < depot_nodes.size(); idx++){
			if(min_val > arcs[v.route[v.route.size() - 1].gen_idx][depot_nodes[idx].gen_idx]){
				min_val = arcs[v.route[v.route.size() - 1].gen_idx][depot_nodes[idx].gen_idx];
			}
		}
		arc_lengths += min_val;
	} else if (p == 0 && d < v.route.size() - 1) {
		// Change beginning depot then add arcs
		arc_lengths += arcs[tn.gen_idx][v.route[d-1].gen_idx];
		
		double min_val = numeric_limits<double>::max();
		for(size_t idx = 0; idx < depot_nodes.size(); idx++){
			if(min_val > arcs[depot_nodes[idx].gen_idx][v.route[0].gen_idx]){
				min_val = arcs[depot_nodes[idx].gen_idx][v.route[0].gen_idx];
			}
		}
	} else if (p == 0 && d == v.route.size() - 1) {
		// Change both depots then add arcs
		
		double min_val_start = numeric_limits<double>::max();
		double min_val_end = numeric_limits<double>::max();
		for(size_t idx = 0; idx < depot_nodes.size(); idx++){
			if(min_val_end > arcs[v.route[v.route.size() - 1].gen_idx][depot_nodes[idx].gen_idx]){
				min_val_end = arcs[v.route[v.route.size() - 1].gen_idx][depot_nodes[idx].gen_idx];
			}
			if(min_val_start > arcs[depot_nodes[idx].gen_idx][v.route[0].gen_idx]){
				min_val_start = arcs[depot_nodes[idx].gen_idx][v.route[0].gen_idx];
			}
		}
		arc_lengths += min_val_start + min_val_end;
	} else {
		cout << "Error calculating costs for transfer insertion!\n";
	}
	return travel_cost*arc_lengths;
}

double Instance::costs_of_inserting_request_with_transfer_delivery(Vehicle v, size_t p, size_t d, size_t request, Transfer_Node tn) {
	double arc_lengths = 0.0;
	
	if (d == p + 1) {
		arc_lengths += arcs[tn.gen_idx][delivery_nodes[request].gen_idx]
						- arcs[v.route[p-1].gen_idx][v.route[p].gen_idx];
	} else {
		arc_lengths += arcs[tn.gen_idx][v.route[p].gen_idx]
						+ arcs[v.route[d-2].gen_idx][delivery_nodes[request].gen_idx]
						- arcs[v.route[p-1].gen_idx][v.route[p].gen_idx]
						- arcs[v.route[d-2].gen_idx][v.route[d-1].gen_idx];
	}
	
	if (p > 0 && d < v.route.size()-1) {
		arc_lengths += arcs[v.route[p-1].gen_idx][tn.gen_idx] + arcs[delivery_nodes[request].gen_idx][v.route[d-1].gen_idx];
	} else if (p > 0 && d == v.route.size() - 1) {
		// Change ending depot then add 
		arc_lengths += arcs[v.route[p].gen_idx][tn.gen_idx];
		
		double min_val = numeric_limits<double>::max();
		for(size_t idx = 0; idx < depot_nodes.size(); idx++){
			if(min_val > arcs[v.route[v.route.size() - 1].gen_idx][depot_nodes[idx].gen_idx]){
				min_val = arcs[v.route[v.route.size() - 1].gen_idx][depot_nodes[idx].gen_idx];
			}
		}
		arc_lengths += min_val;
	} else if (p == 0 && d < v.route.size() - 1) {
		// Change beginning depot then add arcs
		arc_lengths += arcs[delivery_nodes[request].gen_idx][v.route[d-1].gen_idx];
		
		double min_val = numeric_limits<double>::max();
		for(size_t idx = 0; idx < depot_nodes.size(); idx++){
			if(min_val > arcs[depot_nodes[idx].gen_idx][v.route[0].gen_idx]){
				min_val = arcs[depot_nodes[idx].gen_idx][v.route[0].gen_idx];
			}
		}
	} else if (p == 0 && d == v.route.size() - 1) {
		// Change both depots then add arcs
		
		double min_val_start = numeric_limits<double>::max();
		double min_val_end = numeric_limits<double>::max();
		for(size_t idx = 0; idx < depot_nodes.size(); idx++){
			if(min_val_end > arcs[v.route[v.route.size() - 1].gen_idx][depot_nodes[idx].gen_idx]){
				min_val_end = arcs[v.route[v.route.size() - 1].gen_idx][depot_nodes[idx].gen_idx];
			}
			if(min_val_start > arcs[depot_nodes[idx].gen_idx][v.route[0].gen_idx]){
				min_val_start = arcs[depot_nodes[idx].gen_idx][v.route[0].gen_idx];
			}
		}
		arc_lengths += min_val_start + min_val_end;
	} else {
		cout << "Error calculating costs for transfer insertion!\n";
	}
	return travel_cost*arc_lengths;
}