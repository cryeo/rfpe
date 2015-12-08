/*
 * Util.cpp
 *
 *  Created on: 2012. 11. 27.
 *      Author: chaerim
 */

#include "Util.h"

int pow(int a, int b) {
	int res = 1;
	while (b-- > 0) res *= a;
	return res;
}

PreciseNumber cross(const P& a, const P& b) {
	return imag(conj(a) * b);
}

PreciseNumber dot(const P& a, const P& b) {
	return real(conj(a) * b);
}

int ccw(Belief ba, Belief bb, Belief bc) {
	P a(ba.get(0).get_d(), ba.get(1).get_d()), b(bb.get(0).get_d(), bb.get(1).get_d()), c(bc.get(0).get_d(), bc.get(1).get_d());
	b -= a; c -= a;
	if (cross(b, c) > 0)   return +1;       // counter clockwise
	if (cross(b, c) < 0)   return -1;       // clockwise
	if (dot(b, c) < 0)     return +2;       // c--a--b on line
	if (norm(b) < norm(c)) return -2;       // a--b--c on line
	return 0;
}

vector<Belief> convexHull(vector<Belief> ps) {
	int n = ps.size(), k = 0;

	if (n < 3) return ps;

	for (int i = 0; i < n - 1; i++) {
		for (int j = i + 1; j < n; j++) {
			if (ps[i] < ps[j]) {
				swap(ps[i], ps[j]);
			}
		}
	}

	vector<Belief> ch(2 * n);
	for (int i = 0; i < n; ch[k++] = ps[i++]) // lower-hull
		while (k >= 2 && ccw(ch[k - 2], ch[k - 1], ps[i]) <= 0) --k;
	for (int i = n - 2, t = k + 1; i >= 0; ch[k++] = ps[i--]) // upper-hull
		while (k >= t && ccw(ch[k - 2], ch[k - 1], ps[i]) <= 0) --k;
	ch.resize(k - 1);
	return ch;
}

string beliefToString(Belief &bs, int precision) {
	string format = "%." + toString(precision) + "Ff";

	string res = "";
	for (int j = 0; j < bs.getDimension(); j++) {
		char temp[255];
		gmp_sprintf(temp, format.c_str(), mpf_class(bs.get(j)).get_mpf_t());
		res += temp;
	}
	return res;
}

string beliefToString(vector<Belief> &bs, int precision) {
	string format = "%." + toString(precision) + "Ff";

	string res = "";
	for (int i = 0; i < bs.size(); i++) {
		for (int j = 0; j < bs[i].getDimension(); j++) {
			char temp[255];
			gmp_sprintf(temp, format.c_str(), mpf_class(bs[i].get(j)).get_mpf_t());
			res += temp;
		}
	}

	return res;
}

bool isSameWithPrecision(Belief a, Belief b, int digit) {
	if (a.getDimension() != b.getDimension()) return false;
	string format = "%." + toString(digit) + "Ff";
	for (int i = 0; i < a.getDimension(); i++) {
		char s1[255], s2[255];
		gmp_sprintf(s1, format.c_str(), mpf_class(a.get(i)).get_mpf_t());
		gmp_sprintf(s2, format.c_str(), mpf_class(b.get(i)).get_mpf_t());
		if (string(s1) != string(s2)) return false;
	}
	return true;
}

bool isSame(vector< vector<Belief> > &b1, vector< vector<Belief> > &b2) {
	if (b1.size() != b2.size()) {
		return false;
	}

	for (int i = 0; i < b1.size(); i++) {
		if (b1[i].size() != b2[i].size()) {
			return false;
		}
		sort(b1[i].begin(), b1[i].end());
		sort(b2[i].begin(), b2[i].end());
		for (int j = 0; j < b1[i].size(); j++) {
			if (b1[i][j] != b2[i][j]) {
				return false;
			}
		}
	}

	return true;
}


