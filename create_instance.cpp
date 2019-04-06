#include "instance.h"

#include <limits>
#include <algorithm>

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

    for(size_t idx = 0; idx < request_amount; idx++){
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
        pickup_nodes[idx].request_idx = idx;


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
        delivery_nodes[idx].request_idx = idx;
    }

    for(size_t idx = 0; idx < transfer_location_amount; idx++){
        //Create transfer nodes
        transfer_nodes.push_back(Transfer_Node());
        transfer_nodes[idx].index = idx;
        transfer_nodes[idx].x = input_data[ins][coordinate_idx + 4 * request_amount + 2 * idx];
        transfer_nodes[idx].y = input_data[ins][coordinate_idx + 4 * request_amount + 2 * idx + 1];
        transfer_nodes[idx].service_time = input_data[ins][service_time_idx + 2 * request_amount + idx];
        transfer_nodes[idx].gen_idx = 2 * request_amount + idx;
        transfer_nodes[idx].costs = input_data[ins][transfer_cost_idx + idx];
        transfer_nodes[idx].type = 't';
        transfer_nodes[idx].open = 0;
    }

    for(size_t idx = 0; idx < depot_amount; idx++){
        //Create depot nodes
        depot_nodes.push_back(Depot_Node());
        depot_nodes[idx].index = idx;
        depot_nodes[idx].x = input_data[ins][coordinate_idx + 4 * request_amount + 2 * transfer_location_amount + 2 * idx];
        depot_nodes[idx].y = input_data[ins][coordinate_idx + 4 * request_amount + 2 * transfer_location_amount + 2 * idx + 1];
        depot_nodes[idx].service_time = 0;
        depot_nodes[idx].gen_idx = 2 * request_amount + transfer_location_amount + idx;
        depot_nodes[idx].type = 'o';
    }

    for(size_t idx = 0; idx < request_amount; idx++){
        ride_times.push_back(input_data[ins][ride_times_idx + idx]);
    }



    all_nodes.insert(all_nodes.end(), pickup_nodes.begin(), pickup_nodes.end());
    all_nodes.insert(all_nodes.end(), delivery_nodes.begin(), delivery_nodes.end());
    all_nodes.insert(all_nodes.end(), transfer_nodes.begin(), transfer_nodes.end());
    all_nodes.insert(all_nodes.end(), depot_nodes.begin(), depot_nodes.end());
}

// -----------------------------------
// HAS TO BE DONE BEFORE PREPROCESSING
// -----------------------------------
vector<double> Instance::facility_weights() {
	vector<double> averages;
	for (size_t t = 0; t < transfer_nodes.size(); t++) {
		vector<double> distances;
		for (size_t n = 0; n < all_nodes.size(); n++) {
			distances.push_back(arcs[transfer_nodes[t].gen_idx][all_nodes[n].gen_idx]);
		}
		averages.push_back(accumulate(distances.begin(), distances.end(), 0.0)/distances.size());
	}
	double normalizing_constant = accumulate(averages.begin(), averages.end(), 0.0);
	for (size_t i = 0; i < averages.size(); i++) {
		averages[i] /= normalizing_constant;
		averages[i] = 1/(averages[i]*averages[i]);
	}
	
	return averages;
}

