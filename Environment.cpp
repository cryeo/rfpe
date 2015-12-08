/*
 * Environment.cpp
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

#include "Environment.h"

Environment::Environment() {
	numPlayers = 0;
	totalNumOfActions = 0;
	totalNumOfSignals = 0;
	totalNumOfStates = 0;
	existCDOption = false;
}

Environment::Environment(PreciseNumber dR, vector<int> &nSt, vector<int> &nA, vector<int> &nS) {
	configure(dR, nSt, nA, nS);
}

Environment::~Environment() {
}

bool Environment::checkEnvironment() {
	bool result = true;
	PreciseNumber sum = ZERO;
	for (int i = 0; i < numActionsOfPlayer[0]; i++) {
		for (int j = 0; j < numActionsOfPlayer[1]; j++) {
			sum = ZERO;
			for (int k = 0; k < numSignalsOfPlayer[0]; k++) {
				for (int l = 0; l < numSignalsOfPlayer[1]; l++) {
					PreciseNumber prob = getSignalDistribution(ActionProfile(i, j), SignalProfile(k, l));
					if (prob < ZERO || equal(prob, ZERO) || prob > ONE || equal(prob, ONE)) {
						result = false;
						return result;
					}
					sum += getSignalDistribution(ActionProfile(i, j), SignalProfile(k, l));
				}
			}
			if (!equal(sum, ONE)) {
				result = false;
				return result;
			}
		}
	}

	if (this->existCorrelationDevice()) {
		sum = ZERO;
		for (int i = 0; i < numStatesOfPlayer[0]; i++) {
			for (int j = 0; j < numStatesOfPlayer[1]; j++) {
				PreciseNumber prob = getCorrelationDevice(StateProfile(i, j));
				if (prob < ZERO || equal(prob, ZERO) || prob > ONE || equal(prob, ONE)) {
					result = false;
					return result;
				}
				sum += getCorrelationDevice(StateProfile(i, j));
			}
		}
		if (!equal(sum, ONE)) {
			result = false;
			return result;
		}
	}

	return result;
}

PreciseNumber Environment::getCorrelationDevice(StateProfile sp) {
	return getCorrelationDevice(sp.getIndex(numStatesOfPlayer));
}
PreciseNumber Environment::getCorrelationDevice(int sp) {
	return correlationDevice[sp];
}

void Environment::configure(PreciseNumber dR, vector<int> &nSt, vector<int> &nA, vector<int> &nS) {
	discountRate = dR;
	numPlayers = nA.size();
	numStatesOfPlayer = nSt;
	numActionsOfPlayer = nA;
	numSignalsOfPlayer = nS;

	totalNumOfStates = accumulate(numStatesOfPlayer.begin(), numStatesOfPlayer.end(), 1, multiplies<int>());
	totalNumOfActions = accumulate(numActionsOfPlayer.begin(), numActionsOfPlayer.end(), 1, multiplies<int>());
	totalNumOfSignals = accumulate(numSignalsOfPlayer.begin(), numSignalsOfPlayer.end(), 1, multiplies<int>());

	correlationDevice.resize(totalNumOfStates);
	rawCorrelationDevice.resize(totalNumOfStates);

	signalDistribution.resize(totalNumOfActions);
	rawSignalDistribution.resize(totalNumOfActions);

	for (int i = 0; i < totalNumOfActions; i++) {
		signalDistribution[i].resize(totalNumOfSignals);
		rawSignalDistribution[i].resize(totalNumOfSignals);
	}
}

void Environment::setDiscountRate(PreciseNumber dR) {
	discountRate = dR;
}

void Environment::setSignalDistribution(ActionProfile ap, SignalProfile sp, PreciseNumber pr) {
	setSignalDistribution(ap.getIndex(numActionsOfPlayer), sp.getIndex(numSignalsOfPlayer), pr);
}

void Environment::setSignalDistribution(int ap, int sp, PreciseNumber pr) {
	signalDistribution[ap][sp] = pr;
}

void Environment::setRawSignalDistribution(ActionProfile ap, SignalProfile sp, string pr) {
	setRawSignalDistribution(ap.getIndex(numActionsOfPlayer), sp.getIndex(numSignalsOfPlayer), pr);
}

void Environment::setRawSignalDistribution(int ap, int sp, string pr) {
	rawSignalDistribution[ap][sp] = pr;
}

void Environment::setEnvironmentFromEnvironment(Environment &env, bool transition) {
	*this = env;
	if (transition) {
		if (numPlayers == 2) {
			reverse(numActionsOfPlayer.begin(), numActionsOfPlayer.end());
			reverse(numSignalsOfPlayer.begin(), numSignalsOfPlayer.end());

			for (int i = 0; i < numActionsOfPlayer[0]; i++) {
				for (int j = 0; j < numActionsOfPlayer[1]; j++) {
					for (int k = 0; k < numSignalsOfPlayer[0]; k++) {
						for (int l = 0; l < numSignalsOfPlayer[1]; l++) {
							setSignalDistribution(ActionProfile(i, j), SignalProfile(k, l), env.getSignalDistribution(ActionProfile(j, i), SignalProfile(l, k)));
							setRawSignalDistribution(ActionProfile(i, j), SignalProfile(k, l), env.getRawSignalDistribution(ActionProfile(j, i), SignalProfile(l, k)));
						}
					}
				}
			}
		}
	}
}

vector<int> Environment::getNumberOfActionsOfPlayer() {
	return numActionsOfPlayer;
}

vector<int> Environment::getNumberOfSignalsOfPlayer() {
	return numSignalsOfPlayer;
}


PreciseNumber Environment::getDiscountRate() {
	return discountRate;
}

PreciseNumber Environment::getSignalDistribution(ActionProfile ap, SignalProfile sp) {
	return getSignalDistribution(ap.getIndex(numActionsOfPlayer), sp.getIndex(numSignalsOfPlayer));
}

PreciseNumber Environment::getSignalDistribution(int ap, int sp) {
	return signalDistribution[ap][sp];
}

string Environment::getRawSignalDistribution(ActionProfile ap, SignalProfile sp) {
	return getRawSignalDistribution(ap.getIndex(numActionsOfPlayer), sp.getIndex(numSignalsOfPlayer));
}

string Environment::getRawSignalDistribution(int ap, int sp) {
	return rawSignalDistribution[ap][sp];
}

int Environment::getNumberOfActionProfiles() {
	return totalNumOfActions;
}

int Environment::getNumberOfSignalProfiles() {
	return totalNumOfSignals;
}

int Environment::getNumberOfStateProfiles() {
	return totalNumOfStates;
}

vector<int> Environment::getNumberOfStatesOfPlayer() {
	return numStatesOfPlayer;
}

void Environment::view(vector<Player> &players) {
	displayTitle("Environment");
	gmp_printf("* Discount rate : %.10Ff\n", mpf_class(discountRate).get_mpf_t());
	printf("* Raw signal distribution\n");
	viewRawSignalDistribution(players);
	printf("* Signal distribution\n");
	viewSignalDistribution(players);
}

void Environment::viewCorrelationDevice() {
	if (this->existCorrelationDevice()) {
		printf("* Correlation Device\n");
		for (int i = 0; i < numStatesOfPlayer[0]; i++) {
			for (int j = 0; j < numStatesOfPlayer[1]; j++) {
				StateProfile sp(i, j);
				gmp_printf("% .10Ff ", mpf_class(correlationDevice[sp.getIndex(numStatesOfPlayer)]).get_mpf_t());
			}
			printf("\n");
		}
	}
	else {
		printf("* Correlation Device : Invariant Distribution\n");
	}
}

void Environment::viewRawCorrelationDevice() {
	if (this->existCorrelationDevice()) {
		printf("* Raw Correlation Device\n");
		for (int i = 0; i < numStatesOfPlayer[0]; i++) {
			for (int j = 0; j < numStatesOfPlayer[1]; j++) {
				StateProfile sp(i, j);
				printf("%15s ", rawCorrelationDevice[sp.getIndex(numStatesOfPlayer)].c_str());
			}
			printf("\n");
		}
	}
	else {
		printf("* Correlation Device : Invariant Distribution\n");
	}
}


void Environment::viewRawSignalDistribution(vector<Player> &players) {
	for (int i = 0; i < numActionsOfPlayer[0]; i++) {
		for (int j = 0; j < numActionsOfPlayer[1]; j++) {
			viewRawSignalDistribution(ActionProfile(i, j), players);
		}
	}
}

void Environment::viewRawSignalDistribution(ActionProfile ap, vector<Player> &players) {
	int indexAction = ap.getIndex(numActionsOfPlayer);
	printf("Action profile (%s, %s)\n", players[0].getNameOfActions(ap[0]).c_str(), players[1].getNameOfActions(ap[1]).c_str());
	for (int i = 0; i < numSignalsOfPlayer[0]; i++) {
		for (int j = 0; j < numSignalsOfPlayer[1]; j++) {
			SignalProfile sp(i, j);
			printf("%15s ", rawSignalDistribution[ap.getIndex(numActionsOfPlayer)][sp.getIndex(numSignalsOfPlayer)].c_str());
		}
		printf("\n");
	}
}

void Environment::viewSignalDistribution(vector<Player> &players) {
	for (int i = 0; i < numActionsOfPlayer[0]; i++) {
		for (int j = 0; j < numActionsOfPlayer[1]; j++) {
			viewSignalDistribution(ActionProfile(i, j), players);
		}
	}
}
void Environment::viewSignalDistribution(ActionProfile ap, vector<Player> &players) {
	int indexAction = ap.getIndex(numActionsOfPlayer);
	printf("Action profile (%s, %s)\n", players[0].getNameOfActions(ap[0]).c_str(), players[1].getNameOfActions(ap[1]).c_str());
	for (int i = 0; i < numSignalsOfPlayer[0]; i++) {
		for (int j = 0; j < numSignalsOfPlayer[1]; j++) {
			SignalProfile sp(i, j);
			gmp_printf("% .10Ff ", mpf_class(signalDistribution[ap.getIndex(numActionsOfPlayer)][sp.getIndex(numSignalsOfPlayer)]).get_mpf_t());
		}
		printf("\n");
	}
}

void Environment::setSignalDistributionFromRawSignalDistribution(map<string, PreciseNumber> variables) {
	for (int i = 0; i < numActionsOfPlayer[0]; i++) {
		for (int j = 0; j < numActionsOfPlayer[1]; j++) {
			for (int k = 0; k < numSignalsOfPlayer[0]; k++) {
				for (int l = 0; l < numSignalsOfPlayer[1]; l++) {
					ActionProfile ap(i, j);
					SignalProfile sp(k, l);
					setSignalDistribution(ap, sp, parseEquation(rawSignalDistribution[ap.getIndex(numActionsOfPlayer)][sp.getIndex(numSignalsOfPlayer)], variables));
				}
			}
		}
	}
}

void Environment::setOptionOfCorrelationDevice(bool exists) {
	existCDOption = exists;
}

bool Environment::existCorrelationDevice() {
	return existCDOption;
}

void Environment::setRawCorrelationDevice(StateProfile sp, string v) {
	setRawCorrelationDevice(sp.getIndex(numStatesOfPlayer), v);
}

void Environment::setRawCorrelationDevice(int sp, string v) {
	rawCorrelationDevice[sp] = v;
}

vector<string> Environment::getRawCorrelationDevice() {
	return rawCorrelationDevice;
}

void Environment::setCorrelationDevice(StateProfile sp, PreciseNumber v) {
	setCorrelationDevice(sp.getIndex(numStatesOfPlayer), v);
}

void Environment::setCorrelationDevice(int sp, PreciseNumber v) {
	correlationDevice[sp] = v;
}

vector<PreciseNumber> Environment::getCorrelationDevice() {
	return correlationDevice;
}

void Environment::setCorrelationDeviceFromRawCorrelationDevice(map<string, PreciseNumber> variables) {
	for (int i = 0; i < numStatesOfPlayer[0]; i++) {
		for (int j = 0; j < numStatesOfPlayer[1]; j++) {
			StateProfile sp(i, j);
			setCorrelationDevice(sp, parseEquation(rawCorrelationDevice[sp.getIndex(numStatesOfPlayer)], variables));
		}
	}
}
