#include "instance.h"

#include <limits>

void Instance::create_instance(vector<vector<int>> &input_data, int ins){
    index = input_data[ins][0];
    request_amount = input_data[ins][1];
    transfer_location_amount = input_data[ins][2];
    depot_amount = input_data[ins][3];
    vehicle_capacity = input_data[ins][4];
    travel_cost = input_data[ins][5];
    node_amount = 2 * request_amount + transfer_location_amount + depot_amount;

	// Initialize request arrays with request_amount as size
	pickup_vehicle.resize(request_amount, 0);
	delivery_vehicle.resize(request_amount, 0);

    size_t transfer_cost_idx = 6;
    size_t coordinate_idx = transfer_cost_idx + transfer_location_amount;
    size_t time_window_idx = coordinate_idx + 2 * node_amount;
    size_t ride_times_idx = time_window_idx + 4 * request_amount;
    size_t service_time_idx = ride_times_idx + request_amount;

    for(int idx = 0; idx < request_amount; idx++){
        // Create pickup nodes
        pickup_nodes.push_back(Pickup_Node());
        pickup_nodes[idx].index = idx;
        pickup_nodes[idx].x = input_data[ins][coordinate_idx + 2 * idx];
        pickup_nodes[idx].y = input_data[ins][coordinate_idx + 2 * idx + 1];
        pickup_nodes[idx].service_time = input_data[ins][service_time_idx + idx];
        pickup_nodes[idx].gen_idx = idx;
        pickup_nodes[idx].lower_bound = input_data[ins][time_window_idx + 2* idx];
        pickup_nodes[idx].upper_bound = input_data[ins][time_window_idx + 2* idx + 1];
        pickup_nodes[idx].type = 'p';


        // Create delivery nodes
        delivery_nodes.push_back(Delivery_Node());
        delivery_nodes[idx].index = idx;
        delivery_nodes[idx].x = input_data[ins][coordinate_idx + 2 * request_amount + 2 * idx];
        delivery_nodes[idx].y = input_data[ins][coordinate_idx + 2 * request_amount + 2 * idx + 1];
        delivery_nodes[idx].service_time = input_data[ins][service_time_idx + request_amount + idx];
        delivery_nodes[idx].gen_idx = request_amount + idx;
        delivery_nodes[idx].lower_bound = input_data[ins][time_window_idx + 2 * request_amount + 2 * idx];
        delivery_nodes[idx].upper_bound = input_data[ins][time_window_idx + 2 * request_amount + 2 * idx + 1];
        delivery_nodes[idx].type = 'd';
    }

    for(int idx = 0; idx < transfer_location_amount; idx++){
        //Create transfer nodes
        transfer_nodes.push_back(Transfer_Node());
        transfer_nodes[idx].index = idx;
        transfer_nodes[idx].x = input_data[ins][coordinate_idx + 4 * request_amount + 2 * idx];
        transfer_nodes[idx].y = input_data[ins][coordinate_idx + 4 * request_amount + 2 * idx + 1];
        transfer_nodes[idx].service_time = input_data[ins][service_time_idx + 2 * request_amount + idx];
        transfer_nodes[idx].gen_idx = 2 * request_amount + idx;
        transfer_nodes[idx].costs = input_data[ins][transfer_cost_idx + idx];
        transfer_nodes[idx].type = 't';
    }

    for(int idx = 0; idx < depot_amount; idx++){
        //Create depot nodes
        depot_nodes.push_back(Depot_Node());
        depot_nodes[idx].index = idx;
        depot_nodes[idx].x = input_data[ins][coordinate_idx + 4 * request_amount + 2 * transfer_location_amount + 2 * idx];
        depot_nodes[idx].y = input_data[ins][coordinate_idx + 4 * request_amount + 2 * transfer_location_amount + 2 * idx + 1];
        depot_nodes[idx].service_time = 0;
        depot_nodes[idx].gen_idx = 2 * request_amount + transfer_location_amount + idx;
        depot_nodes[idx].type = 'o';
    }

    for(int idx = 0; idx < request_amount; idx++){
        ride_times.push_back(input_data[ins][ride_times_idx + idx]);
    }

    all_nodes.insert(all_nodes.end(), pickup_nodes.begin(), pickup_nodes.end());
    all_nodes.insert(all_nodes.end(), delivery_nodes.begin(), delivery_nodes.end());
    all_nodes.insert(all_nodes.end(), transfer_nodes.begin(), transfer_nodes.end());
    all_nodes.insert(all_nodes.end(), depot_nodes.begin(), depot_nodes.end());
}

