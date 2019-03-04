#include "vehicle.h"
#include "instance.h"

#include <limits>

using namespace std;

//defined here so vehicles can reach this global variable
vector<vector<double>> arcs = {};
vector<Pickup_Node> pickup_nodes = {};
vector<Delivery_Node> delivery_nodes = {};
vector<Transfer_Node> transfer_nodes = {};
vector<Depot_Node> depot_nodes = {};
vector<Node> all_nodes = {};
vector<size_t> pickup_vehicle = {};
vector<size_t> delivery_vehicle = {};

Vehicle::Vehicle(){ //vehicles get initialized with a random depot node
    route.push_back(depot_nodes[0]);
    route.push_back(depot_nodes[0]);
}

void Vehicle::change_first_depot(){ //automatically change first depot to optimal one
    route.erase(route.begin());
    size_t min_idx = 0;
    double min_val = numeric_limits<double>::max();
    for(size_t idx = 0; idx < depot_nodes.size(); idx++){
        if(min_val > arcs[depot_nodes[idx].gen_idx][route[0].gen_idx]){
            min_val = arcs[depot_nodes[idx].gen_idx][route[0].gen_idx];
            min_idx = idx;
        }
    }
    route.insert(route.begin(), depot_nodes[min_idx]);
}

void Vehicle::change_last_depot(){ //automatically changes last depot to optimal one
    route.erase(route.end());
    size_t min_idx = 0;
    double min_val = numeric_limits<double>::max();
    for(size_t idx = 0; idx < depot_nodes.size(); idx++){
        if(min_val > arcs[route[route.size() - 1].gen_idx][depot_nodes[idx].gen_idx]){
            min_val = arcs[route[route.size() - 1].gen_idx][depot_nodes[idx].gen_idx];
            min_idx = idx;
        }
    }
    route.insert(route.end(), depot_nodes[min_idx]);
}

void Vehicle::add_node(size_t location, Pickup_Node &node){
    route.insert(route.begin() + location, node);
    if (location == 1){
        change_first_depot();
    }
    if (location == route.size() - 2){
        change_last_depot();
    }

    arc_durations.insert(arc_durations.begin() + location - 1, arcs[route[location - 1].gen_idx][node.gen_idx]);
    arc_durations[location] = arcs[node.gen_idx][route[location + 1].gen_idx];

    //update capacity and times
    current_capacity.insert(current_capacity.begin() + location, current_capacity[location - 1]);
    waiting_times.insert(waiting_times.begin() + location, 0);
    time_at_node.insert(time_at_node.begin() + location, 0);
    for(size_t idx = location; idx < route.size(); idx++){
        current_capacity[idx]++;
        time_at_node[idx] = time_at_node[idx - 1] + arc_durations[idx - 1] + route[idx - 1].service_time + waiting_times[idx - 1];
        if(time_at_node[idx] < route[idx].lower_bound){
            waiting_times[idx] = route[idx].lower_bound - time_at_node[idx];
        }
    }
	pickup_vehicle[node.index] = this->index;
}

void Vehicle::add_node(size_t location, Delivery_Node &node){
    route.insert(route.begin() + location, node);
    if (location == 1){
        change_first_depot();
    }
    if (location == route.size() - 2){
        change_last_depot();
    }
    arc_durations.insert(arc_durations.begin() + location - 1, arcs[route[location - 1].gen_idx][node.gen_idx]);
    arc_durations[location] = arcs[node.gen_idx][route[location + 1].gen_idx];
	
    //update capacity and times
    current_capacity.insert(current_capacity.begin() + location, current_capacity[location - 1]);
    waiting_times.insert(waiting_times.begin() + location, 0);
    time_at_node.insert(time_at_node.begin() + location, 0);
    for(size_t idx = location; idx < route.size(); idx++){
        current_capacity[idx]--;
        time_at_node[idx] = time_at_node[idx - 1] + arc_durations[idx - 1] + route[idx - 1].service_time + waiting_times[idx - 1];
        if(time_at_node[idx] < route[idx].lower_bound){
            waiting_times[idx] = route[idx].lower_bound - time_at_node[idx];
        }
    }
	delivery_vehicle[node.index] = this->index;
}

