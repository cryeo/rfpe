/*
 * Automaton.cpp
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

#include "Automaton.h"

Automaton::Automaton() {
	numSignals = 0;
	numActions = 0;
	numStates = 0;
}

Automaton::~Automaton() {

}

string Automaton::getNameOfStates(int index) {
	return nameStates[index];
}

void Automaton::setNameOfStates(vector<string> &names) {
	nameStates = names;
	numStates = names.size();
	actionChoice.resize(numStates);
	stateTransition.resize(numStates);
}

string Automaton::getNameOfActions(int index) {
	return nameActions[index];
}

void Automaton::setNameOfActions(vector<string> &names) {
	nameActions = names;
	numActions = names.size();
}

string Automaton::getNameOfSignals(int index) {
	return nameSignals[index];
}

void Automaton::setNameOfSignals(vector<string> &names) {
	nameSignals = names;
	numSignals = names.size();
	for (int i = 0; i < numStates; i++) {
		stateTransition[i].resize(numSignals);
	}
}

void Automaton::setActionChoice(int state, int action) {
	this->actionChoice[state] = action;
}

void Automaton::setStateTransition(int state, int signal, int nextState) {
	this->stateTransition[state][signal] = nextState;
}

int Automaton::getActionChoice(int state) {
	return actionChoice[state];
}

int Automaton::getStateTransition(int state, int signal) {
	return stateTransition[state][signal];
}

vector<int> Automaton::getStateTransition(int state) {
	return stateTransition[state];
}

int Automaton::getNumberOfStates() {
	return numStates;
}

int Automaton::getNumberOfActions() {
	return numActions;
}

int Automaton::getNumberOfSignals() {
	return numSignals;
}

int Automaton::getIndex(vector<string> &list, string &value) {
	return int(find(list.begin(), list.end(), value) - list.begin());
}

int Automaton::getIndexOfStates(string state) {
	return getIndex(nameStates, state);
}

int Automaton::getIndexOfActions(string action) {
	return getIndex(nameActions, action);
}

int Automaton::getIndexOfSignals(string signal) {
	return getIndex(nameSignals, signal);
}

void Automaton::view() {
	printf("* States  : %d\n", numStates);
	cout << "  ";
	copy(nameStates.begin(), nameStates.end(), ostream_iterator<string>(cout, " "));
	cout << endl;
	printf("* Actions : %d\n", numActions);
	cout << "  ";
	copy(nameActions.begin(), nameActions.end(), ostream_iterator<string>(cout, " "));
	cout << endl;
	printf("* Signals : %d\n", numSignals);
	cout << "  ";
	copy(nameSignals.begin(), nameSignals.end(), ostream_iterator<string>(cout, " "));
	cout << endl;
	printf("* Action choice (State -> Action)\n");
	for (int i = 0; i < numStates; i++) {
		printf("  %s -> %s\n", getNameOfStates(i).c_str(), getNameOfActions(actionChoice[i]).c_str());
	}
	printf("* State transition (State Signal -> State)\n");
	for (int i = 0; i < numStates; i++) {
		for (int j = 0; j < numSignals; j++) {
			printf("  %s %s -> %s\n", getNameOfStates(i).c_str(), getNameOfSignals(j).c_str(), getNameOfStates(stateTransition[i][j]).c_str());
		}
	}
}

vector<string> Automaton::getNamesOfStates() {
	return nameStates;
}

vector<string> Automaton::getNamesOfActions() {
	return nameActions;
}

vector<string> Automaton::getNamesOfSignals() {
	return nameSignals;
}

vector<int> Automaton::getActionChoices() {
	return actionChoice;
}

vector< vector<int> > Automaton::getAllStateTransition() {
	return stateTransition;
}

bool operator == (const Automaton& lhs, const Automaton& rhs) {
	if (const_cast<Automaton &>(lhs).getNumberOfStates() != const_cast<Automaton &>(rhs).getNumberOfStates()) return false;
	if (const_cast<Automaton &>(lhs).getNumberOfActions() != const_cast<Automaton &>(rhs).getNumberOfActions()) return false;
	if (const_cast<Automaton &>(lhs).getNumberOfSignals() != const_cast<Automaton &>(rhs).getNumberOfSignals()) return false;
	if (const_cast<Automaton &>(lhs).getNamesOfStates() != const_cast<Automaton &>(rhs).getNamesOfStates()) return false;
	if (const_cast<Automaton &>(lhs).getNamesOfActions() != const_cast<Automaton &>(rhs).getNamesOfActions()) return false;
	if (const_cast<Automaton &>(lhs).getNamesOfSignals() != const_cast<Automaton &>(rhs).getNamesOfSignals()) return false;
	if (const_cast<Automaton &>(lhs).getActionChoices() != const_cast<Automaton &>(rhs).getActionChoices()) return false;
	if (const_cast<Automaton &>(lhs).getAllStateTransition() != const_cast<Automaton &>(rhs).getAllStateTransition()) return false;
	return true;
}


void Automaton::visit(int v, vector< vector<int> >& scc, stack<int> &S, vector<bool> &inS, vector<int> &low, vector<int> &num, int& time) {
	low[v] = num[v] = ++time;
	S.push(v); inS[v] = true;
	for (int i = 0; i < numSignals; i++) {
		int w = stateTransition[v][i];
		if (num[w] == 0) {
			visit(w, scc, S, inS, low, num, time);
			low[v] = min(low[v], low[w]);
		} else if (inS[w])
		low[v] = min(low[v], num[w]);
	}
	if (low[v] == num[v]) {
		scc.push_back(vector<int>());
		while (1) {
			int w = S.top(); S.pop(); inS[w] = false;
			scc.back().push_back(w);
			if (v == w) break;
		}
	}
}

Automaton Automaton::deleteTransientFSA() {
	vector< vector<int> > scc;
	vector<int> num(numStates), low(numStates);
	stack<int> S;
	vector<bool> inS(numStates);
	int time = 0;
	for (int u = 0; u < numStates; u++) {
		if (num[u] == 0) {
			visit(u, scc, S, inS, low, num, time);
		}
	}

	int numSCCs = scc.size();

	if (numSCCs == 1) {
		return *this;
	}

	vector<int> stateToSCC(numStates, -1);
	for (int i = 0; i < numSCCs; i++) {
		for (int j = 0; j < scc[i].size(); j++) {
			stateToSCC[scc[i][j]] = i;
		}
	}

	vector<bool> hasOutDegree(numSCCs, false);
	for (int i = 0; i < numSCCs; i++) {
		for (int j = 0; j < scc[i].size(); j++) {
			for (int o = 0; o < numSignals; o++) {
				if (stateToSCC[stateTransition[scc[i][j]][o]] != i) {
					hasOutDegree[i] = true;
					break;
				}
			}
			if (hasOutDegree[i]) break;
		}
	}

	vector<bool> isTransient(numStates, true);
	vector<int> newIndex(numStates, -1);
	vector<string> newNameStates;

	int numNewStates = 0;
	for (int i = 0; i < numSCCs; i++) {
		if (hasOutDegree[i]) continue;
		for (int j = 0; j < scc[i].size(); j++) {
			isTransient[scc[i][j]] = false;
			newIndex[scc[i][j]] = numNewStates;
			newNameStates.push_back(nameStates[scc[i][j]]);
			numNewStates++;
		}
	}

	Automaton newFSA;
	newFSA.setNameOfStates(newNameStates);
	newFSA.setNameOfActions(nameActions);
	newFSA.setNameOfSignals(nameSignals);
	for (int i = 0; i < numStates; i++) {
		if (isTransient[i]) continue;
		newFSA.setActionChoice(newIndex[i], actionChoice[i]);
		for (int o = 0; o < numSignals; o++) {
			newFSA.setStateTransition(newIndex[i], o, newIndex[stateTransition[i][o]]);
		}
	}

	return newFSA;
}
