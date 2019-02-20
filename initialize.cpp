#include "instance.h"

#include <limits>

void Instance::initial_solution(){
	vector<size_t> bank(request_amount);
	for(size_t idx = 0; idx < request_amount; idx++){
		bank[idx] = idx;
	}
	
	size_t bank_not_empty = 1; //still requests that need allocating
	size_t v_idx = 0;
	
	while(bank_not_empty)
	{
		//add vehicle
		routes.push_back(Vehicle());
		
		//look for request with first time window
		double min_window = numeric_limits<double>::max();
		size_t min_index = 0;
		for(size_t idx = 0; idx < bank.size(); idx++){
			if (min_window < pickup_nodes[bank[idx]].lower_bound){
				min_window = pickup_nodes[bank[idx]].lower_bound;
				min_index = bank[idx];
			}
		}
		bank.erase(bank.begin() + min_index);
		routes[v_idx].add_node(routes[v_idx].route.size(), pickup_nodes[min_index]);
		routes[v_idx].add_node(routes[v_idx].route.size(), delivery_nodes[min_index]);
		bank_not_empty = (bank.size() == 0) ? 1 : 0;
		
		//look for nearest depot and insert
		double min_arc = numeric_limits<double>::max();
		size_t min_arc_index = 0;
		for(size_t idx = 0; idx < depot_amount; idx++){
			if(min_arc > arcs[depot_nodes[idx].gen_idx][min_index]){
				min_arc = arcs[depot_nodes[idx].gen_idx][min_index];
				min_arc_index = idx;
			}
		}
		routes[v_idx].add_node(0, depot_nodes[min_arc_index]);
		
		//add different nodes
		size_t adx = 0;
		size_t inserted = 1;
		while(inserted && (bank.size() == 0)){ //add while requests are left, and we added a request last iteration
		//while(inserted && bank_empty){ //add while requests are left, and we added a request last iteration
			adx += 2;
			min_arc = numeric_limits<double>::max();
			for(size_t idx = 0; idx < bank.size(); idx++){
				//check if node is can be reached in time
				if(routes[v_idx].time_at_node[adx] + routes[v_idx].route[adx].service_time + arcs[routes[0].route[adx].gen_idx][pickup_nodes[idx].gen_idx] > pickup_nodes[idx].upper_bound){
					//find nearest neighbour
					if(min_arc > arcs[routes[v_idx].route[adx].gen_idx][pickup_nodes[idx].gen_idx]){
						min_arc = arcs[routes[v_idx].route[adx].gen_idx][pickup_nodes[idx].gen_idx];
						min_arc_index = idx;
					}
				}
			}
			//If the minimum arc is unchanged (no nodes could be reached) we have not inserted
			if(min_arc >= numeric_limits<double>::max()){
				inserted = 0;
			}
			//remove node from bank, and add pickup and delivery to route
			bank.erase(bank.begin() + min_arc_index);
			routes[v_idx].add_node(routes[v_idx].route.size(), pickup_nodes[min_arc_index]);
			routes[v_idx].add_node(routes[v_idx].route.size(), delivery_nodes[min_arc_index]);
			bank_not_empty = (bank.size() == 0) ? 1 : 0;
		}
		v_idx++;
	}
}
