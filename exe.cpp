#include <fstream>
#include <sstream>
#include <cmath>
#include <random>
#include <numeric>
#include "instance.h"
#include <chrono>

using namespace std;

// General variables
//string location = "D://Documenten//Studie//OR analysis//OR Analysis//instances.csv";
//string location = "C://Users//Hp//Documents//GitHub//OR-Analysis//instances.csv";
//string location = "C://Users//Luuk//Documents//Codeblocks projecten//OR_analysis//large_instances.csv";
string location = "large_instances.csv";
vector<vector<int>> input_data;
vector<double> transfer_weights;
double start_temperature = 5.0;
size_t maximum_loops = 10000;

bool acceptation_criterion_met(double s, double current_solution, size_t loop_count) {
	double temperature = start_temperature * ((double)maximum_loops - (double)loop_count) / ((double)maximum_loops/10);
	double minimum = min(current_solution - s,0.0);
	double probability = (double)exp(minimum/temperature);
	double percentage = probability*100;
	double outcome = rand() % 101;
	if (outcome < percentage) {
		return true;
	} else {
		return false;
	}
}

bool stopping_criterion_met(size_t loop_count) {
	if (loop_count < maximum_loops) return false;
	else return true;
}
/*
bool time_based_criterion(auto start){
    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end_time - start;
    if(elapsed.count() > 60){
        return true;
    }
    return false;
}*/

