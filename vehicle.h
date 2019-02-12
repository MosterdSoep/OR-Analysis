#ifndef VEHICLE_H
#define VEHICLE_H

#include <iostream>
#include <vector>
using namespace std;

class vehicle {
	public:
	vector<size_t> arc_durations;
	vector<size_t> waiting_times;
	vector<size_t> route;
	vector<size_t> current_capacity;
};

#endif