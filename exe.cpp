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

size_t start_temperature = 1000;
size_t added_temperature = 0;

size_t maximum_loops = 100000;

bool acceptation_criterion_met(double s, double current_solution, size_t loop_count) {
	double temperature = ((double)start_temperature + added_temperature - (double)loop_count/100 + 1)/100;
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

bool time_based_criterion(auto start){
    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end_time - start;
    if(elapsed.count() > 600){
        return true;
    }
    return false;
}



bool transfers_correct(Instance &i){
    size_t counter = 0;
    for(Vehicle v: i.routes){
        for(Node n: v.route){
            if(n.type == 't'){
                counter +=1;
            }
        }
    }
    if(counter % 2 == 0){
        return true;
    }else{
    return false;
    }
}

void ALNS(Instance &i) {

	auto start = chrono::high_resolution_clock::now();

	double best_solution = i.calculate_obj_val();
	double current_solution = best_solution;
	vector<Vehicle> best_routes = i.routes;
	size_t loop_count = 0;

	i.print_routes();
	cout << "Objective value: " << i.calculate_obj_val() << "\n";
    double initialval = i.calculate_obj_val();
	random_device rd;
	mt19937 gen(rd());

    double costs_rejection = 0;
	double feasibility_rejection = 0;

	size_t transfer_loop_count = 0;
	vector<double> transfer_scores = {};
	vector<double> transfer_rewards = {};
	for (size_t i = 0; i < transfer_nodes.size() + 1; i++) {
		transfer_scores.push_back(1);
		transfer_rewards.push_back(0);
	}

	vector<double> deletion_scores = {1,1,1,1,1};
	vector<double> deletion_rewards = {0,0,0,0,0};
	vector<double> deletion_scores_normal = {1,1,1,1,1};

	//vector<double> insertion_scores = {1};
	//vector<double> insertion_rewards = {0};
	vector<double> insertion_scores = {1,1,1,1};
	vector<double> insertion_rewards = {0,0,0,0};
	vector<double> insertion_scores_normal = {1,1,1,1};

	vector<size_t> operator_count(deletion_scores.size()*insertion_scores.size());
	vector<size_t> deletion_count(deletion_scores.size(),0);
	vector<size_t> insertion_count(insertion_scores.size(), 0);

	vector<double> op_time = {0,0,0,0};

	vector<size_t> request_bank = {};

	size_t transfer_count = 0;
	size_t transfer_cost_accept = 0;
	size_t transfer_accept = 0;
    size_t percentage = 0;
	while(!stopping_criterion_met(loop_count) && !time_based_criterion(start)) {
        i.old_routes.clear();
		i.old_routes = i.routes;
		i.old_pickup_vehicle = pickup_vehicle;
		i.old_delivery_vehicle = delivery_vehicle;

		loop_count++;
		if (loop_count % 100 == 0) {
			for (size_t i = 0; i < deletion_scores.size(); i++) {
				deletion_scores[i] += deletion_rewards[i];
				deletion_scores_normal[i] = deletion_scores[i] / (1 + deletion_count[i]);
			}
			for (size_t i = 0; i < insertion_scores.size(); i++) {
				insertion_scores[i] += insertion_rewards[i];
				insertion_scores_normal[i] = insertion_scores[i] / (1 + insertion_count[i]);
			}
		}
		size_t transfer_operator;
		if (loop_count % 1000 == 0) {
			percentage+=1;
			cout << percentage << "% completed\n";

			discrete_distribution<size_t> transfer_op(transfer_scores.begin(), transfer_scores.end());
			transfer_operator = transfer_op(gen);

			size_t many = 0;
			if (transfer_loop_count <= transfer_nodes.size()) {
				for (size_t i = 0; i < transfer_nodes.size(); i++) {
					if (i < transfer_loop_count) {
						many++;
						transfer_nodes[i].open = true;
					} else {
						transfer_nodes[i].open = false;
					}
				}
				added_temperature += 1000;
				transfer_operator = transfer_loop_count;
			} else {
				for (size_t i = 0; i < transfer_nodes.size(); i++) {
					if (i < transfer_operator) {
						transfer_nodes[i].open = true;
					} else {
						transfer_nodes[i].open = false;
					}
				}
			}

			cout << "Opening " << transfer_operator << " transfer facilities\n";

			transfer_loop_count++;
			if (transfer_loop_count % 10 == 0 || transfer_loop_count == transfer_nodes.size()){
				// Update weights
				for (size_t i = 0; i < transfer_nodes.size() + 1; i++) {
					transfer_scores[i] += transfer_rewards[i];
					transfer_rewards[i] = 0;
				}
			}
		}
		size_t amount = (rand() % (int(log(i.request_amount)/log(1.5))+1) ) + 1;
		assert(amount < i.request_amount);
		//size_t amount = (rand() % 1) + 1;
		discrete_distribution<> delete_op(deletion_scores_normal.begin(), deletion_scores_normal.end());
		size_t delete_operator = delete_op(gen);

		switch (delete_operator) {
			case 0 :
				for (size_t j = 0; j < amount; j++) {
					request_bank.push_back(i.greedy_request_deletion(request_bank));
				}
				assert(transfers_correct(i));
				break;
			case 1 :
				for (size_t j = 0; j < amount; j++) {
					request_bank.push_back(i.random_request_deletion(request_bank));
					assert(transfers_correct(i));
				}
				break;
            case 2 :
                request_bank = i.shaw_deletion(amount);
                assert(transfers_correct(i));
				break;
            case 3 :
                request_bank = i.cluster_deletion(request_bank, amount);
                assert(transfers_correct(i));
                break;
            case 4 :
                request_bank = i.transfer_swap(request_bank);
                assert(transfers_correct(i));
                break;
		}
		i.remove_empty_vehicle();

		discrete_distribution<> insert_op(insertion_scores_normal.begin(), insertion_scores_normal.end());
		//discrete_distribution<> insert_op({insertion_scores[0]});
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
				assert(transfers_correct(i));
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

		bool accepted1 = false;
		bool accepted2 = false;
		bool accepted3 = false;
		double s = i.calculate_obj_val();
		if (s < best_solution) {
			accepted1 = true;
		} else if (s < current_solution) {
			//cout << "New solution\n";
			accepted2 = true;
		} else if (acceptation_criterion_met(s, current_solution, loop_count)) {
			//cout << "New solution\n";
			accepted3 = true;
		}
		if((accepted1 || accepted2 || accepted3) && insert_operator == 3){transfer_cost_accept +=1; }
		if (accepted1) {
			if(i.is_feasible()) {
                if(insert_operator==3){transfer_accept+=1;}
				//cout << "Feasible new best solution?\n";
				deletion_rewards[delete_operator] += 33;
				insertion_rewards[insert_operator] += 33;
				best_solution = s;
                current_solution = s;
                i.obj_val = s;
                best_routes.clear();
                best_routes = i.routes;
			} else {
			    i.routes.clear();
				i.routes = i.old_routes;
				pickup_vehicle = i.old_pickup_vehicle;
				delivery_vehicle = i.old_delivery_vehicle;
				feasibility_rejection++;
			}
		} else if (accepted2) {
			if(i.is_feasible()) {
                    if(insert_operator==3){transfer_accept+=1;}
				deletion_rewards[delete_operator] += 20;
				insertion_rewards[insert_operator] += 20;
                current_solution = s;
			} else {
			    i.routes.clear();
				i.routes = i.old_routes;
				pickup_vehicle = i.old_pickup_vehicle;
				delivery_vehicle = i.old_delivery_vehicle;
				feasibility_rejection++;
			}
		} else if (accepted3) {
			if(i.is_feasible()) {
                if(insert_operator==3){transfer_accept+=1;}
				deletion_rewards[delete_operator] += 15;
				insertion_rewards[insert_operator] += 15;
                current_solution = s;
			} else {
			    i.routes.clear();
				i.routes = i.old_routes;
				pickup_vehicle = i.old_pickup_vehicle;
				delivery_vehicle = i.old_delivery_vehicle;
				feasibility_rejection++;
			}
		} else {
			// Did not accept the solution, hence no need to check feasibility and return to previous solution
			i.routes.clear();
			i.routes = i.old_routes;
			pickup_vehicle = i.old_pickup_vehicle;
			delivery_vehicle = i.old_delivery_vehicle;
			costs_rejection++;
		}
	}
	auto finish = chrono::high_resolution_clock::now();
	chrono::duration<double> elapsed = finish - start;
	i.routes = best_routes;


	i.print_routes();
	cout << "infeasible transfers : " << transfer_infeasible_count << '\n';
	cout << "Transfer counts : " << transfer_count << "   "  << transfer_cost_accept << "   " << transfer_accept << '\n';
	cout << "Times  :  " << op_time[0]/insertion_count[0] << "    " << op_time[1]/insertion_count[1]<< "    " << op_time[2]/insertion_count[2]<< "    " << op_time[3]/insertion_count[3]<<  '\n';
    cout << "Deletion operators : " << deletion_count[0] << "  " << deletion_count[1] << "   " << deletion_count[2] << "  " << deletion_count[3] << "   " << deletion_count[4] << '\n';
    cout << "Insertion operators : " << insertion_count[0] << "  " << insertion_count[1]  << "  " << insertion_count[2] << "  " << insertion_count[3] << '\n';
	cout << "Elapsed time: " << elapsed.count() << " s\n";
	cout << "Total number of rejections: " << (costs_rejection + feasibility_rejection) << "\n";
	cout << "Amount of costs rejection: " << costs_rejection << "\n";
	cout << "Amount of feasibility rejection: " << feasibility_rejection << "\n";
	cout << "\n";
	cout << "Initial solution value: "<< initialval << "\n";
	cout << "Best solution: " << best_solution << "\n";
	cout << "\n";

	i.routes = best_routes;
    for(size_t idx = 0; idx < transfer_nodes.size(); idx++){
        transfer_nodes[idx].open = 0;
	}

	for(size_t idx = 0; idx < i.routes.size(); idx++){
        for(size_t adx = 0; adx < i.routes[idx].route.size(); adx++){
            if(i.routes[idx].route[adx].type == 't'){
                assert(i.routes[idx].route[adx].index < transfer_nodes.size());
                transfer_nodes[i.routes[idx].route[adx].index].open = 1;
            }
        }
	}
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

    vehicle_capacity = 0;
}

void solve_instance(vector<vector<int>> &input_data, int ins){
	Instance i;
	i.create_instance(input_data, ins);
    i.calculate_arcs();
    i.preprocess();
    i.initial_solution();
	ALNS(i);
    i.write_output_file(ins);
    clear_global();
}

int main() {
	read_csv();
	bool eol = 0;
	while(!eol){ // Create instance based on user input
	    char response[20];
	    cout << "There are " << input_data.size() << " instances.\nType a number for a specific instance (starting with 0), type A for all instances, type B to abort.\n";
        cin >> response;
        switch(response[0])
        {
        case 'a':
        case 'A':
            for(size_t idx = 160; idx < input_data.size(); idx++){
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
