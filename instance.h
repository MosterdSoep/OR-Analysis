#ifndef INSTANCE_H
#define INSTANCE_H

#include <iostream>
#include <vector>
#include <node.h>
#include <time_window.h>
using namespace std;

class Instance {
  public:
	vector<size_t> facility_costs;
	vector<node> pickup_nodes;
	vector<node> delivery_nodes;
	vector<node> transfer_nodes;
	vector<node> depot_nodes;
	vector<time_window> pickup_time_windows;
	vector<time_window> delivery_time_windows;
	vector<size_t> ride_times;
	vector<size_t> service_times;
};

#endif