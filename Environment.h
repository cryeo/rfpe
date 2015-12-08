/*
 * Environment.h
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

#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

class Environment {
	PreciseNumber discountRate;

	int numPlayers;
	int totalNumOfStates;
	int totalNumOfActions;
	int totalNumOfSignals;

	vector<int> numStatesOfPlayer;
	vector<int> numActionsOfPlayer;
	vector<int> numSignalsOfPlayer;

	vector< vector<PreciseNumber> > signalDistribution;
	vector< vector<string> > rawSignalDistribution;

	bool existCDOption;
	vector<string> rawCorrelationDevice;
	vector<PreciseNumber> correlationDevice;
public:
	Environment();
	Environment(PreciseNumber dR, vector<int> &nSt, vector<int> &nA, vector<int> &nS);
	virtual ~Environment();
	void configure(PreciseNumber dR, vector<int> &nSt, vector<int> &nA, vector<int> &nS);
	void setDiscountRate(PreciseNumber dR);

	void setRawSignalDistribution(ActionProfile ap, SignalProfile sp, string pr);
	void setRawSignalDistribution(int ap, int sp, string pr);

	void setSignalDistribution(ActionProfile ap, SignalProfile sp, PreciseNumber pr);
	void setSignalDistribution(int ap, int sp, PreciseNumber pr);

	void setEnvironmentFromEnvironment(Environment &env, bool transition = false);

	bool checkEnvironment();

	void setOptionOfCorrelationDevice(bool exists);
	bool existCorrelationDevice();

	void setRawCorrelationDevice(StateProfile sp, string v);
	void setRawCorrelationDevice(int sp, string v);
	void setCorrelationDevice(StateProfile sp, PreciseNumber v);
	void setCorrelationDevice(int sp, PreciseNumber v);
	void setCorrelationDeviceFromRawCorrelationDevice(map<string, PreciseNumber> variables);

	vector<string> getRawCorrelationDevice();
	vector<PreciseNumber> getCorrelationDevice();
	PreciseNumber getCorrelationDevice(StateProfile sp);
	PreciseNumber getCorrelationDevice(int sp);


	PreciseNumber getDiscountRate();
	PreciseNumber getSignalDistribution(ActionProfile ap, SignalProfile sp);
	PreciseNumber getSignalDistribution(int ap, int sp);
	string getRawSignalDistribution(ActionProfile ap, SignalProfile sp);
	string getRawSignalDistribution(int ap, int sp);

	int getNumberOfActionProfiles();
	int getNumberOfSignalProfiles();
	int getNumberOfStateProfiles();
	vector<int> getNumberOfActionsOfPlayer();
	vector<int> getNumberOfSignalsOfPlayer();
	vector<int> getNumberOfStatesOfPlayer();

	void view(vector<Player> &players);
	void viewSignalDistribution(vector<Player> &players);
	void viewSignalDistribution(ActionProfile ap, vector<Player> &players);

	void viewRawSignalDistribution(vector<Player> &players);
	void viewRawSignalDistribution(ActionProfile ap, vector<Player> &players);

	void viewCorrelationDevice();
	void viewRawCorrelationDevice();

	void setSignalDistributionFromRawSignalDistribution(map<string, PreciseNumber> variables);

	const static bool modeDebug = false;
	template<typename T> static void debugTitle(T msg) { if (modeDebug) { cout << "************ " << msg << " **************" << "\n"; } }
	template<typename T> static void debug(T msg) { if (modeDebug) { cout << msg << "\n"; } }
	template<typename T> static void displayTitle(T msg) { cout << "************ " << msg << " **************" << "\n"; }
	template<typename T> static void display(T msg) { cout << msg << "\n"; }
	template<typename T> static void error(T msg) { cerr << msg << "\n"; }
};

#endif /* ENVIRONMENT_H_ */