void Instance::preprocess(){
    for(int idx = 0; idx < request_amount; idx++){
        // e_i+n < e_i + s_i + t_i,i+n
        if(delivery_nodes[idx].lower_bound < pickup_nodes[idx].lower_bound + pickup_nodes[idx].service_time + arcs[idx][request_amount + idx])
            delivery_nodes[idx].lower_bound = pickup_nodes[idx].lower_bound + pickup_nodes[idx].service_time + arcs[idx][request_amount + idx];
        // l_i > l_i+n - s_i - t_i,i+n
        if(pickup_nodes[idx].upper_bound > delivery_nodes[idx].upper_bound - pickup_nodes[idx].service_time - arcs[idx][request_amount + idx])
            pickup_nodes[idx].upper_bound = delivery_nodes[idx].upper_bound - pickup_nodes[idx].service_time - arcs[idx][request_amount + idx];

        if(pickup_nodes[idx].lower_bound + pickup_nodes[idx].service_time < delivery_nodes[idx].lower_bound - ride_times[idx])
            pickup_nodes[idx].lower_bound =	 delivery_nodes[idx].lower_bound - ride_times[idx] - pickup_nodes[idx].service_time ;

        if(pickup_nodes[idx].upper_bound + pickup_nodes[idx].service_time < delivery_nodes[idx].upper_bound - ride_times[idx])
            delivery_nodes[idx].upper_bound = pickup_nodes[idx].upper_bound + ride_times[idx] + pickup_nodes[idx].service_time;
        // no delivery before its pickup
        arcs[idx + request_amount][idx] = numeric_limits<double>::max();

        // no routes directly from depot to delivery, or pickup to depot
        for(int adx = depot_nodes[0].gen_idx; adx < depot_nodes[0].gen_idx + depot_amount; adx++){
            arcs[adx][idx + request_amount] = numeric_limits<double>::max();
            arcs[idx][adx] = numeric_limits<double>::max();
        }
    }
    for(int idx = 0; idx < request_amount; idx++){
        for(int adx = 0; adx < request_amount; adx++){
            if(pickup_nodes[idx].lower_bound + arcs[idx][adx] > pickup_nodes[adx].upper_bound){
                arcs[idx][adx] = numeric_limits<double>::max();
            }
            if(pickup_nodes[idx].lower_bound + arcs[idx][adx] > delivery_nodes[adx].upper_bound){
                arcs[idx][adx] = numeric_limits<double>::max();
            }
            if(delivery_nodes[idx].lower_bound + arcs[idx][adx] > pickup_nodes[adx].upper_bound){
                arcs[idx][adx] = numeric_limits<double>::max();
            }
            if(delivery_nodes[idx].lower_bound + arcs[idx][adx] > delivery_nodes[adx].upper_bound){
                arcs[idx][adx] = numeric_limits<double>::max();
            }
        }
    }
    for(int idx = 0; idx < request_amount; idx++){
        if(pickup_nodes[idx].lower_bound > pickup_nodes[idx].upper_bound){cout << "Time window pickups infeasible\n";}
        if(delivery_nodes[idx].lower_bound > delivery_nodes[idx].upper_bound){cout << "Time window deliveries infeasible\n";}
    }
}

void Instance::calculate_arcs() {
	arcs.resize(node_amount);
	for (int i = 0; i < node_amount; i++) {
		arcs[i].resize(node_amount);
	}
	for (int i = 0; i < node_amount; i++) {
		for (int j = i + 1; j < node_amount; j++) {
			arcs[i][j] = euclidian_distance(all_nodes[i],all_nodes[j]);
			arcs[j][i] = arcs[i][j];
		}
	}
}

double Instance::calculate_obj_val() {
	double total_distance = 0;
	for (Vehicle v : routes) {
		total_distance += accumulate(v.arc_durations.begin(),v.arc_durations.end(),0);
	}
	double facility_costs = 0;
	for (Transfer_Node node : transfer_nodes) {
		if (node.open) {
			facility_costs += node.costs;
		}
	}
	return travel_cost*total_distance + facility_costs;
}