void ALNS(Instance &i) {
	auto start = chrono::high_resolution_clock::now();
	
	double best_solution = i.calculate_obj_val();
	double current_solution = best_solution;
	vector<Vehicle> best_routes = i.routes;
	
	vector<vector<Vehicle>> best_routes_all (transfer_nodes.size() + 1, best_routes);
	vector<vector<Vehicle>> current_solution_routes (transfer_nodes.size() + 1, best_routes);
	vector<vector<size_t>> pickup_vehicle_all (transfer_nodes.size() + 1, pickup_vehicle);
	vector<vector<size_t>> delivery_vehicle_all (transfer_nodes.size() + 1, delivery_vehicle);
	vector<vector<bool>> transfer_nodes_opened (transfer_nodes.size() + 1, {true});
	cout << "transfer size: " << transfer_nodes.size() << "\n";
	for (size_t i = 0; i < transfer_nodes.size() + 1; i++) {
		transfer_nodes_opened[i].clear();
		for (size_t j = 0; j < i; j++) {
			transfer_nodes_opened[i].push_back(true);
		}
		for (size_t j = i; j < transfer_nodes.size(); j++) {
			transfer_nodes_opened[i].push_back(false);
		}
	}
	for (size_t t = 0;  t < transfer_nodes.size() + 1; t++) {
		for (size_t i = 0; i < transfer_nodes.size(); i++) {
			cout << transfer_nodes_opened[t][i] << " ";
		}
		cout << "\n";
	}
	size_t loop_count = 0;

	i.print_routes();
	for (size_t i = 0; i < transfer_nodes.size(); i++) {
			transfer_nodes[i].open = false;
	}
	cout << "Objective value: " << i.calculate_obj_val() << "\n";
    double initialval = i.calculate_obj_val();
	random_device rd;
	mt19937 gen(rd());

    double costs_rejection = 0;
	double feasibility_rejection = 0;
	
	size_t transfer_loop_count = 0;
	vector<double> transfer_scores;
	vector<double> transfer_rewards;
	vector<double> transfer_scores_normal;
	for (size_t i = 0; i < transfer_nodes.size() + 1; i++) {
		transfer_scores.push_back(1);
		transfer_rewards.push_back(0);
		transfer_scores_normal.push_back(transfer_nodes.size() + 1 - i);
	}

	vector<double> deletion_scores = {1,1,1,1,1};
	vector<double> deletion_rewards = {0,0,0,0,0};
	vector<double> deletion_scores_normal = {1,1,1,1,1};

	vector<double> insertion_scores = {1,1,1,1};
	vector<double> insertion_rewards = {0,0,0,0};
	vector<double> insertion_scores_normal = {1,1,1,1};

	vector<size_t> operator_count(deletion_scores.size()*insertion_scores.size(), 0);
	vector<size_t> deletion_count(deletion_scores.size(),0);
	vector<size_t> insertion_count(insertion_scores.size(), 0);
	vector<size_t> transfer_count_vector(transfer_scores.size(), 0);
	
	vector<vector<size_t>> interactive_all(4, {0,0,0,0,0});
	vector<vector<size_t>> interactive_acc1(4, {0,0,0,0,0});
	vector<vector<size_t>> interactive_acc2(4, {0,0,0,0,0});
	vector<vector<size_t>> interactive_acc3(4, {0,0,0,0,0});

	vector<double> op_time = {0,0,0,0};
	vector<double> op_del_time = {0,0,0,0,0};

	vector<size_t> request_bank = {};

	size_t transfer_count = 0;
	size_t transfer_cost_accept = 0;
	size_t transfer_accept = 0;
	
	double percentage = 0;
	vector<double> best_results;
	size_t transfer_operator = 0;
	while(!stopping_criterion_met(loop_count)) {// && !time_based_criterion(start)) {
		i.old_routes.clear();
		i.old_routes = i.routes;
		i.old_pickup_vehicle = pickup_vehicle;
		i.old_delivery_vehicle = delivery_vehicle;
		
		loop_count++;
		if (loop_count % 1000 == 0) {
			for (size_t i = 0; i < deletion_scores.size(); i++) {
				deletion_scores[i] += deletion_rewards[i];
				deletion_rewards[i] = 0;
				deletion_scores_normal[i] = deletion_scores[i] / (1 + deletion_count[i]);
			}
			for (size_t i = 0; i < insertion_scores.size(); i++) {
				insertion_scores[i] += insertion_rewards[i];
				insertion_rewards[i] = 0;
				insertion_scores_normal[i] = insertion_scores[i] / (1 + insertion_count[i]);
			}
		}
		
		// Choosing the amount of transfer facilities to be open
		if (loop_count % (maximum_loops/100) == 0) {
			percentage+=1;
			cout << percentage << "% completed\n";
		}
		
		if (loop_count % 100 == 0) {
			// Save information before changing the amount of transfer facilities opened
			best_routes_all[transfer_operator].clear();
			best_routes_all[transfer_operator] = best_routes;
			current_solution_routes[transfer_operator].clear();
			current_solution_routes[transfer_operator] = i.routes;
			pickup_vehicle_all[transfer_operator].clear();
			pickup_vehicle_all[transfer_operator] = pickup_vehicle;
			delivery_vehicle_all[transfer_operator].clear();
			delivery_vehicle_all[transfer_operator] = delivery_vehicle;
			//cout << "previous amount of transfers: " << transfer_operator << "\n";
			for (size_t i = 0; i < transfer_nodes.size(); i++) {
				transfer_nodes_opened[transfer_operator][i] = transfer_nodes[i].open;
				//cout << transfer_nodes_opened[transfer_operator][i] << " ";
			}
			//cout << "\n";
			
			// Change the amount of transfer facilities opened
			discrete_distribution<size_t> transfer_op(transfer_scores_normal.begin(), transfer_scores_normal.end());
			transfer_operator = transfer_op(gen);
			//cout << "Opening " << transfer_operator << " amount of transfer facilities\n";
			
			// Load the new information of the new amount of transfer facilities opened
			best_routes.clear();
			best_routes = best_routes_all[transfer_operator];
			i.routes.clear();
			i.routes = best_routes;
			best_solution = i.calculate_obj_val();
			i.routes.clear();
			i.old_routes.clear();
			i.old_routes = current_solution_routes[transfer_operator];
			i.routes = i.old_routes;
			current_solution = i.calculate_obj_val();
			i.old_pickup_vehicle.clear();
			i.old_pickup_vehicle = pickup_vehicle_all[transfer_operator];
			pickup_vehicle.clear();
			pickup_vehicle = i.old_pickup_vehicle;
			i.old_delivery_vehicle.clear();
			i.old_delivery_vehicle = delivery_vehicle_all[transfer_operator];
			delivery_vehicle.clear();
			delivery_vehicle = i.old_delivery_vehicle;
			//cout << "new amount of transfers: " << transfer_operator << "\n";
			for (size_t i = 0; i < transfer_nodes.size(); i++) {
				transfer_nodes[i].open = transfer_nodes_opened[transfer_operator][i];
				//cout << transfer_nodes_opened[transfer_operator][i] << " ";
			}
			//cout << "\n";
			
			transfer_loop_count++;
			if (transfer_loop_count % 100 == 0){
				// Update weights
				//cout << "Updating weights\n";
				for (size_t i = 0; i < transfer_scores.size(); i++) {
					//cout << "transfer reward: " << transfer_rewards[i] << "\n";
					transfer_scores[i] += transfer_rewards[i];
					transfer_rewards[i] = 0;
					transfer_scores_normal[i] = transfer_scores[i] / (1 + transfer_count_vector[i]);
				}
			}
		}
		
		size_t amount = (rand() % ((int)(i.request_amount/25))) + 1;
		assert(amount < i.request_amount);
		discrete_distribution<> delete_op(deletion_scores_normal.begin(), deletion_scores_normal.end());
		size_t delete_operator = delete_op(gen);	
		auto begin_op_del = chrono::high_resolution_clock::now();	
		
		vector<bool> backup = {};
		while ((transfer_operator == 0 || transfer_operator == transfer_nodes.size()) && delete_operator  == 4) {
			delete_operator = delete_op(gen);
		}
		switch (delete_operator) {
			case 0 :
				//cout << "Greedy deletion\n";
				for (size_t j = 0; j < amount; j++) {
					request_bank.push_back(i.greedy_request_deletion(request_bank));
				}
				break;
			case 1 :
				//cout << "Random deletion\n";
				for (size_t j = 0; j < amount; j++) {
					request_bank.push_back(i.random_request_deletion(request_bank));
				}
				break;
            case 2 :
				//cout << "Shaw deletion\n";
                request_bank = i.shaw_deletion(amount);
				break;
			case 3 :
				//cout << "Cluser removal\n";
				request_bank = i.cluster_deletion(request_bank, amount);
				break;
			case 4 :
				//cout << "Transfer swap\n";
				backup = transfer_nodes_opened[transfer_operator];
				request_bank = i.transfer_swap(request_bank, transfer_weights, transfer_nodes_opened[transfer_operator]);
				for ( size_t j = request_bank.size(); j < amount; j++){
					request_bank.push_back(i.random_request_deletion(request_bank));
				}
				break;
		}
		
		auto end_op_del = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> elapsed_op_del = chrono::duration_cast<chrono::duration<double>>(end_op_del - begin_op_del);
		op_del_time[delete_operator] += elapsed_op_del.count();
		
		i.remove_empty_vehicle();
		
		discrete_distribution<> insert_op(insertion_scores_normal.begin(), insertion_scores_normal.end());
		size_t insert_operator = insert_op(gen);

        auto begin_op = chrono::high_resolution_clock::now();
        assert(insert_operator <= 3);
        size_t max_iters = request_bank.size();
		switch (insert_operator) {
			case 0 :
                //cout << "insert greedy\n";
				for (size_t r = 0; r < max_iters; r++) {
					size_t req_loc = i.greedy_request_insertion(request_bank);
					request_bank.erase(request_bank.begin() + req_loc);
				}
				request_bank.clear();
				break;
            case 1:
                //cout << "insert regret\n";
                for (size_t r = 0; r < max_iters; r++) {
					size_t req_loc = i.regret_2_insertion(request_bank);
					request_bank.erase(request_bank.begin() + req_loc);
				}
				request_bank.clear();
				break;
            case 2:
                //cout << "insert semigreedy\n";
                for (size_t r = 0; r < max_iters; r++) {
					size_t req_loc = i.random_request_greedy_insertion(request_bank);
					request_bank.erase(request_bank.begin() + req_loc);
				}
				request_bank.clear();
				break;
			case 3 :
			    //cout << "insert with transfer\n";
			    transfer_count  += 1;
                for (size_t r = 0; r < max_iters; r++) {
					size_t req_loc = i.greedy_route_insertion(request_bank);
					request_bank.erase(request_bank.begin() + req_loc);
				}
				//cout << "After transfer insertion\n";
				request_bank.clear();
				break;
			default : cout << "No insert error\n";
		}
		
        //measuring time for operators
        auto end_op = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> elapsed_op = chrono::duration_cast<chrono::duration<double>>(end_op - begin_op);
        op_time[insert_operator] += elapsed_op.count();
		

        operator_count[delete_operator*insertion_scores.size() + insert_operator] += 1;
        insertion_count[insert_operator] += 1;
        deletion_count[delete_operator] += 1;
		transfer_count_vector[transfer_operator] += 1;
		
		interactive_all[insert_operator][delete_operator]++;
		
		if (insert_operator == 3) { start_temperature = 1000;}
		bool accepted1 = false;
		bool accepted2 = false;
		bool accepted3 = false;
		double s = i.calculate_obj_val();
		//cout << "Best solution: " << best_solution << ", current solution: " << current_solution << ", new solution: " << s << "\n"; 
		if (s < best_solution) {
			//cout << "New solution best\n";
			accepted1 = true;
		} else if (s < current_solution) {
			//cout << "New solution current\n";
			accepted2 = true;
		} else if (acceptation_criterion_met(s, current_solution, loop_count)) {
			//cout << "New solution criterion\n";
			accepted3 = true;
		}
		
		if((accepted1 || accepted2 || accepted3) && insert_operator == 3){transfer_cost_accept +=1; }
		if (accepted1) {
			if(i.is_feasible()) {
                if(insert_operator==3){transfer_accept+=1;}
				//cout << "Feasible new best solution?\n";
				deletion_rewards[delete_operator] += 33;
				insertion_rewards[insert_operator] += 33;
				//transfer_rewards[transfer_operator] += 100;
				best_solution = s;
                current_solution = s;
                i.obj_val = s;
                best_routes.clear();
                best_routes = i.routes;
				
				interactive_acc1[insert_operator][delete_operator]++;
			} else {
			    i.routes.clear();
				i.routes = i.old_routes;
				pickup_vehicle = i.old_pickup_vehicle;
				delivery_vehicle = i.old_delivery_vehicle;
				feasibility_rejection++;
				
				// Resetting which transfer facilities are opened if transfer swap has failed
				if (delete_operator == 4) {
					for (size_t i = 0; i < transfer_nodes.size(); i++) {
						transfer_nodes_opened[transfer_operator][i] = backup[i];
						transfer_nodes[i].open = transfer_nodes_opened[transfer_operator][i];
					}
				}
			}
		} else if (accepted2) {
			if(i.is_feasible()) {
				//cout << "accepted 2\n";
                if(insert_operator==3){transfer_accept+=1;}
				deletion_rewards[delete_operator] += 20;
				insertion_rewards[insert_operator] += 20;
				transfer_rewards[transfer_operator] += 20;
                current_solution = s;
				
				interactive_acc2[insert_operator][delete_operator]++;
			} else {
			    i.routes.clear();
				i.routes = i.old_routes;
				pickup_vehicle = i.old_pickup_vehicle;
				delivery_vehicle = i.old_delivery_vehicle;
				feasibility_rejection++;
				
				// Resetting which transfer facilities are opened if transfer swap has failed
				if (delete_operator == 4) {
					for (size_t i = 0; i < transfer_nodes.size(); i++) {
						transfer_nodes_opened[transfer_operator][i] = backup[i];
						transfer_nodes[i].open = transfer_nodes_opened[transfer_operator][i];
					}
				}
			}
		} else if (accepted3) {
			if(i.is_feasible()) {
				//cout << "accepted 3\n";
                if(insert_operator==3){transfer_accept+=1;}
				deletion_rewards[delete_operator] += 15;
				insertion_rewards[insert_operator] += 15;
				transfer_rewards[transfer_operator] += 10;
                current_solution = s;
				
				interactive_acc3[insert_operator][delete_operator]++;
			} else {
			    i.routes.clear();
				i.routes = i.old_routes;
				pickup_vehicle = i.old_pickup_vehicle;
				delivery_vehicle = i.old_delivery_vehicle;
				feasibility_rejection++;
				
				// Resetting which transfer facilities are opened if transfer swap has failed
				if (delete_operator == 4) {
					for (size_t i = 0; i < transfer_nodes.size(); i++) {
						transfer_nodes_opened[transfer_operator][i] = backup[i];
						transfer_nodes[i].open = transfer_nodes_opened[transfer_operator][i];
					}
				}
			}
		} else {
			// Did not accept the solution, hence no need to check feasibility and return to previous solution
			i.routes.clear();
			i.routes = i.old_routes;
			pickup_vehicle = i.old_pickup_vehicle;
			delivery_vehicle = i.old_delivery_vehicle;
			costs_rejection++;
			
			// Resetting which transfer facilities are opened if transfer swap has failed
			if (delete_operator == 4) {
				for (size_t i = 0; i < transfer_nodes.size(); i++) {
					transfer_nodes_opened[transfer_operator][i] = backup[i];
					transfer_nodes[i].open = transfer_nodes_opened[transfer_operator][i];
				}
			}
		}
		if (insert_operator == 3) { start_temperature = 5;}
		best_results.push_back(best_solution);
	}
	auto finish = chrono::high_resolution_clock::now();
	chrono::duration<double> elapsed = finish - start;
	i.routes = best_routes;

	cout << "Transfer counts : " << transfer_count << "   "  << transfer_cost_accept << "   " << transfer_accept << '\n';
	cout << "Times  :  " << op_time[0]/insertion_count[0] << "    " << op_time[1]/insertion_count[1] << "    " << op_time[2]/insertion_count[2]<< "    " << op_time[3]/insertion_count[3]<<  '\n';
	cout << "Transfer scores: ";
	for (size_t i = 0; i < transfer_scores_normal.size(); i++) { cout << transfer_scores_normal[i] << "   "; } cout << "\n";
	cout << "Deletion scores: " << deletion_scores_normal[0] << "   " << deletion_scores_normal[1] << "  " << deletion_scores_normal[2] << "   " << deletion_scores_normal[3] << "   " << deletion_scores_normal[4] << "\n";
	cout << "Insertion scores: " << insertion_scores_normal[0] << "   " << insertion_scores_normal[1] << "  " << insertion_scores_normal[2] << "   "  << insertion_scores_normal[3] << "\n";
	cout << "Elapsed time: " << elapsed.count() << " s\n";
	cout << "Total number of rejections: " << (costs_rejection + feasibility_rejection) << "\n";
	cout << "Amount of costs rejection: " << costs_rejection << "\n";
	cout << "Amount of feasibility rejection: " << feasibility_rejection << "\n";
	cout << "\n";
	cout << "Initial solution value: "<< initialval << "\n";
	best_solution = 10000;
	size_t best_solution_index = -1;
	for (size_t j = 0; j < best_routes_all.size(); j++) {
		for (size_t i = 0; i < transfer_nodes.size(); i++)  {
			transfer_nodes[i].open = false;
		}
		i.routes.clear();
		i.routes = best_routes_all[j];
		for (size_t r = 0; r < i.routes.size(); r++) {
			for (size_t c = 0; c < i.routes[r].route.size(); c++) {
				if (i.routes[r].route[c].type == 't') {
					transfer_nodes[i.routes[r].route[c].index].open = true;
				}
			}
		}
		double candid_solution = i.calculate_obj_val();
		if (candid_solution < best_solution) { 
			best_solution = candid_solution;
			best_solution_index = j;
		}
	}
	
	i.routes.clear();
	i.routes = best_routes_all[best_solution_index];
	i.print_routes();
	size_t amount_facilities_opened = 0;
	for (size_t i = 0; i < transfer_nodes.size(); i++)  {
			transfer_nodes[i].open = false;
		}
		for (size_t r = 0; r < i.routes.size(); r++) {
			for (size_t c = 0; c < i.routes[r].route.size(); c++) {
				if (i.routes[r].route[c].type == 't') {
					transfer_nodes[i.routes[r].route[c].index].open = true;
					amount_facilities_opened++;
				}
			}
		}
	cout << "\n";
	cout << "Best solution: " << i.calculate_obj_val() << "\n";
	cout << "Amount of facilities open: " << amount_facilities_opened << "\n";
	cout << "\n";
	for (size_t t = 0;  t < transfer_nodes.size() + 1; t++) {
		for (size_t i = 0; i < transfer_nodes.size(); i++) {
			cout << transfer_nodes_opened[t][i] << " ";
		}
		cout << "\n";
	}
	
	i.output_vector(best_results, i.index);
	i.output_data(interactive_all,interactive_acc1,interactive_acc2,interactive_acc3,op_time,op_del_time,i.index,costs_rejection,feasibility_rejection,elapsed.count());
}

