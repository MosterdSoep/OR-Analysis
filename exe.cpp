#include <fstream>
#include <sstream>
#include <cmath>
#include <random>
#include "instance.h"
using namespace std;

// General variables
//string location = "D://Downloads//instances.csv";
//string location = "C://Users//Hp//Desktop//Master//Blok3//ORACS//instances.csv";
string location = "X://My Documents//Master//ORACS//OR-Analysis-master//OR-Analysis-master//instances.csv";
vector<vector<int>> input_data;

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

/*

bool stopping_criterion_met(size_t loop_count) {
	if (loop_count <= 1000) { return false; }
	else { return true; }
}

bool acceptation_criterion_met(double candid_solution, double current_solution) {
	if (current_solution - candid_solution < 0) { return false; }
	else { return true; }
}

size_t worst_removal(int amount_removed) {
	// 1. Find the worst point in a route by looping over the vehicle vectors and calculating the difference in costs.
	// 2. Once a location has been found, remove it from the vehicle by altering all the vectors.

	size_t best_removal = 0;
	size_t current_removal;
	size_t index_removed_node;
	size_t removed_node_index;
	Vehicle vehicle_removed_node;
	for (Vehicle v : routes) {
		for (Node n : v.route) {
			// If first node is removed then just substract the first transportation cost
						current_removal = arcs[v.route[i-1]][v.route[i+1]] - arcs[v.route[i-1]][v.route[i]] - arcs[v.route[i]][v.route[i+1]];
			if (current_removal < best_removal) {
				best_removal = current_removal;
				index_removed_node = i;
				removed_node_index = i;
				vehicle_removed_node = v;
			}
		}
	}
	// Remove the node from the vehicle
	vehicle_removed_node.route.erase(index_removed_node);
	return removed_node_index;
}

size_t destroy() {
	size_t destroyed_node_index = worst_removal(1);
}

void greedy_insertion(size_t node_index) {
	// 1. Find the worst point in a route by looping over the vehicle vectors and calculating the difference in costs.
	// 2. Once a location has been found, remove it from the vehicle by altering all the vectors.
	node = all_nodes[node_index];
	bool first = true;
	size_t best_insertion = 0;
	size_t current_insertion;
	size_t index_insertion;
	Vehicle vehicle_insertion;
	for (Vehicle v : routes) {
		for (int i : v.route) {
			// If first node is removed then just substract the first transportation cost
			current_removal =  arcs[v.route[i-1]][node.gen_idx] + arcs[v.route[i]][node.gen_idx] - arcs[v.route[i-1]][v.route[i]];
			if (first) { current_insertion = best_insertion; first = false; }
			if (current_insertion <= best_insertion) {
				best_insertion = current_insertion;
				index_insertion = i;
				vehicle_insertion = v;
			}
		}
	}
	// Remove the node from the vehicle
	vehicle_insertion.route.insert(index_insertion+1);
}

void repair(size_t node_index) {
	greedy_insertion(node_index);
}

double ALNS(double init_solution) {
	double best_solution;
	double current_solution;

	size_t loop_count = 0;
	while(!stopping_criterion_met(loop_count)) {
		double s = current_solution;
		size_t destroyed_node_index = destroy();
		repair(destroyed_node_index);

		// check feasibility and compute obj_val for s

		if (s < best_solution) {
			best_solution = s;
			current_solution = s;
		} else {
			if (acceptation_criterion_met(s, current_solution)) {
				current_solution = s;
			}
		}
		loop_count++;
	}
	return best_solution;
}

double create_init_solution() {

	return 0;
}
*/

void solve_instance(vector<vector<int>> &input_data, int ins){
	Instance i;
	i.create_instance(input_data, ins);
    i.calculate_arcs();
    i.preprocess();
	//double init_solution = create_init_solution();
	//double best_solution = ALNS(init_solution);
	//write_output_file();
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
        case 'A':
            for(size_t idx = 0; idx < input_data.size(); idx++){
				solve_instance(input_data, idx);
				cout << "Instance " << idx << " succesfully solved!\n";
				cout << "\n";
			}
            break;
        case 'B':
            eol = 1;
            break;
        default :
            if(isdigit(response[0])){
                if(atoi(response) >= input_data.size()){
                    cout << "Number too large, please try again\n";
                } else {
                    solve_instance(input_data, atoi(response));
					cout << "Instance " << atoi(response) << " succesfully solved!\n";
					cout << "\n";
                }
            } else {
				cout << "Not a number, please try again\n";
            }
        }
	}
    return 0;
}
