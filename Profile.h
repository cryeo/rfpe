/*
 * Profile.h
 *
 *  Created on: 2013. 1. 8.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef PROFILE_H_
#define PROFILE_H_

class Profile {
	int dimension;
	vector<int> data;
public:
	Profile();
	Profile(int index, vector<int> &bounds);
	Profile(int a);
	Profile(int a, int b);
	Profile(int a, int b, int c);
	Profile(int a, int b, int c, int d);
	Profile(int a, int b, int c, int d, int e);
	Profile(int a, int b, int c, int d, int e, int f);
	virtual ~Profile();
	int getSize();
	int getIndex(vector<int> &bounds);
	int operator[] (int index);
	void view();
};

typedef Profile StateProfile;
typedef Profile ActionProfile;
typedef Profile SignalProfile;

#endif /* PROFILE_H_ */
