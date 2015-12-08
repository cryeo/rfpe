/*
 * Belief.cpp
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

#include "Belief.h"

Belief::Belief() {
	dimension = 0;
}

Belief::Belief(int dimension) {
	configure(dimension);
}

Belief::~Belief() {

}

void Belief::configure(int dimension) {
	this->dimension = dimension;
	belief.resize(dimension);
}

void Belief::setReward(PreciseNumber value) {
	reward = value;
}

PreciseNumber Belief::getReward() {
	return reward;
}


int Belief::getDimension(){
	return dimension;
}

bool Belief::checkValidity()
{
	PreciseNumber sum = ZERO;
	for (int i = 0; i < dimension; i++) {
		if ((belief[i] < ZERO && !equal(belief[i], ZERO)) || (belief[i] > ONE && !equal(belief[i], ONE))) {
			return false;
		}
		sum += belief[i];
	}
	if (!equal(sum, ONE)) return false;
	return true;
}

PreciseNumber Belief::get(int index) {
	return belief[index];
}

void Belief::set(int index, PreciseNumber value) {
	if (abs(value) == 0) value = 0;
	belief[index] = value;
}

void Belief::view()
{
	cout << "(";
	for (int i = 0; i < dimension; i++) {
		if (i > 0) cout << ",";
		gmp_printf("%.15Ff", mpf_class(belief[i]).get_mpf_t());
	}
	cout << ")" << endl;
}

bool operator < (const Belief& lhs, const Belief& rhs) {
	for (int i = 0; i < const_cast<Belief &>(lhs).getDimension(); i++) {
		if (const_cast<Belief &>(lhs).get(i) == const_cast<Belief &>(rhs).get(i)) {
			continue;
		}
		else if (const_cast<Belief &>(lhs).get(i) < const_cast<Belief &>(rhs).get(i)) {
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}

bool operator > (const Belief& lhs, const Belief& rhs) {
	for (int i = 0; i < const_cast<Belief &>(lhs).getDimension(); i++) {
		if (const_cast<Belief &>(lhs).get(i) == const_cast<Belief &>(rhs).get(i)) {
			continue;
		}
		else if (const_cast<Belief &>(lhs).get(i) > const_cast<Belief &>(rhs).get(i)) {
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}

bool operator == (const Belief& lhs, const Belief& rhs) {
	if (const_cast<Belief &>(lhs).getDimension() != const_cast<Belief &>(rhs).getDimension()) return false;
	for (int i = 0; i < const_cast<Belief &>(rhs).getDimension(); i++) {
		if (!equal(const_cast<Belief &>(lhs).get(i), const_cast<Belief &>(rhs).get(i))) return false;
	}
	return true;
}


bool operator != (const Belief& lhs, const Belief& rhs) {
	if (const_cast<Belief &>(lhs).getDimension() != const_cast<Belief &>(rhs).getDimension()) return true;
	for (int i = 0; i < const_cast<Belief &>(rhs).getDimension(); i++) {
		if (!equal(const_cast<Belief &>(lhs).get(i), const_cast<Belief &>(rhs).get(i))) return true;
	}
	return false;
}

PreciseNumber operator - (const Belief& lhs, const Belief& rhs) {
	PreciseNumber res;
	for (int i = 0; i < const_cast<Belief &>(rhs).getDimension(); i++) {
		res += abs(const_cast<Belief &>(lhs).get(i) - const_cast<Belief &>(rhs).get(i));
	}
	return res;
}

bool equal(PreciseNumber a, PreciseNumber b) {
	return abs(a - b) < EPS;
}

PreciseNumber dist(const Belief &lhs, const Belief& rhs) {
	PreciseNumber res;
	for (int i = 0; i < const_cast<Belief &>(rhs).getDimension(); i++) {
		res += (const_cast<Belief &>(lhs).get(i) - const_cast<Belief &>(rhs).get(i)) * (const_cast<Belief &>(lhs).get(i) - const_cast<Belief &>(rhs).get(i));
	}
	return res;

}
