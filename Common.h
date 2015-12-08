/*
 * common.h
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

#ifndef GMPRATIONAL
#define GMPRATIONAL
#endif

#ifndef COMMON_H_
#define COMMON_H_

#include <cstdio>
#include <cstdlib>
#include <numeric>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <string>
#include <sstream>
#include <complex>
#include <cstring>
#include <stdexcept>
#include <cassert>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <cmath>
#include <cstdarg>
#include <stack>
#include <queue>
#include <gmpxx.h>

using namespace std;

typedef double Number;
typedef mpq_class PreciseNumber;

const PreciseNumber EPS = PreciseNumber("1/1000000000");
const PreciseNumber INF = PreciseNumber("1000000000");
const PreciseNumber ONE = PreciseNumber(1);
const PreciseNumber ZERO = PreciseNumber(0);

struct Inequality {
	vector< vector<PreciseNumber> > coefficient;
	vector<bool> isLinear;
	Inequality() {}
	Inequality(vector < vector<PreciseNumber> > ce, vector<bool> il) : coefficient(ce), isLinear(il) {}
};

static int globalPlayer;
static map<string, PreciseNumber> globalVar;

#endif /* COMMON_H_ */
