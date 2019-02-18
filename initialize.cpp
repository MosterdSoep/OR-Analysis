#include "instance.h"

#include <limits>

void Instance::initial_solution(int request_amount){
	vector<size_t> bank(request_amount);
	for(size_t idx = 0; idx < request_amount; idx++){
		bank[idx] = idx;
	}
	
	routes.push_back(Vehicle());
	double min_window = numeric_limits<double>::max();
	size_t min_index;
	for(size_t idx = 0; idx < bank.size(); idx++){
		if (min_window < pickup_nodes[bank[idx]].lower_bound){
			min_window = pickup_nodes[bank[idx]].lower_bound;
			min_index = bank[idx];
		}
	}
	bank.remove(min_index);
	
	//Add nearest starting depot
	
	routes[0].route.push_back(pickup_nodes[min_idx])
	
	double min_arc = numeric_limits<double>::max();
	size_t min_arc_index;
	for(size_t idx = 0; idx < depot_amount; idx++){
		if(min_arc > arcs[depot_nodes[idx].gen_idx][min_index])
		{
			min_arc = arcs[depot_nodes[idx].gen_idx][min_index];
			min_arc_index = idx;
		}
	}
	routes[0].route.insert(routes[0].route.insert.begin(), depot_nodes[idx]);
}