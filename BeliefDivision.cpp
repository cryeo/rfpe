/*
 * BeliefDivision.cpp
 *
 *  Created on: 2012. 11. 21.
 *      Author: chaerim
 */

#include "BeliefDivision.h"
#include "Util.h"

BeliefDivision::BeliefDivision() {
	dimension = 0;
}

BeliefDivision::BeliefDivision(int dimension) {
	configure(dimension);
}

void BeliefDivision::configure(int dimension) {
	this->dimension = dimension;
	extremePoints.resize(dimension);
	inequalities.resize(dimension);
}

void BeliefDivision::addPolytopeToDivision(int division, vector<Belief> po) {
	extremePoints[division].push_back(po);
}

vector< vector<Belief> > BeliefDivision::getDivision(int division) {
	return extremePoints[division];
}

vector<Belief> BeliefDivision::getExtremePointsOfPolytope(int division, int index) {
	return extremePoints[division][index];
}

Inequality BeliefDivision::getInequalitiesOfPolytope(int division, int index) {
	return inequalities[division][index];
}

void BeliefDivision::makeInequalities(int division) {
	if (extremePoints[division].size() > 0) {
		inequalities[division].resize(getSizeOfDivisions(division));
		for (int i = 0; i < getSizeOfDivisions(division); i++) {
			if (extremePoints[division][i].size() > 0) {
				inequalities[division][i] = convertBeliefToInequality(extremePoints[division][i]);
			}
		}
	}
}

vector<Inequality> BeliefDivision::getDivisionInequalities(int division) {
	return inequalities[division];
}

int BeliefDivision::getSizeOfDivisions(int division) {
	return extremePoints[division].size();
}

int BeliefDivision::getNumberOfDivisions() {
	return dimension;
}

bool BeliefDivision::existEmptyDivision() {
	for (int i = 0; i < dimension; i++) {
		if (extremePoints[i].size() == 0) return true;
	}
	return false;
}

bool operator == (const BeliefDivision& lhs, const BeliefDivision& rhs) {
	if (const_cast<BeliefDivision &>(lhs).getNumberOfDivisions() != const_cast<BeliefDivision &>(rhs).getNumberOfDivisions()) return false;
	for (int i = 0; i < const_cast<BeliefDivision &>(rhs).getNumberOfDivisions(); i++) {
		if (const_cast<BeliefDivision &>(lhs).getSizeOfDivisions(i) != const_cast<BeliefDivision &>(rhs).getSizeOfDivisions(i)) return false;
		for (int j = 0; j < const_cast<BeliefDivision &>(lhs).getSizeOfDivisions(i); j++) {
			vector<Belief> lhsv = const_cast<BeliefDivision &>(lhs).getExtremePointsOfPolytope(i, j);
			vector<Belief> rhsv = const_cast<BeliefDivision &>(rhs).getExtremePointsOfPolytope(i, j);
			if (lhsv.size() != rhsv.size()) return false;
			for (int k = 0; k < lhsv.size(); k++) {
				if (lhsv[k] != rhsv[k]) {
					return false;
				}
			}

		}
	}
	return true;
}