void Instance::preprocess(){
    double big_M = 50000000000;

    for(size_t idx = 0; idx < request_amount; idx++){
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
        arcs[idx + request_amount][idx] = big_M;

        // no routes directly from depot to delivery, or pickup to depot
        for(size_t adx = depot_nodes[0].gen_idx; adx < depot_nodes[0].gen_idx + depot_amount; adx++){
            arcs[adx][idx + request_amount] = big_M;
            arcs[idx][adx] = big_M;
        }
    }
    for(size_t idx = 0; idx < request_amount; idx++){
        for(size_t adx = 0; adx < request_amount; adx++){
            if(pickup_nodes[idx].lower_bound + arcs[idx][adx] + pickup_nodes[idx].service_time > pickup_nodes[adx].upper_bound){
                arcs[idx][adx] = big_M;
            }
            if(pickup_nodes[idx].lower_bound + arcs[idx][adx] + pickup_nodes[idx].service_time > delivery_nodes[adx].upper_bound){
                arcs[idx][adx] = big_M;
            }
            if(delivery_nodes[idx].lower_bound + arcs[idx][adx] + pickup_nodes[idx].service_time > pickup_nodes[adx].upper_bound){
                arcs[idx][adx] = big_M;
            }
            if(delivery_nodes[idx].lower_bound + arcs[idx][adx] + pickup_nodes[idx].service_time > delivery_nodes[adx].upper_bound){
                arcs[idx][adx] = big_M;
            }
        }
    }
    for(size_t idx = 0; idx < request_amount; idx++){
        if(pickup_nodes[idx].lower_bound > pickup_nodes[idx].upper_bound){cout << "Time window pickups infeasible\n";}
        if(delivery_nodes[idx].lower_bound > delivery_nodes[idx].upper_bound){cout << "Time window deliveries infeasible\n";}
    }
}

void Instance::calculate_arcs() {
	arcs.resize(node_amount);
	for (size_t i = 0; i < node_amount; i++) {
		arcs[i].resize(node_amount);
	}
	double max_length = 0;
	for (size_t i = 0; i < node_amount; i++) {
		for (size_t j = i + 1; j < node_amount; j++) {
			arcs[i][j] = euclidian_distance(all_nodes[i],all_nodes[j]);
			arcs[j][i] = arcs[i][j];
			if (i == 0 && j == 1) { max_length = arcs[j][i]; }
			if (arcs[i][j] > max_length) { max_length = arcs[i][j]; }
		}
	}
	alpha = max_length;

    for(size_t idx = 0; idx < request_amount; idx++){
        nearest_depot_gen_idx_p.push_back(min_element(arcs[pickup_nodes[idx].gen_idx].begin() + depot_nodes[0].gen_idx, arcs[pickup_nodes[idx].gen_idx].begin() +
                                depot_nodes[0].gen_idx + depot_nodes.size()) - arcs[pickup_nodes[idx].gen_idx].begin());
        nearest_depot_gen_idx_d.push_back(min_element(arcs[delivery_nodes[idx].gen_idx].begin() + depot_nodes[0].gen_idx, arcs[delivery_nodes[idx].gen_idx].begin() +
                                depot_nodes[0].gen_idx + depot_nodes.size()) - arcs[delivery_nodes[idx].gen_idx].begin());
        nearest_depot_insertion_cost.push_back( (arcs[pickup_nodes[idx].gen_idx][delivery_nodes[idx].gen_idx] + arcs[nearest_depot_gen_idx_p[idx]][pickup_nodes[idx].gen_idx] +
                                               arcs[delivery_nodes[idx].gen_idx][nearest_depot_gen_idx_d[idx]] ) * travel_cost );
    }
    for(size_t idx = 0; idx < transfer_location_amount; idx++){
        nearest_depot_gen_idx_t.push_back(min_element(arcs[transfer_nodes[idx].index].begin() + depot_nodes[0].gen_idx, arcs[transfer_nodes[idx].index].begin() +
                                depot_nodes[0].gen_idx + depot_nodes.size()) - arcs[transfer_nodes[idx].index].begin());
    }
}

double Instance::calculate_obj_val() {
	double total_distance = 0.0;
	for (Vehicle v : routes) {
		for (size_t i = 0; i < v.route.size() - 1; i++) {
			total_distance += arcs[v.route[i].gen_idx][v.route[i + 1].gen_idx];
		}
	}
	double facility_costs = 0;
	for (size_t i = 0; i < transfer_nodes.size(); i++) {
		if (transfer_nodes[i].open) {
			//cout << "transfer node open hence costs of: " << transfer_nodes[i].costs << "\n";
			facility_costs +=  transfer_nodes[i].costs;
		}
	}
	return travel_cost*total_distance + facility_costs;
}