dd_MatrixPtr dd_VectorToMatrix(const vector< vector<PreciseNumber> > &input, dd_RepresentationType rep) {
	dd_MatrixPtr M = NULL;
	dd_rowrange i;
	dd_colrange j;
	dd_rowrange m = input.size();
	dd_colrange d = input[0].size();
	dd_NumberType NT = dd_Rational;

	M = dd_CreateMatrix(m, d);
	M->representation = rep;
	M->numbtype = NT;

	for (i = 0; i < m; i++) {
		for (j = 0; j < d; j++) {
			dd_set(M->matrix[i][j], input[i][j].get_mpq_t());
		}
	}
	return M;
}

dd_MatrixPtr dd_VectorToMatrix(const vector< vector<PreciseNumber> > &input, dd_RepresentationType rep, vector<bool> &isLinear) {
	dd_MatrixPtr M = NULL;
	dd_rowrange i;
	dd_colrange j;
	dd_rowrange m = input.size();
	dd_colrange d = input[0].size();
	dd_NumberType NT = dd_Rational;

	M = dd_CreateMatrix(m, d);
	M->representation = rep;
	M->numbtype = NT;
	if (isLinear.size() > 0) {
		for (int i = 1; i <= m; i++) {
			if (isLinear[i - 1]) {
				set_addelem(M->linset, i);
			}
		}
	}
	for (i = 0; i < m; i++) {
		for (j = 0; j < d; j++) {
			dd_set(M->matrix[i][j], input[i][j].get_mpq_t());
		}
	}
	return M;
}

vector< vector<PreciseNumber> > dd_MatrixToVector(dd_MatrixPtr &M) {
	vector< vector<PreciseNumber> > res;
	res.resize(M->rowsize);
	for (int i = 0; i < M->rowsize; i++) {
		res[i].resize(M->colsize);
		for (int j = 0; j < M->colsize; j++) {
			res[i][j] = PreciseNumber(M->matrix[i][j]);
		}
	}

	return res;
}

vector< vector<int> > dd_SetFamilyToVector(dd_SetFamilyPtr &GI) {
	vector< vector<int> > res(GI->famsize);

	for (long i = 0; i < GI->famsize; i++) {
		for (long j = 1; j <= GI->set[i][0]; j++) {
			if (set_member(j, GI->set[i])) {
				res[i].push_back(j - 1);
			}
		}
	}

	return res;
}

Inequality convertPointToInequality(const vector< vector<PreciseNumber> > &input)
{
	vector< vector<PreciseNumber> > res;
	vector<bool> il;

	dd_set_global_constants();
	dd_MatrixPtr M = dd_VectorToMatrix(input, dd_Generator);
	dd_PolyhedraPtr poly;
	dd_ErrorType err;
	// TODO this error is caused by number inconsistency
	poly = dd_DDMatrix2Poly(M, &err);
	if (err == dd_NoError) {
		dd_MatrixPtr A;
		A = dd_CopyInequalities(poly);
		res = dd_MatrixToVector(A);

		il.resize(res.size());

		for (int i = 1; i <= res.size(); i++) {
			if (set_member(i, A->linset)) il[i - 1] = true;
		}

		dd_FreeMatrix(A);
	}
	else {
		cout << "Error" << endl;
	}

	dd_FreeMatrix(M);
	dd_FreePolyhedra(poly);

	dd_free_global_constants();

	return Inequality(res, il);
}

vector<Belief> convertPointToBelief(const vector< vector<PreciseNumber> > &input) {
	vector<Belief> res;
	for (int i = 0; i < input.size(); i++) {
		if (input[i][0] == 0) continue; // ignore if result is extreme ray
		PreciseNumber lastBelief = 1;
		Belief b(input[i].size());
		for (int j = 1; j < input[i].size(); j++) {
			b.set(j - 1, input[i][j]);
			lastBelief -= input[i][j];
		}
		b.set(input[i].size() - 1, lastBelief);
		res.push_back(b);
	}

	return res;
}

