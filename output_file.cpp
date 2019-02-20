#include "instance.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include <windows.h>


void Instance::write_output_file(size_t instance_number) {
	SetCurrentDirectory("C:\\Users\\Hp\\Desktop\\Master\\Blok3\\ORACS");
	cout << GetCurrentDirectory;
	ostringstream file_name_stream;
	file_name_stream << "/oracs_" << instance_number << ".csv";
	string file_name = file_name_stream.str();
	//cout << file_name_stream;
	ofstream output_file("iets.csv");
	if(output_file.fail()){ cout << "failed\n";}
	output_file << "2\n";
	output_file << instance_number << "\n";
	output_file << (roundf(obj_val * 100) / 100) << "\n";
	output_file << routes.size() << "\n";
	for (Vehicle v : routes) {
		output_file << "\n";
		for (Node node : v.route) {
			if ((node.gen_idx >= (2*request_amount)) && (node.gen_idx <= (2*request_amount+transfer_location_amount-1))) {
				// node is a transfer node
				string rep = "";
				if (node.type == 'p') { rep.append("0"); }
				else { rep.append("1"); }
				
				// Node
				// number of digits : (i > 0 ? (int) log10 ((double) i) + 1 : 1)
				
				for (size_t i = 0; i < (3-(node.index > 0 ? (int) log10 ((double) node.index) + 1 : 1)); i++) {
					rep.append("0");
				}
				rep.append(to_string(node.index));
				
				// Request
				
				for (size_t i = 0; i < (3-(node.request_idx > 0 ? (int) log10 ((double) node.request_idx) + 1 : 1)); i++) {
					rep.append(to_string(node.request_idx));
				}
				
				if (node.gen_idx == v.route.back().gen_idx) {
					output_file << rep << '\n';
				} else {
					output_file << rep << ',';
				}
			} else {
				// node is not a transfer node
				if (node.gen_idx == v.route.back().gen_idx) {
					output_file << node.index << '\n';
				} else {
					output_file << node.index << ',';
				}
			}
		}
		for (double service_time : v.time_at_node) {
			if (service_time == v.time_at_node.back()) {
				output_file << service_time << '\n';
			} else {
				output_file << service_time << ',';
			}
		}
	}
	output_file << "\n";
	output_file.close();
}

