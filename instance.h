#ifndef INSTANCE_H
#define INSTANCE_H

#include <iostream>
#include <vector>
#include "node.h"
#include "time_window.h"
using namespace std;

class Instance {
  public:
	vector<int> facility_costs;
	vector<Node> pickup_nodes;
	vector<Node> delivery_nodes;
	vector<Node> transfer_nodes;
	vector<Node> depot_nodes;
	vector<Time_window> pickup_time_windows;
	vector<Time_window> delivery_time_windows;
	vector<int> ride_times;
	vector<int> service_times;
};

#endif