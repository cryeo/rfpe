/*
 * BeliefDivision.h
 *
 *  Created on: 2012. 11. 21.
 *      Author: chaerim
 */

#include "Common.h"

#ifndef BELIEF_H_
#include "Belief.h"
#endif

#ifndef BELIEFDIVISION_H_
#define BELIEFDIVISION_H_

class BeliefDivision {
	int dimension;
	vector< vector< vector<Belief> > > extremePoints;
	// a0 + a1 x1 + a2 x2 + .. + an xn >= 0
	vector< vector<Inequality> > inequalities;
public:
	BeliefDivision();
	BeliefDivision(int dimension);
	void configure(int dimension);
	void addPolytopeToDivision(int division, vector<Belief> po);
	vector< vector<Belief> > getDivision(int division);
	vector<Inequality> getDivisionInequalities(int division);
	vector<Belief> getExtremePointsOfPolytope(int division, int index);
	Inequality getInequalitiesOfPolytope(int division, int index);
	void makeInequalities(int division);
	int getNumberOfDivisions();
	int getSizeOfDivisions(int division);
	bool existEmptyDivision();
};

bool operator == (const BeliefDivision& lhs, const BeliefDivision& rhs);

#endif /* BELIEFDIVISION_H_ */
