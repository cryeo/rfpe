/*
 * Profile.cpp
 *
 *  Created on: 2013. 1. 8.
 *      Author: chaerim
 */

#include "Profile.h"

Profile::Profile() {
	dimension = 0;
}

Profile::~Profile() {
}

Profile::Profile(int index, vector<int> &bounds) {
	dimension = bounds.size();
	data.resize(dimension);
	for (int i = dimension - 1; i >= 0; i--) {
		data[i] = index % bounds[i];
		index /= bounds[i];
	}
}

Profile::Profile(int a) {
	dimension = 1;
	data.resize(dimension);
	data[0] = a;
}

Profile::Profile(int a, int b) {
	dimension = 2;
	data.resize(dimension);
	data[0] = a; data[1] = b;
}

Profile::Profile(int a, int b, int c) {
	dimension = 3;
	data.resize(dimension);
	data[0] = a; data[1] = b; data[2] = c;
}

Profile::Profile(int a, int b, int c, int d) {
	dimension = 4;
	data.resize(dimension);
	data[0] = a; data[1] = b; data[2] = c; data[3] = d;
}

Profile::Profile(int a, int b, int c, int d, int e) {
	dimension = 5;
	data.resize(dimension);
	data[0] = a; data[1] = b; data[2] = c; data[3] = d; data[4] = e;
}

Profile::Profile(int a, int b, int c, int d, int e, int f) {
	dimension = 6;
	data.resize(dimension);
	data[0] = a; data[1] = b; data[2] = c; data[3] = d; data[4] = e; data[5] = f;
}

int Profile::getSize() {
	return dimension;
}

int Profile::getIndex(vector<int> &bounds) {
	int res = 0;
	for (int i = 0; i < dimension; i++) {
		res *= bounds[i];
		res += data[i];
	}
	return res;
}

int Profile::operator[] (int index) {
	if (index >= dimension) return -1;
	return data[index];
}

void Profile::view() {
	cout << "(";
	for (int i = 0; i < dimension; i++) {
		if (i > 0) cout << ",";
		cout << data[i];
	}
	cout << ")" << endl;
}
