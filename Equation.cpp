/*
 * Equation.cpp
 *
 *  Created on: 2012. 11. 21.
 *      Author: chaerim
 */

#include "Equation.h"

Equation::Equation() {
	dimension = 0;
	success = false;
}

Equation::Equation(int dimension) {
	configure(dimension);
}

Equation::Equation(int dimension, vector< vector<PreciseNumber> > &eq) {
	configure(dimension, eq);
}

Equation::~Equation() {

}

void Equation::configure(int dimension) {
	this->dimension = dimension;
	equation.resize(dimension);
	for (int i = 0; i < dimension; i++) {
		equation[i].resize(dimension + 1);
	}
	result.resize(dimension);
	success = true;
}

void Equation::configure(int dimension, vector< vector<PreciseNumber> > &eq) {
	this->dimension = dimension;
	this->equation = eq;
	result.resize(dimension);
	success = true;
}

int Equation::getDimension() {
	return dimension;
}

bool Equation::solve() {
	vector<PreciseNumber> b(dimension), y(dimension);

	for (int i = 0; i < dimension; i++) {
		b[i] = equation[i][dimension];
	}

	// LU decompression O(n^3)
	for (int i = 0; i < dimension - 1; i++) {
		if (equation[i][i] == 0) {
			return false;
		}
		for (int j = i + 1 ; j < dimension; j++) {
			if (equation[i][i] == 0) {
				return false;
			}
			equation[j][i] /= equation[i][i];
			for (int k = i + 1; k < dimension; k++) {
				equation[j][k] -= equation[j][i] * equation[i][k];
			}
		}
	}

	for (int i = 0; i < dimension; i++) {
		y[i] = b[i];
		for (int j = 0; j < i; j++) {
			y[i] -= equation[i][j] * y[j];
		}
	}

	for (int i = dimension - 1; i >= 0; i--) {
		result[i] = y[i];
		for (int j = dimension - 1; j >= i + 1; j--) {
			result[i] -= equation[i][j] * result[j];
		}
		if (equation[i][i] == 0) {
			return false;
		}
		result[i] /= equation[i][i];
	}

	return true;
}

vector<PreciseNumber> Equation::getResult() {
	return result;
}

PreciseNumber Equation::getResult(int index) {
	return result[index];
}
