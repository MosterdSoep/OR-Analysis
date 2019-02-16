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
	void remove_node(size_t){};
};

void Vehicle::remove_node(size_t location) {
	this.arc_durations.erase(location);
	this.waiting_times.erase(location);
	this.route.erase(location);
	this.current_capacity.erase(location);
}

#endif
