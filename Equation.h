/*
 * Equation.h
 *
 *  Created on: 2012. 11. 21.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef EQUATION_H_
#define EQUATION_H_

class Equation {
	int dimension;
	vector< vector<PreciseNumber> > equation;
	vector<PreciseNumber> result;
	int success;
public:
	Equation();
	Equation(int dimension);
	Equation(int dimension, vector< vector<PreciseNumber> > &eq);
	virtual ~Equation();
	void configure(int dimension);
	void configure(int dimension, vector< vector<PreciseNumber> > &eq);
	bool solve();
	int getDimension();
	vector<PreciseNumber> getResult();
	PreciseNumber getResult(int index);
};

#endif /* EQUATION_H_ */