double Vehicle::add_delivery_transfer(size_t location, Transfer_Node &node){
    //Creates fictional new node
    Transfer_Node fictional_node = node;
    fictional_node.pickup = 0;

    route.insert(route.begin() + location, fictional_node);
    if (location == 1){
        change_first_depot();
    }
    if (location == route.size() - 2){
        change_last_depot();
    }
    arc_durations.insert(arc_durations.begin() + location - 1, arcs[route[location - 1].gen_idx][fictional_node.gen_idx]);
    arc_durations[location] = arcs[fictional_node.gen_idx][route[location + 1].gen_idx];

    //update capacity and times
    current_capacity.insert(current_capacity.begin() + location, current_capacity[location - 1]);
    waiting_times.insert(waiting_times.begin() + location, 0);
    time_at_node.insert(time_at_node.begin() + location, 0);
    for(size_t idx = location; idx < route.size(); idx++){
        current_capacity[idx]--;
        time_at_node[idx] = time_at_node[idx - 1] + arc_durations[idx - 1] + route[idx - 1].service_time + waiting_times[idx - 1];
        if(time_at_node[idx] < route[idx].lower_bound){
            waiting_times[idx] = route[idx].lower_bound - time_at_node[idx];
        }
    }
    return time_at_node[location] + node.service_time; //returns the time at which pickup can start
}

void Vehicle::add_pickup_transfer(size_t location, Transfer_Node &node, double min_time){
    //Creates fictional new node
    Transfer_Node fictional_node = node;
    fictional_node.pickup = 1;
    fictional_node.lower_bound = min_time; //update the lower bound for pickup to the time at which the transfer is ready

    route.insert(route.begin() + location, fictional_node);
    if (location == 1){
        change_first_depot();
    }
    if (location == route.size() - 2){
        change_last_depot();
    }
    arc_durations.insert(arc_durations.begin() + location - 1, arcs[route[location - 1].gen_idx][fictional_node.gen_idx]);
    arc_durations[location] = arcs[fictional_node.gen_idx][route[location + 1].gen_idx];

    //update capacity and times
    current_capacity.insert(current_capacity.begin() + location, current_capacity[location - 1]);
    waiting_times.insert(waiting_times.begin() + location, 0);
    time_at_node.insert(time_at_node.begin() + location, 0);
    for(size_t idx = location; idx < route.size(); idx++){
        current_capacity[idx]++;
        time_at_node[idx] = time_at_node[idx - 1] + arc_durations[idx - 1] + route[idx - 1].service_time + waiting_times[idx - 1];
        if(time_at_node[idx] < route[idx].lower_bound){
            waiting_times[idx] = route[idx].lower_bound - time_at_node[idx];
        }
    }
}

void Vehicle::remove_node(size_t location){
	arc_durations[location] = arcs[route[location - 1].gen_idx][route[location + 1].gen_idx];
	arc_durations.erase(arc_durations.begin() + location - 1);
	waiting_times.erase(waiting_times.begin() + location); // Waiting times not optimal, could possibly wait less in other nodes but that may change feasibility

	// Changing the capacity of the vehicle depends on the node that is being removed
	if (route[location].type == 'p' || (route[location].pickup && route[location].type == 't')) {
		// Pickup will be removed
        for (size_t i = location + 1; i < current_capacity.size(); i++) {
			current_capacity[i]--;
		}
    } else if (route[location].type == 'd' || (!route[location].pickup && route[location].type == 't')) {
		// Delivery will be removed
		for (size_t i = location + 1; i < current_capacity.size(); i++) {
			current_capacity[i]++;
		}
	} else if (route[location].type == 'o') {
        cout << "ERROR: Removing depot node!\n";
	} else {
		cout << "ERROR: Removing non pickup, non delivery and non transfer node!\n";
	}
	route.erase(route.begin() + location);

    for(size_t idx = location; idx < route.size(); idx++){
        time_at_node[idx] = time_at_node[idx - 1] + arc_durations[idx - 1] + route[idx - 1].service_time + waiting_times[idx - 1];
        if(time_at_node[idx] < route[idx].lower_bound){
            waiting_times[idx] = route[idx].lower_bound - time_at_node[idx];
        }
    }
};