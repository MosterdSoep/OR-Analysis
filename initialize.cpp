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
				min_index = idx;
			}
		}

		routes[v_idx].add_node(1, pickup_nodes[bank[min_index]]);
		if(routes[v_idx].time_at_node[1] + pickup_nodes[bank[min_index]].service_time +
            arcs[pickup_nodes[bank[min_index]].gen_idx][delivery_nodes[bank[min_index]].gen_idx] < delivery_nodes[bank[min_index]].lower_bound){
                routes[v_idx].waiting_times[1] = delivery_nodes[bank[min_index]].lower_bound - routes[v_idx].time_at_node[1] - pickup_nodes[bank[min_index]].service_time -
                arcs[pickup_nodes[bank[min_index]].gen_idx][delivery_nodes[bank[min_index]].gen_idx];
            }
		routes[v_idx].add_node(2, delivery_nodes[bank[min_index]]);
        bank.erase(bank.begin() + min_index);
		bank_not_empty = (bank.size() > 0) ? 1 : 0;
		//add different nodes
		size_t adx = 0;
		size_t inserted = 1;
		size_t min_arc_index = 0;
		double min_arc = 0;

		while(inserted && bank_not_empty){ //add while requests are left, and we added a request last iteration
		//while(inserted && bank_empty){ //add while requests are left, and we added a request last iteration
			adx += 2;
			min_arc = numeric_limits<double>::max();
			for(size_t idx = 0; idx < bank.size(); idx++){
				//check if node is can be reached in time
				if(routes[v_idx].time_at_node[adx] + routes[v_idx].route[adx].service_time + arcs[routes[v_idx].route[adx].gen_idx][pickup_nodes[bank[idx]].gen_idx] <= pickup_nodes[bank[idx]].upper_bound){
					//find nearest neighbour
					if(min_arc > arcs[routes[v_idx].route[adx].gen_idx][pickup_nodes[bank[idx]].gen_idx]){
						min_arc = arcs[routes[v_idx].route[adx].gen_idx][pickup_nodes[bank[idx]].gen_idx];
						min_arc_index = idx;
					}
				}
			}
			//If the minimum arc is unchanged (no nodes could be reached) we have not inserted
			if(min_arc >= numeric_limits<double>::max()){
				inserted = 0;
			}else{
			//remove node from bank, and add pickup and delivery to route

			routes[v_idx].add_node(routes[v_idx].route.size()-1, pickup_nodes[bank[min_arc_index]]);
            if(routes[v_idx].time_at_node[routes[v_idx].route.size()-1] + pickup_nodes[bank[min_arc_index]].service_time +
            arcs[pickup_nodes[bank[min_arc_index]].gen_idx][delivery_nodes[bank[min_arc_index]].gen_idx] < delivery_nodes[bank[min_arc_index]].lower_bound){
                routes[v_idx].waiting_times[routes[v_idx].route.size()-1] = delivery_nodes[bank[min_arc_index]].lower_bound - routes[v_idx].time_at_node[routes[v_idx].route.size()-1] - pickup_nodes[bank[min_arc_index]].service_time -
                arcs[pickup_nodes[bank[min_arc_index]].gen_idx][delivery_nodes[bank[min_arc_index]].gen_idx];
            }
			routes[v_idx].add_node(routes[v_idx].route.size()-1, delivery_nodes[bank[min_arc_index]]);
			bank.erase(bank.begin() + min_arc_index);
			bank_not_empty = (bank.size() > 0) ? 1 : 0;

			}
		}
		v_idx++;
	}

}
