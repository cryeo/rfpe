/*
 * RepeatedGame.h
 *
 *  Created on: 2012. 11. 21.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef BELIEF_H_
#include "Belief.h"
#endif

#ifndef EQUATION_H_
#include "Equation.h"
#endif

#ifndef BELIEFDIVISION_H_
#include "BeliefDivision.h"
#endif

#ifndef ENVIRONMENT_H_
#include "Environment.h"
#endif

#ifndef PARSER_H_
#include "Parser.h"
#endif

#ifndef PAYOFF_H_
#include "Payoff.h"
#endif

#ifndef PLAYER_H_
#include "Player.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

#ifndef REPEATEDGAME_H_
#define REPEATEDGAME_H_

#include <boost/timer.hpp>
// for 2 person game
#define PLAYER 0
#define OPPONENT 1

class RepeatedGame {
	int playerStates;
	int playerActions;
	int playerSignals;
	int opponentStates;
	int opponentSignals;

	int numPlayers;
	int numJointStates;
	int numJointActions;
	int numJointSignals;

	vector<int> numStatesOfPlayer;
	vector<int> numActionsOfPlayer;
	vector<int> numSignalsOfPlayer;

	vector<Player> players;
	Environment environment;
	Payoff payoff;

	vector<Equation> equations;
	vector< vector<PreciseNumber> > alphaVectors;
	vector<Belief> invariantDistribution;
	BeliefDivision initialBeliefDivision;
	BeliefDivision targetBeliefDivision;

	vector<Belief> candidates;

public:
	bool checkInvariantDistribution();

	map<string, PreciseNumber> globalVar;
	void setVariables(map<string, PreciseNumber> &vars);

	RepeatedGame();
	RepeatedGame(Environment &env, Payoff &po, vector<Player> &pls);
	virtual ~RepeatedGame();

	void configure(Environment &env, Payoff &po, vector<Player> &pls);

	void solveJointFSA();

	void setRewardEquation(int player);
	void solveRewardEquation(int player);
	vector< vector<PreciseNumber> > makeRewardEquation(int player);

	ActionProfile actionsOfJointStates(StateProfile sp);
	StateProfile transitionsOfJointStates(StateProfile sp, SignalProfile op);

	PreciseNumber getReward(int player, StateProfile sp);

	vector< vector<PreciseNumber> > getAlphaVectors();
	void makeAlphaVectors();

	vector<Belief> getExtremePoints();
	PreciseNumber getRewardFromAlphaVectorAndBelief(vector<PreciseNumber> &alphaVector, Belief &b);

	void solve();

	void makeBeliefDivision();
	void makeTargetBeliefDivision();

	void plotAlphaVectors(string dir, vector< vector<PreciseNumber> > aVs);

	Belief nextBelief(Belief bp, int action, int signal);
	vector<Belief> nextBeliefSet(vector<Belief> &bd, int action, int signal);
	vector<Belief> nextBeliefDivision(vector<Belief> &bd, int action, int signal);

	BeliefDivision getTargetBeliefDivision();

	BeliefDivision operatorW(BeliefDivision originalDivisions);
	BeliefDivision operatorU(BeliefDivision originalDivisions);

	int getNumberOfStatesOfPlayer(int pid);
	int getNumberOfActionsOfPlayer(int pid);
	int getNumberOfSignalsOfPlayer(int pid);

	string getNameOfStates(int player, int index);
	string getNameOfActions(int player, int index);
	string getNameOfSignals(int player, int index);

	int checkResilent();
	bool checkGlobalResilent();

	vector<Belief> getCandidates();

	bool checkSeminalBeliefDivision(BeliefDivision &closed);
	bool checkSeminalWithExtension(BeliefDivision &closed, int action, vector<int> nextTransition);

	bool checkSeminalBelief(Belief &b);
	bool checkSeminalWithExtension(Belief &b, int action, vector<int> nextTransition);

	bool checkSeminalBeliefSet(vector<Belief> &b);
	bool checkSeminalWithExtension(vector<Belief> &b, int action, vector<int> nextTransition);

	int checkNoEquilibrium();
	bool checkResilentFromBelief(Belief initial);


	vector<Belief> getInvariantDistribution();
	void makeInvariantDistribution();

	vector< vector<Belief> > Co(BeliefDivision &now);

	static const bool modeDebug = false;
	static const bool modeVerbose = false;

	template<typename T> static void debugTitle(T msg) { if (modeDebug) { cout << "************ " << msg << " **************" << "\n"; } }
	template<typename T> static void debug(T msg) { if (modeDebug) { cout << msg << "\n"; } }
	template<typename T> static void displayTitle(T msg) { cout << "************ " << msg << " **************" << "\n"; }
	template<typename T> static void display(T msg) { cout << msg << "\n"; }
	template<typename T> static void verboseTitle(T msg) { if (modeVerbose) { cout << "************ " << msg << " **************" << "\n"; } }
	template<typename T> static void verbose(T msg) { if (modeVerbose) { cout << msg << "\n"; } }
	template<typename T> static void error(T msg) { cerr << msg << "\n"; }
};

#endif /* REPEATEDGAME_H_ */
