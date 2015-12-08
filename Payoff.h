/*
 * Payoff.h
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef PARSER_H_
#include "Parser.h"
#endif

#ifndef PROFILE_H_
#include "Profile.h"
#endif

#ifndef PLAYER_H_
#include "Player.h"
#endif

#ifndef PAYOFF_H_
#define PAYOFF_H_

class Payoff {
	int numPlayers;
	int totalNumOfActions;
	vector<int> numActionsOfPlayer;

	vector< vector<PreciseNumber> > payoff;
	vector< vector<string> > rawPayoff;

public:
	Payoff();
	Payoff(int nP, vector<int> &nA);
	virtual ~Payoff();
	void configure(int nP, vector<int> &nA);

	void setRawPayoff(int player, ActionProfile ap, string reward);
	void setRawPayoff(int player, int ap, string reward);

	void setPayoff(int player, ActionProfile ap, PreciseNumber reward);
	void setPayoff(int player, int ap, PreciseNumber reward);

	void setPayoffFromRawPayoff(map<string, PreciseNumber> variables);
	void setPayoffFromPayoff(Payoff &po, bool transition = false);

	PreciseNumber getPayoff(int player, ActionProfile ap);
	PreciseNumber getPayoff(int player, int ap);

	string getRawPayoff(int player, ActionProfile ap);
	string getRawPayoff(int player, int ap);

	int getNumberOfActionProfiles();

	void view(vector<Player> &players);

	const static bool modeDebug = true;
	template<typename T> static void debugTitle(T msg) { if (modeDebug) { cout << "************ " << msg << " **************" << endl; } }
	template<typename T> static void debug(T msg) { if (modeDebug) { cout << msg << endl; } }
	template<typename T> static void displayTitle(T msg) { cout << "************ " << msg << " **************" << endl; }
	template<typename T>	static void display(T msg) { cout << msg << endl; }
	template<typename T> static void error(T msg) { cerr << msg << endl; }
};

#endif /* PAYOFF_H_ */
