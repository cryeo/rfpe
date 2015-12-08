/*
 * Automaton.h
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef AUTOMATON_H_
#define AUTOMATON_H_

class Automaton {
	int numStates;
	int numActions;
	int numSignals;
	vector<string> nameStates;
	vector<string> nameActions;
	vector<string> nameSignals;

	vector<int> actionChoice;
	vector< vector<int> > stateTransition;

	void visit(int v, vector< vector<int> >& scc, stack<int> &S, vector<bool> &inS, vector<int> &low, vector<int> &num, int& time);
public:
	Automaton();
	virtual ~Automaton();

	void setActionChoice(int state, int action);
	void setStateTransition(int state, int signal, int nextState);
	int getActionChoice(int state);
	int getStateTransition(int state, int signal);
	vector<int> getStateTransition(int state);
	int getNumberOfStates();
	int getNumberOfActions();
	int getNumberOfSignals();
	int getIndex(vector<string> &list, string &value);
	int getIndexOfStates(string state);
	int getIndexOfActions(string action);
	int getIndexOfSignals(string signal);
	void view();
	string getNameOfStates(int index);
	void setNameOfStates(vector<string> &names);
	string getNameOfActions(int index);
	void setNameOfActions(vector<string> &names);
	string getNameOfSignals(int index);
	void setNameOfSignals(vector<string> &names);
	vector<string> getNamesOfStates();
	vector<string> getNamesOfActions();
	vector<string> getNamesOfSignals();
	vector<int> getActionChoices();
	vector< vector<int> > getAllStateTransition();

	Automaton deleteTransientFSA();
};

bool operator == (const Automaton& lhs, const Automaton& rhs);


#endif /* AUTOMATON_H_ */