vector<Belief> convertPointToBeliefWithReward(const vector< vector<PreciseNumber> > &input, vector<bool> &isPoint) {
	vector<Belief> res(input.size());
	for (int i = 0; i < input.size(); i++) {
		if (input[i][0] == 0) continue; // ignore if result is extreme ray
		isPoint[i] = true;
		PreciseNumber lastBelief = 1;
		Belief b(input[i].size() - 1);
		for (int j = 1; j < input[i].size() - 1; j++) {
			b.set(j - 1, input[i][j]);
			lastBelief -= input[i][j];
		}
		b.set(input[i].size() - 2, lastBelief);
		b.setReward(input[i][input[i].size() - 1]);
		res[i] = b;
	}

	return res;
}

vector< vector<PreciseNumber> > convertPointToPoint(const vector< vector<PreciseNumber> > &input)
{
	vector< vector<PreciseNumber> > res;
	dd_set_global_constants();
	dd_MatrixPtr M = dd_VectorToMatrix(input, dd_Generator);
	dd_ErrorType err;
	dd_rowset redrows = dd_RedundantRows(M, &err);
	dd_MatrixPtr M2 = dd_MatrixSubmatrix(M, redrows);
	res = dd_MatrixToVector(M2);
	dd_FreeMatrix(M2);
	dd_FreeMatrix(M);
	dd_free_global_constants();
	return res;
}

vector<Belief> getConvex(vector<Belief> &ps) {
	dd_set_global_constants();

	dd_rowrange i;
	dd_colrange j;
	dd_rowrange m = ps.size();
	dd_colrange d = ps[0].getDimension();

	dd_MatrixPtr M = dd_CreateMatrix(m, d);
	M->representation = dd_Generator;
	M->numbtype = dd_Rational;
	for (i = 0; i < m; i++) {
		dd_set(M->matrix[i][0], ONE.get_mpq_t());
		for (j = 1; j < d; j++) {
			dd_set(M->matrix[i][j], ps[i].get(j - 1).get_mpq_t());
		}
	}
	dd_ErrorType err;
	dd_rowset redrows = dd_RedundantRows(M, &err);
	dd_MatrixPtr M2 = dd_MatrixSubmatrix(M, redrows);
	vector<Belief> res;
	for (int i = 0; i < M2->rowsize; i++) {
		Belief b(d);
		PreciseNumber last = ONE;
		for (int j = 1; j < M2->colsize; j++) {
			b.set(j - 1, PreciseNumber(M2->matrix[i][j]));
			last -= PreciseNumber(M2->matrix[i][j]);
		}
		b.set(d - 1, last);
		res.push_back(b);
	}

	dd_FreeMatrix(M2);
	dd_FreeMatrix(M);
	dd_free_global_constants();

	return res;
}

pair< vector< vector<PreciseNumber> >, vector< vector<int> > > convertInequalityToPointWithIncidence(const Inequality &input)
{
	vector< vector<PreciseNumber> > res1;
	vector< vector<int> > res2;

	dd_set_global_constants();
	dd_MatrixPtr M = dd_VectorToMatrix(input.coefficient, dd_Inequality, const_cast<Inequality &>(input).isLinear);

	dd_PolyhedraPtr poly;
	dd_ErrorType err;
	poly = dd_DDMatrix2Poly(M, &err);

	dd_MatrixPtr G;
	G = dd_CopyGenerators(poly);
	res1 = dd_MatrixToVector(G);
	dd_FreeMatrix(G);

	dd_SetFamilyPtr GI;
	GI = dd_CopyIncidence(poly);
	res2 = dd_SetFamilyToVector(GI);
	dd_FreeSetFamily(GI);

	dd_FreeMatrix(M);
	dd_FreePolyhedra(poly);

	dd_free_global_constants();

	return make_pair(res1, res2);
}

vector< vector<PreciseNumber> > convertInequalityToPoint(const Inequality &input)
{
	vector< vector<PreciseNumber> > res1;
	dd_set_global_constants();
	dd_MatrixPtr M = dd_VectorToMatrix(const_cast<Inequality &>(input).coefficient, dd_Inequality, const_cast<Inequality &>(input).isLinear);
	dd_PolyhedraPtr poly;
	dd_ErrorType err;
	poly = dd_DDMatrix2Poly(M, &err);
	if (err == dd_NoError) {
		dd_MatrixPtr G;
		G = dd_CopyGenerators(poly);
		res1 = dd_MatrixToVector(G);
		dd_FreeMatrix(G);
	}
	dd_FreeMatrix(M);
	dd_FreePolyhedra(poly);
	dd_free_global_constants();
	return res1;
}


