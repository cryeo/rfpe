/*
 * Player.h
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef AUTOMATON_H_
#include "Automaton.h"
#endif

#ifndef BELIEF_H_
#include "Belief.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

#ifndef PLAYER_H_
#define PLAYER_H_

class Player {
private:
	string namePlayer;
	Automaton strategy;
	Belief belief;
public:
	Player();
	Player(string name, Automaton &st);
	virtual ~Player();
	void configure(string name, Automaton &st);
	void setAutomaton(Automaton &st);
	int getActionChoice(int state);
	int getNumberOfStates();
	int getNumberOfActions();
	int getNumberOfSignals();
	void setNameOfPlayer(string name);
	string getNameOfPlayer();
	string getNameOfStates(int index);
	string getNameOfActions(int index);
	string getNameOfSignals(int index);
	int getIndexOfStates(string state);
	int getIndexOfActions(string action);
	int getIndexOfSignals(string signal);

	int getStateTransition(int state, int signal);
	Automaton getAutomaton();

	void view();

	const static bool modeDebug = true;
	template<typename T> static void debugTitle(T msg) { if (modeDebug) { cout << "************ " << msg << " **************" << endl; } }
	template<typename T> static void debug(T msg) { if (modeDebug) { cout << msg << endl; } }
	template<typename T> static void displayTitle(T msg) { cout << "************ " << msg << " **************" << endl; }
	template<typename T>	static void display(T msg) { cout << msg << endl; }
	template<typename T> static void error(T msg) { cerr << msg << endl; }
};

#endif /* PLAYER_H_ */
