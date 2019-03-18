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
string location = "C://Users//Luuk//Documents//Codeblocks projecten//OR_analysis//instances.csv";
vector<vector<int>> input_data;

size_t maximum_loops = 100;

bool acceptation_criterion_met(double s, double current_solution, size_t loop_count) {
	double temperature = (double)maximum_loops - (double)loop_count + 1;
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

void ALNS(Instance &i) {
	//transfer_nodes[0].open = true;

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

	vector<double> deletion_scores = {1,1};
	vector<double> deletion_rewards = {0,0};

	//vector<double> insertion_scores = {1};
	//vector<double> insertion_rewards = {0};
	vector<double> insertion_scores = {1,1};
	vector<double> insertion_rewards = {0,0};
	while(!stopping_criterion_met(loop_count)) {
		i.old_routes = i.routes;
		i.old_pickup_vehicle = pickup_vehicle;
		i.old_delivery_vehicle = delivery_vehicle;

		loop_count++;
		if (loop_count % 100 == 0) {
			for (size_t i = 0; i < deletion_scores.size(); i++) {
				deletion_scores[i] += deletion_rewards[i];
			}
			for (size_t i = 0; i < insertion_scores.size(); i++) {
				insertion_scores[i] += insertion_rewards[i];
			}
		}
		vector<size_t> request_bank;
<<<<<<< HEAD
		
		//size_t amount = (rand() % (int(log(i.request_amount)/log(1.5))+1) ) + 1;
		size_t amount = (rand() % 1) + 1;
=======

		size_t amount = (rand() % (int(log(i.request_amount)/log(1.5))+1) ) + 1;
		//size_t amount = (rand() % 1) + 1;
>>>>>>> 44514bb264d15047d31f429dcbbb3b7f002ec7b3
		discrete_distribution<> delete_op({deletion_scores[0]});
		size_t delete_operator = delete_op(gen);

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
		}
		i.remove_empty_vehicle();
<<<<<<< HEAD
		
		//discrete_distribution<> insert_op({insertion_scores[0],insertion_scores[1]});
		discrete_distribution<> insert_op({insertion_scores[0]});
		size_t insert_operator = insert_op(gen);
		
=======

		discrete_distribution<> insert_op({insertion_scores[0],insertion_scores[1]});
		//discrete_distribution<> insert_op({insertion_scores[0]});
		size_t insert_operator = insert_op(gen);
>>>>>>> 44514bb264d15047d31f429dcbbb3b7f002ec7b3
		switch (insert_operator) {
			case 0 :
				//cout << "Greedy insertion with score: " << insertion_scores[insert_operator] << ", at loop: " << loop_count << "\n";
				for (size_t r : request_bank) {
					size_t req_loc = i.greedy_request_insertion(request_bank);
					request_bank.erase(request_bank.begin() + req_loc);
				}
				request_bank.clear();
				break;
            case 1:
                cout << "test\n";
                for (size_t r : request_bank) {
					size_t req_loc = i.regret_2_insertion(request_bank);
					cout << "after 2\n";
					request_bank.erase(request_bank.begin() + req_loc);
					cout << "after 3\n";
				}
				request_bank.clear();
				cout << "regret succesfull \n";
				break;
            case 2:
				for (size_t r : request_bank) {
					size_t req_loc = i.random_request_greedy_insertion(request_bank);
					request_bank.erase(request_bank.begin() + req_loc);
				}
				request_bank.clear();
				break;

			/*case 1 :
				//cout << "Transfer insertion with score: " << insertion_scores[insert_operator] << ", at loop: " << loop_count << "\n";
				for (size_t index = 0; index < request_bank.size(); index++) {
					i.greedy_route_insertion(request_bank[index]);
				}
				//cout << "After transfer insertion\n";
				request_bank.clear();
				break;*/
			default : cout << "No insert error\n";
		}
		bool accepted1 = false;
		bool accepted2 = false;
		bool accepted3 = false;
		double s = i.calculate_obj_val();
		if (s < best_solution) {
			//cout << "New solution\n";
			accepted1 = true;
		} else if (s < current_solution) {
			//cout << "New solution\n";
			accepted2 = true;
		} else if (acceptation_criterion_met(s, current_solution, loop_count)) {
			//cout << "New solution\n";
			accepted3 = true;
		}

		if (accepted1) {
			if(i.is_feasible()) {
				//cout << "Feasible new best solution?\n";
				deletion_rewards[delete_operator] += 33;
				insertion_rewards[insert_operator] += 33;
				best_solution = s;
                current_solution = s;
                i.obj_val = s;
                best_routes = i.routes;
			} else {
				i.routes = i.old_routes;
				pickup_vehicle = i.old_pickup_vehicle;
				delivery_vehicle = i.old_delivery_vehicle;
				feasibility_rejection++;
			}
		} else if (accepted2) {
			if(i.is_feasible()) {
				deletion_rewards[delete_operator] += 20;
				insertion_rewards[insert_operator] += 20;
                current_solution = s;
			} else {
				i.routes = i.old_routes;
				pickup_vehicle = i.old_pickup_vehicle;
				delivery_vehicle = i.old_delivery_vehicle;
				feasibility_rejection++;
			}
		} else if (accepted3) {
			if(i.is_feasible()) {
				deletion_rewards[delete_operator] += 15;
				insertion_rewards[insert_operator] += 15;
                current_solution = s;
			} else {
				i.routes = i.old_routes;
				pickup_vehicle = i.old_pickup_vehicle;
				delivery_vehicle = i.old_delivery_vehicle;
				feasibility_rejection++;
			}
		} else {
			// Did not accept the solution, hence no need to check feasibility and return to previous solution
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
	cout << "Elapsed time: " << elapsed.count() << " s\n";
	cout << "Total number of rejections: " << (costs_rejection + feasibility_rejection) << "\n";
	cout << "Amount of costs rejection: " << costs_rejection << "\n";
	cout << "Amount of feasibility rejection: " << feasibility_rejection << "\n";
	cout << "\n";
	cout << "Initial solution value: "<< initialval << "\n";
	cout << "Best solution: " << best_solution << "\n";
	cout << "\n";

	i.routes = best_routes;
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
            for(size_t idx = 0; idx < input_data.size(); idx++){
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