vector< vector<PreciseNumber> > intersection(const Inequality &inequalities1, const Inequality &inequalities2) {
	Inequality inequalities = inequalities1;
	inequalities.coefficient.insert(inequalities.coefficient.end(), inequalities2.coefficient.begin(), inequalities2.coefficient.end());
	inequalities.isLinear.insert(inequalities.isLinear.end(), inequalities2.isLinear.begin(), inequalities2.isLinear.end());
	return convertInequalityToPoint(inequalities);
}

vector<Belief> merge(const vector<Belief> &polytope1, const vector<Belief> &polytope2) {
	vector< vector<PreciseNumber> > points1 = convertBeliefToPoint(polytope1);
	vector< vector<PreciseNumber> > points2 = convertBeliefToPoint(polytope2);
	points1.insert(points1.end(), points2.begin(), points2.end());
	return convertPointToBelief(convertPointToPoint(points1));
}


vector< vector<PreciseNumber> > convertBeliefToPoint(const vector<Belief> &nextBeliefDivision) {
	vector< vector<PreciseNumber> > res(nextBeliefDivision.size());

	for (int i = 0; i < nextBeliefDivision.size(); i++) {
		res[i].resize(const_cast<Belief &>(nextBeliefDivision[i]).getDimension());
		res[i][0] = 1;
		for (int j = 0; j < const_cast<Belief &>(nextBeliefDivision[i]).getDimension() - 1; j++) {
			res[i][j + 1] = const_cast<Belief &>(nextBeliefDivision[i]).get(j);
		}
	}

	return res;
}

Inequality convertBeliefToInequality(const vector<Belief> &nextBeliefDivision) {
	vector< vector<PreciseNumber> > b2p = convertBeliefToPoint(nextBeliefDivision);
	return convertPointToInequality(b2p);
}

ostream& operator << (ostream& os, const vector<PreciseNumber> &v) {
	os << "(";
	for (int i = 0; i < v.size(); i++) {
		if (i > 0) os << ",";
		os << rationalToString(v[i]);
	}
	os << ")";
	return os;
}

ostream& operator << (ostream& os, const vector<Belief> &v) {
	for (int i = 0; i < v.size(); i++) {
		os << const_cast< vector<Belief> &>(v).at(i) << " ";
	}
	return os;
}
ostream& operator << (ostream& os, const Belief &b) {
	os << "(";
	for (int i = 0; i < const_cast<Belief &>(b).getDimension(); i++) {
		if (i > 0) os << ",";
		os << rationalToString(const_cast<Belief &>(b).get(i));
	}
	os << ") ";// << rationalToString(const_cast<Belief &>(b).getReward());
	return os;
}


ostream& operator << (ostream& os, const BeliefDivision &bd) {
	for (int i = 0; i < const_cast<BeliefDivision &>(bd).getNumberOfDivisions(); i++) {
		os << "** Division " << i << endl;
		for (int j = 0; j < const_cast<BeliefDivision &>(bd).getSizeOfDivisions(i); j++) {
			vector<Belief> d = const_cast<BeliefDivision &>(bd).getExtremePointsOfPolytope(i, j);
			os << "* Polytope " << j << endl;
			for (int k = 0; k < d.size(); k++) {
				os << d[k] << endl;
			}
		}
	}
	return os;
}

ostream& operator << (ostream& os, const Profile &p) {
	os << "(";
	for (int i = 0; i < const_cast<Profile &>(p).getSize(); i++) {
		if (i > 0) os << ",";
		os << const_cast<Profile &>(p)[i];
	}
	os << ")";
	return os;
}

