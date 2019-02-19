#include "instance.h"

void Instance::write_output_file(size_t instance_number) {
	ofstream output_file;
	ostringstream file_name_stream;
	file_name_stream << "oracs_" << instance_number << ".csv";
	string file_name = file_name_stream.str();
	output_file.open(file_name);
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
				if (node.pickup) { rep.append('0'); }
				else { rep.append('1') }
				
				// Node
				// number of digits : (i > 0 ? (int) log10 ((double) i) + 1 : 1)
				
				for (size_t i = 0; i < (3-(node.index > 0 ? (int) log10 ((double) node.index) + 1 : 1)); i++) {
					rep.append("0");
				}
				rep.append(node.index);
				
				// Request
				
				for (size_t i = 0; i < (3-(node.request_idx > 0 ? (int) log10 ((double) node.request_idx) + 1 : 1)); i++) {
					rep.append(node.request_idx);
				}
				
				if (node == v.route.back()) {
					output_file << rep << '\n';
				} else {
					output_file << rep << ',';
				}
			} else {
				// node is not a transfer node
				if (node == v.route.back()) {
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
}