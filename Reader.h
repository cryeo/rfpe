/*
 * IO.h
 *
 *  Created on: 2012. 12. 4.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef AUTOMATON_H_
#include "Automaton.h"
#endif

#ifndef ENVIRONMENT_H_
#include "Environment.h"
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

#ifndef READER_H_
#define READER_H_

#include <boost/regex.hpp>

enum READSTATUS {
	NOERROR = 0,
	FILENOTFOUND = 1,
	ITEMNOTFOUND = 2,
	SYNTAXERROR = 3
};

class Reader {
private:
	ifstream fin;

	string title;
	PreciseNumber discountRate;
	vector<string> players;
	map<string, PreciseNumber> variables;
	vector<string> rawCorrelationDevice;
	int numPlayers;

	vector<int> numStatesOfPlayer;
	vector<int> numActionsOfPlayer;
	vector<int> numSignalsOfPlayer;

public:
	Reader();
	Reader(string nameDataFile);
	virtual ~Reader();

	READSTATUS read(string &tit, Environment &env, Payoff &po, vector<Player> &pls);

	READSTATUS readTitle();
	READSTATUS readDiscountRate();
	READSTATUS readPlayers();
	READSTATUS readVariables();
	void setVariable(string name, PreciseNumber value);
	READSTATUS readAutomaton(string namePlayer, Automaton &am);
	READSTATUS readItem(string tag, vector<string> &data, bool startFirst = false);
	READSTATUS readPayoffMatrix(Payoff &po, vector<Player> &pls);
	READSTATUS readSignalDistribution(Environment &env, vector<Player> &pls);
	READSTATUS readCorrelationDevice(Environment &env);
	int getIndex(vector<string> &list, string &value);
	int getIndexOfPlayers(string player);

	map<string, PreciseNumber> getVariables();

	string numberToRational(string number);
};

#endif /* READER_H_ */