string rationalToString(PreciseNumber val) {
	int len = 10;
	string fm = "%." + toString(len) + "Ff";
	char buf[255];

	gmp_sprintf(buf, fm.c_str(), mpf_class(val).get_mpf_t());
	stringstream s;
	s << buf;
	return s.str();
}

int toInteger(string val) {
	int res;
	istringstream s(val);
	s >> res;
	return res;
}

bool isInnerPointOfPolytope(Belief &point, const vector<Belief> &polytope) {
	bool result = false;

	dd_set_global_constants();

	dd_MatrixPtr M = NULL;
	dd_rowrange i;
	dd_colrange j;
	dd_rowrange m = polytope.size() + 1;
	dd_colrange d = const_cast<vector<Belief> &>(polytope)[0].getDimension() + 2;
	dd_NumberType NT = dd_Rational;

	M = dd_CreateMatrix(m, d);
	M->objective = dd_LPmax;
	M->numbtype = NT;

	for (i = 0; i < m - 1; i++) {
		dd_set(M->matrix[i][0], ZERO.get_mpq_t());
		dd_set(M->matrix[i][1], ONE.get_mpq_t());
		for (j = 0; j < d - 2; j++) {
			dd_set(M->matrix[i][j + 2], PreciseNumber(const_cast<vector<Belief> &>(polytope)[i].get(j) * -1).get_mpq_t());
		}
	}

	dd_set(M->matrix[m - 1][0], ONE.get_mpq_t());
	dd_set(M->matrix[m - 1][1], ONE.get_mpq_t());
	dd_set(M->rowvec[0], ZERO.get_mpq_t());
	dd_set(M->rowvec[1], PreciseNumber(ONE * -1).get_mpq_t());

	for (j = 0; j < d - 2; j++) {
		dd_set(M->matrix[i][j + 2], PreciseNumber(point.get(j) * -1).get_mpq_t());
		dd_set(M->rowvec[j + 2], PreciseNumber(point.get(j)).get_mpq_t());
	}

	dd_LPSolverType solver = dd_DualSimplex;
	dd_LPPtr lp;
	dd_ErrorType err;
	lp = dd_Matrix2LP(M, &err);
	if (err == dd_NoError) {
		dd_LPSolve(lp, solver, &err);
		if (err == dd_NoError) {
 			if (PreciseNumber(lp->optvalue) == ZERO) {
				result = true;
			}
		}
		dd_FreeLPData(lp);
	}
	else {
		cerr << "error" << endl;
	}

	dd_FreeMatrix(M);

	dd_free_global_constants();

	return result;
}

bool innerPolytope(vector<Belief> &innerPolytope, vector<Belief> &outerPolytope) {
	for (int i = 0; i < innerPolytope.size(); i++) {
		if (!isInnerPointOfPolytope(innerPolytope[i], outerPolytope)) return false;
	}

	return true;
/*
	Inequality inequality2 = convertBeliefToInequality(outerPolytope);

	int n = inequality2.coefficient.size();
	for (int k = 0; k < innerPolytope.size(); k++) {
		Belief pivot = innerPolytope[k];
		for (int i = 0; i < n; i++) {
			PreciseNumber res = inequality2.coefficient[i][0];
			for (int j = 1; j < inequality2.coefficient[i].size(); j++) {
				res += inequality2.coefficient[i][j] * pivot.get(j - 1);
			}

			if (inequality2.isLinear[i]) {
				if (equal(res, 0)) {

				}
				else {
					return false;
				}
			}
			else {
				if (equal(res, 0) || res > 0) {

				}
				else {
					return false;
				}
			}
		}
	}

	return true;
*/
/*
	int n = innerPolytope[0].getDimension();
	vector< vector<PreciseNumber> > input;
	for (int i = 0; i < innerPolytope.size(); i++) {
		vector<PreciseNumber> e;
		e.push_back(1);
		for (int j = 0; j < n - 1; j++) {
			e.push_back(innerPolytope[i].get(j));
		}
		input.push_back(e);
	}

	int ninput = 0;
	for (int i = 0; i < outerPolytope.size(); i++) {
		bool check = true;
		for (int j = 0; j < innerPolytope.size(); j++) {
			if (outerPolytope[i] == innerPolytope[j]) {
				check = false;
				break;
			}
		}
		if (check) {
			ninput++;
			vector<PreciseNumber> e;
			e.push_back(1);
			for (int j = 0; j < n - 1; j++) {
				e.push_back(outerPolytope[i].get(j));
			}
			input.push_back(e);
		}
	}
	dd_set_global_constants();
	dd_MatrixPtr M = dd_VectorToMatrix(input, dd_Generator);
	dd_ErrorType err;
	dd_rowset redrows = dd_RedundantRows(M, &err);
	int count = 0;
	for (int i = innerPolytope.size() + 1; i <= M->rowsize; i++) {
		if (set_member(i, redrows)) {
			count++;
		}
	}
	dd_FreeMatrix(M);

	dd_free_global_constants();

	return (count == ninput);
*/
}

