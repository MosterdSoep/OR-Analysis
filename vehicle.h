#ifndef VEHICLE_H
#define VEHICLE_H

#include <iostream>
#include <vector>
using namespace std;

class Vehicle {
  public:
	vector<int> arc_durations;
	vector<int> waiting_times;
	vector<int> route;
	vector<int> current_capacity;
};

#endif