void read_csv() {
	ifstream ip(location);
	if(!ip.is_open()) cout << "ERROR: File Open" << '\n';

    string line;
	while (getline(ip, line)) {
        vector<int> tmp_data;
        string input_var;
        istringstream line_stream(line);
        while(getline(line_stream, input_var, ',')){
            int csv_element = atoi(input_var.c_str());
            tmp_data.push_back(csv_element);
        }
        input_data.push_back(tmp_data);
	}
	ip.close();
}

double euclidian_distance(Node a, Node b) {
	return sqrt(pow(a.x - b.x,2) + pow(a.y - b.y,2));
}

void clear_global(){
    arcs.clear();
    pickup_nodes.clear();
    delivery_nodes.clear();
    transfer_nodes.clear();
    depot_nodes.clear();
    all_nodes.clear();
    pickup_vehicle.clear();
    delivery_vehicle.clear();
    ride_times.clear();
    nearest_depot_gen_idx_d.clear();
    nearest_depot_gen_idx_p.clear();
    nearest_depot_gen_idx_t.clear();
    nearest_depot_insertion_cost.clear();
}

void solve_instance(vector<vector<int>> &input_data, int ins){
	Instance i;
	i.create_instance(input_data, ins);
    i.calculate_arcs();
	transfer_weights = i.facility_weights();
    i.preprocess();
    i.initial_solution();
	ALNS(i);
    i.write_output_file(ins);
    clear_global();
	
}

int main(int argc, char *argv[]) {
	read_csv();
	bool eol = 0;
	while(!eol) {
	    char response[20];
	    cout << "There are " << input_data.size() << " instances.\nType a number for a specific instance (starting with 0), type A for all instances, type B to abort.\n";
        cin >> response;
        switch(response[0])
        {
        case 'a':
        case 'A':
            for(size_t idx = 0; idx < input_data.size(); idx++) {
				solve_instance(input_data, idx);
				cout << "Instance " << idx << " succesfully solved!\n";
				cout << "\n";
			}
            break;
        case 'b':
        case 'B':
            eol = 1;
            break;
        default :
            if(isdigit(response[0])){
                unsigned int resp = atoi(response);
                if(resp >= input_data.size()){
                    cout << "Number too large, please try again\n";
                } else {
                    solve_instance(input_data, resp);
					cout << "Instance " << resp << " succesfully solved!\n\n";

                }
            } else {
				cout << "Not a number, please try again\n";
            }
        }
	}
    return 0;
}