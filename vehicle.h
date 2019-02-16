#ifndef VEHICLE_H
#define VEHICLE_H

#include <iostream>
#include <vector>
#include "node.h"
using namespace std;

class Vehicle {
  public:
	size_t index;
	vector<double> arc_durations;
	vector<double> waiting_times;
	vector<Node> route;
	vector<size_t> current_capacity;
};

#endif
