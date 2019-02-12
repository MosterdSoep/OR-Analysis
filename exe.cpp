#include <iostream>
#include <vector>
#include "vehicle.h"
#include "solution.h"
using namespace std;

int main() {
	double init_solution = create_init_solution();
	double best_solution = ALNS(init_solution);
	cout << best_solution;
    return 0;
}

double ALNS(double init_solution) {
	double best_solution;
	double current_solution;
	
	size_t loop_count = 0;
	while(!stopping_criterion_met(loop_count)) {
		double s = current_solution;
		s = destroy(s);
		s = repair(s);
		
		# check feasibility and compute obj_val for s
		
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

bool stopping_criterion_met(size_t loop_count) {
	if (loop_count <= 1000) { return false; }
	else { return true; }
}

bool acceptation_criterion_met(double candid_solution, double current_solution) {
	if (current_solution - candid_solution < 0) { return false; }
	else { return true; }
}

double destroy(double s) {
	const int range_from = 0;
	const int range_to = 100;
	random_device rand_dev;
	mt19937 generator(rand_dev());
	uniform_int_distribution<int> distr(range_from, range_to);

	random_number = distr(generator);
	
	if (random_number <= 100) { return worst_removal(s); }
	
	return null;
}

double worst_removal(double s) {
	# 1. Find the worst point in a route by looping over the vehicle vectors and calculating the difference in costs.
	# 2. Once a location has been found, remove it from the vehicle by altering all the vectors.
	
	for (vector v : s.routes) {
		
	}
	
	return s;
}

