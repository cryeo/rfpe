/*
 * Payoff.cpp
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

#include "Payoff.h"

Payoff::Payoff() {
	totalNumOfActions = 0;
	numPlayers = 0;
}

Payoff::~Payoff() {
}

void Payoff::setPayoffFromPayoff(Payoff &po, bool transition) {
	*this = po;

	if (transition) {
		if (numPlayers == 2) {
			reverse(numActionsOfPlayer.begin(), numActionsOfPlayer.end());
			for (int p = 0; p < numPlayers; p++) {
				for (int i = 0; i < numActionsOfPlayer[0]; i++) {
					for (int j = 0; j < numActionsOfPlayer[1]; j++) {
						setPayoff(p, ActionProfile(i, j), po.getPayoff(numPlayers - p - 1, ActionProfile(j, i)));
						setRawPayoff(p, ActionProfile(i, j), po.getRawPayoff(numPlayers - p - 1, ActionProfile(j, i)));
					}
				}
			}
		}
	}
}

Payoff::Payoff(int nP, vector<int> &nA) {
	configure(nP, nA);
}

void Payoff::configure(int nP, vector<int> &nA) {
	numPlayers = nP;
	numActionsOfPlayer = nA;
	totalNumOfActions = accumulate(numActionsOfPlayer.begin(), numActionsOfPlayer.end(), 1, multiplies<int>());

	payoff.resize(numPlayers);
	rawPayoff.resize(numPlayers);
	for (int i = 0; i < numPlayers; i++) {
		rawPayoff[i].resize(totalNumOfActions);
		payoff[i].resize(totalNumOfActions);
	}
}

void Payoff::setPayoff(int player, int ap, PreciseNumber reward) {
	payoff[player][ap] = reward;
}

void Payoff::setPayoff(int player, ActionProfile ap, PreciseNumber reward) {
	setPayoff(player, ap.getIndex(numActionsOfPlayer), reward);
}

void Payoff::setRawPayoff(int player, int ap, string reward) {
	rawPayoff[player][ap] = reward;
}

void Payoff::setRawPayoff(int player, ActionProfile ap, string reward) {
	setRawPayoff(player, ap.getIndex(numActionsOfPlayer), reward);
}

PreciseNumber Payoff::getPayoff(int player, int ap) {
	return payoff[player][ap];
}

PreciseNumber Payoff::getPayoff(int player, ActionProfile ap) {
	return getPayoff(player, ap.getIndex(numActionsOfPlayer));
}

string Payoff::getRawPayoff(int player, ActionProfile ap) {
	return getRawPayoff(player, ap.getIndex(numActionsOfPlayer));
}

string Payoff::getRawPayoff(int player, int ap) {
	return rawPayoff[player][ap];
}

void Payoff::setPayoffFromRawPayoff(map<string, PreciseNumber> variables) {
	for (int i = 0; i < numPlayers; i++) {
		for (int j = 0; j < numActionsOfPlayer[0]; j++) {
			for (int k = 0; k < numActionsOfPlayer[1]; k++) {
				ActionProfile ap(j, k);
				setPayoff(i, ap, parseEquation(rawPayoff[i][ap.getIndex(numActionsOfPlayer)], variables));
			}
		}
	}
}

int Payoff::getNumberOfActionProfiles() {
	return totalNumOfActions;
}

void Payoff::view(vector<Player> &players) {
	displayTitle("Payoff matrix");
	printf("* Raw Payoff matrix\n");
	for (int i = 0; i < numActionsOfPlayer[0]; i++) {
		for (int j = 0; j < numActionsOfPlayer[1]; j++) {
			ActionProfile ap(i, j);
			printf("(%15s, %15s)    ", rawPayoff[0][ap.getIndex(numActionsOfPlayer)].c_str(), rawPayoff[1][ap.getIndex(numActionsOfPlayer)].c_str());
		}
		printf("\n");
	}
	printf("* Payoff matrix\n");
	for (int i = 0; i < numActionsOfPlayer[0]; i++) {
		for (int j = 0; j < numActionsOfPlayer[1]; j++) {
			ActionProfile ap(i, j);
			gmp_printf("(% .15Ff, % .15Ff)    ", mpf_class(payoff[0][ap.getIndex(numActionsOfPlayer)]).get_mpf_t(), mpf_class(payoff[1][ap.getIndex(numActionsOfPlayer)]).get_mpf_t());
		}
		printf("\n");
	}
}