bool isInnerPointOfPolytope2(Belief &point, vector<Belief> &polytope) {
	bool result = false;

	dd_set_global_constants();

	dd_MatrixPtr M = NULL;
	dd_rowrange i;
	dd_colrange j;
	dd_rowrange m = polytope.size() + 1;
	dd_colrange d = polytope[0].getDimension() + 2;
	dd_NumberType NT = dd_Rational;

	M = dd_CreateMatrix(m, d);
	M->objective = dd_LPmax;
	M->numbtype = NT;

	for (i = 0; i < m - 1; i++) {
		dd_set(M->matrix[i][0], ZERO.get_mpq_t());
		dd_set(M->matrix[i][1], ONE.get_mpq_t());
		for (j = 0; j < d - 2; j++) {
			dd_set(M->matrix[i][j + 2], PreciseNumber(polytope[i].get(j) * -1).get_mpq_t());
		}
	}

	dd_set(M->matrix[m - 1][0], ONE.get_mpq_t());
	dd_set(M->matrix[m - 1][1], ONE.get_mpq_t());
	dd_set(M->rowvec[0], ZERO.get_mpq_t());
	dd_set(M->rowvec[1], PreciseNumber(ONE * -1).get_mpq_t());

	for (j = 0; j < d - 2; j++) {
		dd_set(M->matrix[i][j + 2], PreciseNumber(point.get(j) * -1).get_mpq_t());
		dd_set(M->rowvec[j + 2], PreciseNumber(point.get(j)).get_mpq_t());
	}

	dd_LPSolverType solver = dd_DualSimplex;
	dd_LPPtr lp;
	dd_ErrorType err;
	lp = dd_Matrix2LP(M, &err);
	if (err == dd_NoError) {
		dd_LPSolve(lp, solver, &err);
		if (err == dd_NoError) {
			cout << rationalToString(PreciseNumber(lp->optvalue)) << endl;
 			if (PreciseNumber(lp->optvalue) == ZERO) {
				result = true;
			}
		}
	}

	dd_FreeLPData(lp);
	dd_FreeMatrix(M);

	dd_free_global_constants();

	return result;
}

bool innerPolytope2(vector<Belief> &innerPolytope, vector<Belief> &outerPolytope) {
	for (int i = 0; i < innerPolytope.size(); i++) {
		Belief now = innerPolytope[i];
		for (int j = 0; j < outerPolytope.size(); j++) {
			if (now == outerPolytope[j]) {
				now = outerPolytope[j];
				cout << "hi" << endl;
				break;
			}
		}
		cout << now << " ";
		if (isInnerPointOfPolytope(now, outerPolytope)) {
			cout << "inner" << endl;
		}
		else
			cout << "outer" << endl;
	}

	return true;
}

Belief centerOfPolytope(vector<Belief> &polytope) {
	int n = polytope.size(), m = polytope[0].getDimension();
	vector<PreciseNumber> res(m);
	Belief ret(m);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			res[j] += polytope[i].get(j);
		}
	}

	for (int i = 0; i < m; i++) {
		res[i] /= n;
		ret.set(i, res[i]);
	}

	return ret;
}
