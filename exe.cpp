#include <fstream>
#include <sstream>
#include <cmath>
#include <random>
#include "instance.h"
using namespace std;

// General variables
string location = "D://Documenten//Studie//OR analysis//OR Analysis//instances.csv";
//string location = "C://Users//Hp//Documents//GitHub//OR-Analysis//instances.csv";
//string location = "//home//luuk//Documents//ORACS//OR-Analysis-master";
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

void clear_global(){
    arcs.clear();
    pickup_nodes.clear();
    delivery_nodes.clear();
    transfer_nodes.clear();
    depot_nodes.clear();
    all_nodes.clear();
}

void solve_instance(vector<vector<int>> &input_data, int ins){
	Instance i;
	i.create_instance(input_data, ins);
    i.calculate_arcs();
    i.preprocess();
    i.initial_solution();
    //if(i.capacity_feasible()){cout << "correct\n";}
    //i.routes[1].add_node( 3, pickup_nodes[1]);
    if(i.maximum_ride_time_not_exceeded()){cout << "correct\n";}
    i.write_output_file(ins);
        //for(size_t idx = 0; idx < i.request_amount; idx++){cout << i.ride_times[idx] << '\n';}
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
