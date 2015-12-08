/*
 * Belief.h
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef BELIEF_H_
#define BELIEF_H_

class Belief {
public:
	int dimension;
	vector<PreciseNumber> belief;
	PreciseNumber reward;

	Belief();
	Belief(int dimension);
	virtual ~Belief();

	int getDimension();
	void configure(int dimension);
	PreciseNumber get(int index);
	void set(int index, PreciseNumber value);
	void setReward(PreciseNumber value);
	PreciseNumber getReward();
	bool checkValidity();
	void view();
};

bool operator < (const Belief& lhs, const Belief& rhs);
bool operator > (const Belief& lhs, const Belief& rhs);
bool operator == (const Belief& lhs, const Belief& rhs);
bool operator != (const Belief& lhs, const Belief& rhs);
PreciseNumber operator - (const Belief& lhs, const Belief& rhs);
PreciseNumber dist(const Belief &lhs, const Belief& rhs);
bool equal(PreciseNumber a, PreciseNumber b);

#endif /* BELIEF_H_ */
