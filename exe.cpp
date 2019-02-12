#include <iostream>
#include <vector>
#include "vehicle.h"
#include "solution.h"
using namespace std;

int main() 
{
	Solution init_solution = creat_init_solution();
	Solution best_solution = ALNS(init_solution);
	cout << best_solution.obj_val;
    return 0;
}

Solution ALNS(double init_solution) {
	Solution best_solution;
	Solution current_solution;
	
	while(!stopping_criterion_met()) {
		Solution s = current_solution;
		destroy(s);
		repair(s);
		
		if (s.obj_val < best_solution.obj_val) {
			best_solution = s;
			current_solution = s;
		} else {
			if (acceptation_criterion_met(s.obj_val, current_solution.obj_val)) {
				current_solution = s;
			}
		}
	}
	return best_solution;
}