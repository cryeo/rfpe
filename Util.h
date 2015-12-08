/*
 * Util.h
 *
 *  Created on: 2012. 11. 27.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef BELIEF_H_
#include "Belief.h"
#endif

#ifndef BELIEFDIVISION_H_
#include "BeliefDivision.h"
#endif

#ifndef PROFILE_H_
#include "Profile.h"
#endif

#ifndef UTIL_H_
#define UTIL_H_

#include "setoper.h"
#include "cdd.h"

typedef complex<PreciseNumber> P;

PreciseNumber cross(const P& a, const P& b);
PreciseNumber dot(const P& a, const P& b);
int ccw(Belief ba, Belief bb, Belief bc);
vector<Belief> convexHull(vector<Belief> ps);

vector<Belief> getConvex(vector<Belief> &ps);

ostream& operator << (ostream& os, const vector<PreciseNumber> &v);
ostream& operator << (ostream& os, const vector<Belief> &v);
ostream& operator << (ostream& os, const BeliefDivision &bd);
ostream& operator << (ostream& os, const Belief &b);
ostream& operator << (ostream& os, const Profile &p);

string beliefToString(Belief &bs, int precision = 4);
string beliefToString(vector<Belief> &bs, int precision = 4);

dd_MatrixPtr dd_VectorToMatrix(const vector< vector<PreciseNumber> > &input, dd_RepresentationType rep);
dd_MatrixPtr dd_VectorToMatrix(const vector< vector<PreciseNumber> > &input, dd_RepresentationType rep, vector<bool> &isLinear);

vector< vector<PreciseNumber> > dd_MatrixToVector(dd_MatrixPtr &M);
vector< vector<int> > dd_SetFamilyToVector(dd_SetFamilyPtr &GI);

Inequality convertPointToInequality(const vector< vector<PreciseNumber> > &input);
Inequality convertBeliefToInequality(const vector<Belief> &nextBeliefDivision);

pair< vector< vector<PreciseNumber> >, vector< vector<int> > > convertInequalityToPointWithIncidence(const Inequality &input);

vector< vector<PreciseNumber> > intersection(const Inequality &inequalities1, const Inequality &inequalities2);
vector<Belief> merge(const vector<Belief> &polytope1, const vector<Belief> &polytope2);

vector< vector<PreciseNumber> > convertBeliefToPoint(const vector<Belief> &nextBeliefDivision);
vector< vector<PreciseNumber> > convertPointToPoint(const vector< vector<PreciseNumber> > &input);

vector<Belief> convertPointToBelief(const vector< vector<PreciseNumber> > &input);
vector<Belief> convertPointToBeliefWithReward(const vector< vector<PreciseNumber> > &input, vector<bool> &isPoint);

bool innerPolytope(vector<Belief> &innerPolytope, vector<Belief> &outerPolytope);
bool isInnerPointOfPolytope(Belief &point, const vector<Belief> &polytope);

bool innerPolytope2(vector<Belief> &innerPolytope, vector<Belief> &outerPolytope);

Belief centerOfPolytope(vector<Belief> &polytope);

int pow(int a, int b);

bool isSame(vector< vector<Belief> > &b1, vector< vector<Belief> > &b2);
bool isSameWithPrecision(Belief a, Belief b, int digit = 9);

template<typename T>
string toString(T val) {
	stringstream s;
	s << val;
	return s.str();
}

template <typename T>
void freeVector(T &t) {
    T tmp;
    t.swap(tmp);
}

string rationalToString(PreciseNumber val);
//string rationalToString(PreciseNumber val, int len);

int toInteger(string val);

#endif
