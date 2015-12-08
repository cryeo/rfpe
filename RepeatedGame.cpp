/*
 * RepeatedGame.cpp
 *
 *  Created on: 2012. 11. 21.
 *      Author: chaerim
 */

#include "RepeatedGame.h"
/**
 * TODO　プレイヤーごとに対応しているが，プレイヤー1以外にはちゃんと計算できてない様．
 * Joint FSAのEquationを構成する部分が問題あると推測．
 */
bool RepeatedGame::checkInvariantDistribution() {
	vector<Belief> id = getInvariantDistribution();
	for (int i = 0; i < playerStates; i++) {
		if (!id[i].checkValidity()) return false;
	}
	return true;
}

RepeatedGame::RepeatedGame() {
	playerStates = 0;
	playerActions = 0;
	playerSignals = 0;
	opponentStates = 0;
	opponentSignals = 0;

	numPlayers = 0;
	numJointStates = 0;
	numJointActions = 0;
	numJointSignals = 0;
}

RepeatedGame::RepeatedGame(Environment &env, Payoff &po, vector<Player> &pls) {
	configure(env, po, pls);
}

RepeatedGame::~RepeatedGame() {

}

void RepeatedGame::setVariables(map<string, PreciseNumber> &vars) {
	globalVar = vars;
}

void RepeatedGame::configure(Environment &env, Payoff &po, vector<Player> &pls) {
	environment = env;
	payoff = po;
	numPlayers = pls.size();
	players = pls;
	equations.resize(numPlayers);
	numStatesOfPlayer.resize(numPlayers);

	playerStates = players[PLAYER].getNumberOfStates();
	playerActions = players[PLAYER].getNumberOfActions();
	playerSignals = players[PLAYER].getNumberOfSignals();
	opponentStates = players[OPPONENT].getNumberOfStates();
	opponentSignals = players[OPPONENT].getNumberOfSignals();

	for (int i = 0; i < numPlayers; i++) {
		numStatesOfPlayer[i] = players[i].getNumberOfStates();
	}
	numActionsOfPlayer = env.getNumberOfActionsOfPlayer();
	numSignalsOfPlayer = env.getNumberOfSignalsOfPlayer();

	numJointStates = accumulate(numStatesOfPlayer.begin(), numStatesOfPlayer.end(), 1, multiplies<int>());
	numJointActions = accumulate(numActionsOfPlayer.begin(), numActionsOfPlayer.end(), 1, multiplies<int>());
	numJointSignals = accumulate(numSignalsOfPlayer.begin(), numSignalsOfPlayer.end(), 1, multiplies<int>());
}

int RepeatedGame::getNumberOfStatesOfPlayer(int pid) {
	return players[pid].getNumberOfStates();
}

int RepeatedGame::getNumberOfActionsOfPlayer(int pid) {
	return players[pid].getNumberOfActions();
}

int RepeatedGame::getNumberOfSignalsOfPlayer(int pid) {
	return players[pid].getNumberOfSignals();
}

string RepeatedGame::getNameOfStates(int player, int index) {
	return players[player].getNameOfStates(index);
}

string RepeatedGame::getNameOfActions(int player, int index) {
	return players[player].getNameOfActions(index);
}

string RepeatedGame::getNameOfSignals(int player, int index) {
	return players[player].getNameOfSignals(index);
}

void RepeatedGame::solveJointFSA() {
	setRewardEquation(PLAYER);
	solveRewardEquation(PLAYER);
}

void RepeatedGame::solveRewardEquation(int player) {
	equations[player].solve();
	debugTitle("Solve joint-FSA equations for " + players[player].getNameOfPlayer());
	debug(equations[player].getResult());
}

void RepeatedGame::setRewardEquation(int player) {
	vector< vector<PreciseNumber> > rewardEquation = makeRewardEquation(player);
	equations[player].configure(numJointStates, rewardEquation);
}

ActionProfile RepeatedGame::actionsOfJointStates(StateProfile sp) {
	int actions = 0;
	for (int p = 0; p < sp.getSize(); p++) {
		actions *= players[p].getNumberOfActions();
		actions += players[p].getActionChoice(sp[p]);
	}
	return ActionProfile(actions, numActionsOfPlayer);
}

StateProfile RepeatedGame::transitionsOfJointStates(StateProfile sp, SignalProfile op) {
	int transitions = 0;
	for (int p = 0; p < numPlayers; p++) {
		transitions *= players[p].getNumberOfStates();
		transitions += players[p].getStateTransition(sp[p], op[p]);
	}
	return StateProfile(transitions, numStatesOfPlayer);
}

vector< vector<PreciseNumber> > RepeatedGame::makeRewardEquation(int player) {
	vector< vector<PreciseNumber> > equation(numJointStates, vector<PreciseNumber>(numJointStates + 1));
	for (int states = 0; states < numJointStates; states++) {
		StateProfile sp(states, numStatesOfPlayer);
		ActionProfile ap = actionsOfJointStates(sp);
		equation[states][numJointStates] = payoff.getPayoff(player, ap);
		for (int signals = 0; signals < numJointSignals; signals++) {
			SignalProfile op(signals, numSignalsOfPlayer);
			StateProfile transitions = transitionsOfJointStates(sp, op);
			equation[states][transitions.getIndex(numStatesOfPlayer)] += environment.getSignalDistribution(ap, op);
		}
	}

	for (int i = 0; i < numJointStates; i++) {
		for (int j = 0; j < numJointStates; j++) {
			equation[i][j] = environment.getDiscountRate() * equation[i][j] * -1;
		}
		equation[i][i] = 1 + equation[i][i];
	}

	return equation;
}

void RepeatedGame::makeInvariantDistribution() {
	vector<PreciseNumber> normalCondition(numJointStates);

	if (!environment.existCorrelationDevice()) {
		printf("Invariant distribution as correlation device.\n");
		vector< vector<PreciseNumber> > nc(numJointStates, vector<PreciseNumber>(numJointStates));

		for (int states = 0; states < numJointStates; states++) {
			StateProfile sp(states, numStatesOfPlayer);
			ActionProfile ap = actionsOfJointStates(sp);
			for (int signals = 0; signals < numJointSignals; signals++) {
				SignalProfile op(signals, numSignalsOfPlayer);
				StateProfile transitions = transitionsOfJointStates(sp, op);
				nc[states][transitions.getIndex(numStatesOfPlayer)] += environment.getSignalDistribution(ap, op);
			}
		}

		int loop = 0;
		while (true) {
			loop++;
			vector< vector<PreciseNumber> > nc2(numJointStates, vector<PreciseNumber>(numJointStates, ZERO));
			bool check = true;
			for (int i = 0; i < numJointStates; i++) {
				for (int j = 0; j < numJointStates; j++) {
					for (int k = 0; k < numJointStates; k++) {
						nc2[i][j] += nc[i][k] * nc[k][j];
					}
					if (!equal(nc[i][j], nc2[i][j])) check = false;
				}
			}
			nc = nc2;
			if (check) break;
		}

		if (modeDebug) {
			for (int i = 0; i < numJointStates; i++) {
				for (int j = 0; j < numJointStates; j++) {
					cout << rationalToString(nc[i][j]);
				}
				cout << endl;
			}
		}

		for (int i = 0; i < numJointStates; i++) {
			normalCondition[i] = nc[i][i];
		}
	}
	else {
		printf("There is correlation device option in input.\n");
		normalCondition = environment.getCorrelationDevice();
	}

	if (modeDebug) {
		for (int i = 0; i < numJointStates; i++) {
			cout << rationalToString(normalCondition[i]) << " ";
		}
		cout << endl;
	}

	vector<Belief> res(playerStates);
	for (int i = 0; i < playerStates; i++) {
		Belief seed(opponentStates);
		PreciseNumber denom = ZERO;
		for (int j = 0; j < opponentStates; j++) {
			denom += normalCondition[StateProfile(i, j).getIndex(numStatesOfPlayer)];
		}
		for (int j = 0; j < opponentStates; j++) {
			if (equal(denom, ZERO)) {
				seed.set(j, ZERO);
			}
			else {
				seed.set(j, normalCondition[StateProfile(i, j).getIndex(numStatesOfPlayer)] / denom);
			}
		}
		seed.setReward(getRewardFromAlphaVectorAndBelief(alphaVectors[i], seed));
		res[i] = seed;
		debug(seed);
	}

	invariantDistribution = res;
}

vector<Belief> RepeatedGame::getInvariantDistribution() {
	return invariantDistribution;
}

PreciseNumber RepeatedGame::getReward(int player, StateProfile sp) {
	return equations[player].getResult(sp.getIndex(numStatesOfPlayer));
}

void RepeatedGame::makeAlphaVectors() {
	for (int i = 0; i < playerStates; i++) {
		vector<PreciseNumber> aV(opponentStates);
		for (int j = 0; j < opponentStates; j++) {
			aV[j] = getReward(PLAYER, StateProfile(i, j));
		}
		alphaVectors.push_back(aV);
	}
}

vector< vector<PreciseNumber> > RepeatedGame::getAlphaVectors() {
	return alphaVectors;
}

PreciseNumber RepeatedGame::getRewardFromAlphaVectorAndBelief(vector<PreciseNumber> &alphaVector, Belief &b) {
	PreciseNumber res = 0;
	for (int i = 0; i < alphaVector.size(); i++) {
		res += b.get(i) * alphaVector[i];
	}
	return res;
}

void RepeatedGame::solve() {
	solveJointFSA();
	makeAlphaVectors();
	makeInvariantDistribution();
	makeBeliefDivision();
	makeTargetBeliefDivision();
}

void RepeatedGame::makeBeliefDivision() {
	initialBeliefDivision.configure(playerStates);

	vector< vector<PreciseNumber> > input(playerStates + opponentStates, vector<PreciseNumber>(opponentStates + 1, 0));
	vector<bool> il(playerStates + opponentStates, false);

	int row = 0;
	// alpha vector
	for (int i = 0; i < playerStates; i++) {
		int pivot = opponentStates - 1;
		int col = 0;
		input[row][col++] = -1 * alphaVectors[i][pivot];
		for (int j = 0; j < opponentStates - 1; j++) {
			input[row][col++] = -(alphaVectors[i][j] - alphaVectors[i][pivot]);
		}
		input[row][col++] = 1;
		row++;
	}

	// belief range
	for (int i = 0; i < opponentStates; i++) {
		if (i == opponentStates - 1) {
			input[row][0] = 1;
			for (int j = 0; j < opponentStates - 1; j++) input[row][j + 1] = -1;
		}
		else {
			input[row][i + 1] = 1;
		}
	    row++;
	}

	Inequality inequalities = Inequality(input, il);
	pair< vector< vector<PreciseNumber> >, vector< vector<int> > > res = convertInequalityToPointWithIncidence(inequalities);
	vector< vector<PreciseNumber> > beliefPoints = res.first;
	vector< vector<int> > belongDivisions = res.second;

	vector<bool> isPoint(beliefPoints.size(), false);
	vector<Belief> rawExtremePoints = convertPointToBeliefWithReward(beliefPoints, isPoint);

	vector< vector<Belief> > polytope(playerStates);
	for (int i = 0; i < belongDivisions.size(); i++) {
		if (!isPoint[i]) continue;
		for (int j = 0; j < belongDivisions[i].size(); j++) {
			if (belongDivisions[i][j] < playerStates) {
				polytope[belongDivisions[i][j]].push_back(rawExtremePoints[i]);
			}
		}
	}

	for (int i = 0; i < playerStates; i++) {
		if (polytope[i].size() > 0) {
			sort(polytope[i].begin(), polytope[i].end());
			initialBeliefDivision.addPolytopeToDivision(i, polytope[i]);
			initialBeliefDivision.makeInequalities(i);
		}
	}
}

void RepeatedGame::plotAlphaVectors(string dir, vector< vector<PreciseNumber> > aVs) {
	string cmdGNUPLOT = "";

	cmdGNUPLOT += string("/usr/bin/gnuplot << EOF\n");
	cmdGNUPLOT += string("set grid\n");
	cmdGNUPLOT += string("set dgrid3d\n");
	cmdGNUPLOT += string("set surface\n");
	cmdGNUPLOT += string("set hidden3d\n");
	cmdGNUPLOT += string("set parametric\n");
	cmdGNUPLOT += string("set isosample 50\n");
	cmdGNUPLOT += string("set xlabel 'Prob(OppState=G)'\n");
	cmdGNUPLOT += string("set ylabel 'Prob(OppState=S)'\n");
	cmdGNUPLOT += string("set zlabel 'Expected Payoff' rotate left\n");
	cmdGNUPLOT += string("set term postscript color\n");
	cmdGNUPLOT += string("set output '" + dir + "/alpha-" + players[PLAYER].getNameOfPlayer() + "-p-" + toString(globalVar["p"].get_d()) + "-q-" + toString(globalVar["q"].get_d()) + "-epsilon-" + toString(globalVar["E"].get_d()) + ".eps'\n");
	cmdGNUPLOT += string("splot [0:1][0:1] \\\n");
	for (int i = 0; i < 	(signed)aVs.size(); i++) {
		string color;
		if (i == 0) color = "red";
		else if (i == 1) color = "green";
		else if (i == 2) color = "blue";
		else if (i == 3) color = "cyan";
		else if (i == 4) color = "#003349";
		else if (i == 5) color = "#328594";
		else color = "#482211";
		PreciseNumber a = aVs[i][0] - aVs[i][2], b = aVs[i][1] - aVs[i][2], c =  aVs[i][2];
		cmdGNUPLOT += string(" u,v,(u+v>1 ? 1/0 : " + toString(a.get_d()) + "*u+" + toString(b.get_d()) + "*v+" + toString(c.get_d()) + ") title 'Start from ' lt 1 lw 4 lc rgb '" + color + "'");
		if (i < (signed)aVs.size() - 1) {
			cmdGNUPLOT += ",\\\n";
		}
		else {
			cmdGNUPLOT += "\n";
		}
	}
	cmdGNUPLOT += string("EOF\n");
	cout << cmdGNUPLOT << endl;

	int ret;
	ret = system(cmdGNUPLOT.c_str());
	if (ret != 0) {
		display("Making EPS error");
	}
}

void RepeatedGame::makeTargetBeliefDivision() {
	targetBeliefDivision.configure(playerStates);
	vector< vector<PreciseNumber> > extendedAlphaVectors = alphaVectors;
	vector< vector<PreciseNumber> > eAVs = alphaVectors;

	Automaton automaton = players[PLAYER].getAutomaton();
	vector<int> nextTransition(playerSignals);

	for (int next = 0; next < pow(playerStates, playerSignals); next++) {
		for (int i = 0, t = next; i < playerSignals; i++) {
			nextTransition[i] = t % playerStates;
			t /= playerStates;
		}

		for (int playerAction = 0; playerAction < playerActions; playerAction++) {
			bool exist = false;
			for (int playerState = 0; playerState < playerStates; playerState++) {
				if (automaton.getActionChoice(playerState) == playerAction) {
					if (automaton.getStateTransition(playerState) == nextTransition) {
						exist = true;
						break;
					}
				}
			}

			if (!exist) {
				vector<PreciseNumber> aV(opponentStates);
				for (int opponentState = 0; opponentState < opponentStates; opponentState++) {
					PreciseNumber stateReward = 0;
					for (int playerSignal = 0; playerSignal < playerSignals; playerSignal++) {
						for (int opponentSignal = 0; opponentSignal < opponentSignals; opponentSignal++) {
							stateReward += environment.getSignalDistribution(ActionProfile(playerAction, players[OPPONENT].getActionChoice(opponentState)), SignalProfile(playerSignal, opponentSignal))
											* getReward(PLAYER, StateProfile(nextTransition[playerSignal], players[OPPONENT].getStateTransition(opponentState, opponentSignal)));
						}

					}
					stateReward *= environment.getDiscountRate();
					stateReward += payoff.getPayoff(PLAYER, ActionProfile(playerAction, players[OPPONENT].getActionChoice(opponentState)));
					aV[opponentState] = stateReward;
				}

//					debug("False on " + toString(now) + " " + rationalToString(nextReward));

				if (modeDebug) {
					if (!checkSeminalWithExtension(initialBeliefDivision, playerAction, nextTransition)) {
						cout << getNameOfActions(PLAYER, playerAction);
						for (int o = 0; o < playerSignals; o++) {
							cout << ", " << getNameOfSignals(PLAYER, o) << ":" << getNameOfStates(PLAYER, nextTransition[o]);
						}
						cout << " " << aV << endl;
						eAVs.push_back(aV);
					}
				}
				extendedAlphaVectors.push_back(aV);
			}
		}
	}

//	plotAlphaVectors("./image", eAVs);
	int n = extendedAlphaVectors.size();

	vector< vector<PreciseNumber> > input(n + opponentStates, vector<PreciseNumber>(opponentStates + 1, 0));
	vector<bool> il(n + opponentStates, false);

	int row = 0;
	// alpha vector
	for (int i = 0; i < n; i++) {
		int pivot = opponentStates - 1;
		int col = 0;
		input[row][col++] = -1 * extendedAlphaVectors[i][pivot];
		for (int j = 0; j < opponentStates - 1; j++) {
			input[row][col++] = -(extendedAlphaVectors[i][j] - extendedAlphaVectors[i][pivot]);
		}
		input[row][col++] = 1;
		row++;
	}

	// belief range
	for (int i = 0; i < opponentStates; i++) {
		if (i == opponentStates - 1) {
			input[row][0] = 1;
			for (int j = 0; j < opponentStates - 1; j++) input[row][j + 1] = -1;
		}
		else {
			input[row][i + 1] = 1;
		}
	    row++;
	}

	Inequality inequalities = Inequality(input, il);
	pair< vector< vector<PreciseNumber> >, vector< vector<int> > > res = convertInequalityToPointWithIncidence(inequalities);
	vector< vector<PreciseNumber> > beliefPoints = res.first;
	vector< vector<int> > belongDivisions = res.second;

	vector<bool> isPoint(beliefPoints.size(), false);
	vector<Belief> rawExtremePoints = convertPointToBeliefWithReward(beliefPoints, isPoint);

	vector< vector<Belief> > polytope(playerStates);

	for (int i = 0; i < belongDivisions.size(); i++) {
		if (!isPoint[i]) continue;
		for (int j = 0; j < belongDivisions[i].size(); j++) {
			if (belongDivisions[i][j] < playerStates) {
				polytope[belongDivisions[i][j]].push_back(rawExtremePoints[i]);
			}
		}
	}

	for (int i = 0; i < playerStates; i++) {
		sort(polytope[i].begin(), polytope[i].end());
		if (polytope[i].size() > 0) {
			targetBeliefDivision.addPolytopeToDivision(i, polytope[i]);
			targetBeliefDivision.makeInequalities(i);
		}
	}
}

Belief RepeatedGame::nextBelief(Belief bp, int action, int signal) {
	Belief res(bp.getDimension());

	for (int nextState = 0; nextState < opponentStates; nextState++) {
		PreciseNumber num = 0, denom = 0;
		for (int nowState = 0; nowState < opponentStates; nowState++) {
			PreciseNumber num2 = 0, denom2 = 0;
			for (int nowSignal = 0; nowSignal < opponentSignals; nowSignal++) {
				if (nextState == players[OPPONENT].getStateTransition(nowState, nowSignal)) {
					num2 += environment.getSignalDistribution(ActionProfile(action, players[OPPONENT].getActionChoice(nowState)), SignalProfile(signal, nowSignal));
				}
			}
			num += bp.get(nowState) * num2;

			for (int nowSignal = 0; nowSignal < opponentSignals; nowSignal++) {
				denom2 += environment.getSignalDistribution(ActionProfile(action, players[OPPONENT].getActionChoice(nowState)), SignalProfile(signal, nowSignal));
			}
			denom += bp.get(nowState) * denom2;
		}

		if (denom == 0) {
			denom = 1;
		}

		num /= denom;

		if (num < 0) {
			num = 0;
		}
		if (num > 1) {
			num = 1;
		}

		res.set(nextState, num);
	}

	return res;
}

vector<Belief> RepeatedGame::nextBeliefSet(vector<Belief> &bd, int action, int signal) {
	vector<Belief> res;

	for (int i = 0; i < bd.size(); i++) {
		res.push_back(nextBelief(bd[i], action, signal));
	}

	sort(res.begin(), res.end());

	return res;
}

vector<Belief> RepeatedGame::nextBeliefDivision(vector<Belief> &bd, int action, int signal) {
	vector<Belief> res;

	for (int i = 0; i < bd.size(); i++) {
		res.push_back(nextBelief(bd[i], action, signal));
	}

	vector< vector<PreciseNumber> > b2p = convertBeliefToPoint(res);
	vector< vector<PreciseNumber> > p2p = convertPointToPoint(b2p);

	res = convertPointToBelief(b2p);
	sort(res.begin(), res.end());

	return res;
}

bool RepeatedGame::checkSeminalWithExtension(BeliefDivision &closed, int playerAction, vector<int> nextTransition) {
	for (int i = 0; i < closed.getNumberOfDivisions(); i++) {
		for (int j = 0; j < closed.getSizeOfDivisions(i); j++) {
	//		debug("Belief division " + toString(i) + "(" + getNameOfStates(PLAYER, i) + ")");
			vector<Belief> division = closed.getExtremePointsOfPolytope(i, j);
			for (int k = 0; k < division.size(); k++) {
				Belief now = division[k];
				PreciseNumber nextReward = 0;

				for (int opponentState = 0; opponentState < opponentStates; opponentState++) {
					PreciseNumber stateReward = 0;
					for (int playerSignal = 0; playerSignal < playerSignals; playerSignal++) {
						for (int opponentSignal = 0; opponentSignal < opponentSignals; opponentSignal++) {
							stateReward += environment.getSignalDistribution(ActionProfile(playerAction, players[OPPONENT].getActionChoice(opponentState)), SignalProfile(playerSignal, opponentSignal))
											* getReward(PLAYER, StateProfile(nextTransition[playerSignal], players[OPPONENT].getStateTransition(opponentState, opponentSignal)));
						}

					}
					stateReward *= environment.getDiscountRate();
					stateReward += payoff.getPayoff(PLAYER, ActionProfile(playerAction, players[OPPONENT].getActionChoice(opponentState)));
					nextReward += stateReward * now.get(opponentState);
				}

				debug("Belief Point : " + toString(now) + " " + rationalToString(now.getReward()) + " " + rationalToString(nextReward));
				if (now.getReward() < nextReward) {
					return false;
				}
			}
		}
	}
	return true;
}

bool RepeatedGame::checkSeminalBeliefDivision(BeliefDivision &closed) {
	Automaton automaton = players[PLAYER].getAutomaton();

	vector<int> nextTransition(playerSignals);

	for (int next = 0; next < pow(playerStates, playerSignals); next++) {
		for (int i = 0, t = next; i < playerSignals; i++) {
			nextTransition[i] = t % playerStates;
			t /= playerStates;
		}

		for (int playerAction = 0; playerAction < playerActions; playerAction++) {
			bool exist = false;
			for (int s = 0; s < playerStates; s++) {
				if (automaton.getActionChoice(s) == playerAction) {
					if (automaton.getStateTransition(s) == nextTransition) {
						exist = true;
						break;
					}
				}
			}

			if (!exist) {
				debugTitle("One-shot extension");
				debug("Action : " + players[PLAYER].getNameOfActions(playerAction));
				for (int i = 0; i < playerSignals; i++) {
					debug(players[PLAYER].getNameOfSignals(i) + " -> " + players[PLAYER].getNameOfStates(nextTransition[i]));
				}

				if (!checkSeminalWithExtension(closed, playerAction, nextTransition)) {
					return false;
				}
			}
		}
	}

	return true;
}

bool RepeatedGame::checkSeminalWithExtension(Belief &b, int playerAction, vector<int> nextTransition) {
	bool res = true;
	Belief now = b;
	PreciseNumber nextReward = 0;

	for (int opponentState = 0; opponentState < opponentStates; opponentState++) {
		PreciseNumber stateReward = 0;
		for (int playerSignal = 0; playerSignal < playerSignals; playerSignal++) {
			for (int opponentSignal = 0; opponentSignal < opponentSignals; opponentSignal++) {
				stateReward += environment.getSignalDistribution(ActionProfile(playerAction, players[OPPONENT].getActionChoice(opponentState)), SignalProfile(playerSignal, opponentSignal))
								* getReward(PLAYER, StateProfile(nextTransition[playerSignal], players[OPPONENT].getStateTransition(opponentState, opponentSignal)));
			}

		}
		stateReward *= environment.getDiscountRate();
		stateReward += payoff.getPayoff(PLAYER, ActionProfile(playerAction, players[OPPONENT].getActionChoice(opponentState)));
		nextReward += stateReward * now.get(opponentState);
	}

	debug("Belief Point : " + toString(now) + " " + rationalToString(now.getReward()) + " " + rationalToString(nextReward));
	if (now.getReward() < nextReward) {
		return false;
	}

	return true;
}

bool RepeatedGame::checkSeminalBelief(Belief &b) {
	Automaton automaton = players[PLAYER].getAutomaton();

	vector<int> nextTransition(playerSignals);

	for (int next = 0; next < pow(playerStates, playerSignals); next++) {
		for (int i = 0, t = next; i < playerSignals; i++) {
			nextTransition[i] = t % playerStates;
			t /= playerStates;
		}

		for (int playerAction = 0; playerAction < playerActions; playerAction++) {
			bool exist = false;
			for (int s = 0; s < playerStates; s++) {
				if (automaton.getActionChoice(s) == playerAction) {
					if (automaton.getStateTransition(s) == nextTransition) {
						exist = true;
						break;
					}
				}
			}

			if (!exist) {
				debugTitle("One-shot extension");
				debug("Action : " + players[PLAYER].getNameOfActions(playerAction));
				for (int i = 0; i < playerSignals; i++) {
					debug(players[PLAYER].getNameOfSignals(i) + " -> " + players[PLAYER].getNameOfStates(nextTransition[i]));
				}

				if (!checkSeminalWithExtension(b, playerAction, nextTransition)) {
					return false;
				}
			}
		}
	}

	return true;
}


bool RepeatedGame::checkSeminalWithExtension(vector<Belief> &b, int playerAction, vector<int> nextTransition) {
	for (int i = 0; i < b.size(); i++) {
		Belief now = b[i];
		PreciseNumber nextReward = 0;
		for (int opponentState = 0; opponentState < opponentStates; opponentState++) {
			PreciseNumber stateReward = 0;
			for (int playerSignal = 0; playerSignal < playerSignals; playerSignal++) {
				for (int opponentSignal = 0; opponentSignal < opponentSignals; opponentSignal++) {
					stateReward += environment.getSignalDistribution(ActionProfile(playerAction, players[OPPONENT].getActionChoice(opponentState)), SignalProfile(playerSignal, opponentSignal))
									* getReward(PLAYER, StateProfile(nextTransition[playerSignal], players[OPPONENT].getStateTransition(opponentState, opponentSignal)));
				}

			}
			stateReward *= environment.getDiscountRate();
			stateReward += payoff.getPayoff(PLAYER, ActionProfile(playerAction, players[OPPONENT].getActionChoice(opponentState)));
			nextReward += stateReward * now.get(opponentState);
		}

		debug("Belief Point : " + toString(now) + " " + rationalToString(now.getReward()) + " " + rationalToString(nextReward));
		if (now.getReward() < nextReward) {
			return false;
		}
	}

	return true;
}

bool RepeatedGame::checkSeminalBeliefSet(vector<Belief> &b) {
	Automaton automaton = players[PLAYER].getAutomaton();

	vector<int> nextTransition(playerSignals);

	for (int next = 0; next < pow(playerStates, playerSignals); next++) {
		for (int i = 0, t = next; i < playerSignals; i++) {
			nextTransition[i] = t % playerStates;
			t /= playerStates;
		}

		for (int playerAction = 0; playerAction < playerActions; playerAction++) {
			bool exist = false;
			for (int s = 0; s < playerStates; s++) {
				if (automaton.getActionChoice(s) == playerAction) {
					if (automaton.getStateTransition(s) == nextTransition) {
						exist = true;
						break;
					}
				}
			}

			if (!exist) {
				debugTitle("One-shot extension");
				debug("Action : " + players[PLAYER].getNameOfActions(playerAction));
				for (int i = 0; i < playerSignals; i++) {
					debug(players[PLAYER].getNameOfSignals(i) + " -> " + players[PLAYER].getNameOfStates(nextTransition[i]));
				}

				if (!checkSeminalWithExtension(b, playerAction, nextTransition)) {
					return false;
				}
			}
		}
	}

	return true;
}

BeliefDivision RepeatedGame::getTargetBeliefDivision() {
	return targetBeliefDivision;
}

bool RepeatedGame::checkGlobalResilent() {
	vector<Belief> normalCondition = getInvariantDistribution();

	for (int i = 0; i < playerStates; i++) {
		if (targetBeliefDivision.getSizeOfDivisions(i) == 0) {
			return false;
		}

		if (!isInnerPointOfPolytope(normalCondition[i], initialBeliefDivision.getExtremePointsOfPolytope(i, 0))) {
			return false;
		}
	}

	return checkSeminalBeliefDivision(initialBeliefDivision);
}

vector< vector<Belief> > RepeatedGame::Co(BeliefDivision &now) {
	vector< vector<Belief> > res(playerStates);

	for (int i = 0; i < playerStates; i++) {
		if (now.getSizeOfDivisions(i) == 0) continue;
		for (int j = 0; j < now.getSizeOfDivisions(i); j++) {
			vector<Belief> dv = now.getExtremePointsOfPolytope(i, j);
			for (int k = 0; k < dv.size(); k++) {
				bool check = true;
				for (int l = 0; l < res[i].size(); l++) {
					if (res[i][l] == dv[k]) {
						check = false;
						break;
					}
				}
				if (check) {
					res[i].push_back(dv[k]);
				}
			}
		}
		res[i] = getConvex(res[i]);
		sort(res[i].begin(), res[i].end());
	}

	return res;
}

int RepeatedGame::checkResilent() {
	vector<Belief> normalCondition = getInvariantDistribution();

	for (int i = 0; i < playerStates; i++) {
		if (targetBeliefDivision.getSizeOfDivisions(i) == 0) {
			display("Not exists target belief division");
			return 1;
		}
		if (normalCondition[i].checkValidity()) {
			if (!isInnerPointOfPolytope(normalCondition[i], targetBeliefDivision.getExtremePointsOfPolytope(i, 0))) {
				display("Not consistent");
				return 1;
			}
		}
		else {
			display("Invariant Distribution Error");
			return 1;
		}
	}

	// pre process
	for (int a = 0; a < playerActions; a++) {
		for (int o = 0; o < playerSignals; o++) {
			for (int i = 0; i < playerStates; i++) {
				Belief b = normalCondition[i];
				while (true) {
					Belief next = nextBelief(b, a, o);
					if (b == next) {
						break;
					}
					b = next;
				}
				bool check = false;
				for (int j = 0; j < opponentStates; j++) {
					if (isInnerPointOfPolytope(b, targetBeliefDivision.getExtremePointsOfPolytope(j, 0))) {
						check = true;
						break;
					}
				}
				if (!check) {
					return 1;
				}
			}
		}
	}

	stack< vector< pair<int, int> > > stackHistory;

	vector<Belief> beliefSpace;
	for (int i = 0; i < opponentStates; i++) {
		Belief e(opponentStates);
		e.set(i, ONE);
		beliefSpace.push_back(e);
	}
	sort(beliefSpace.begin(), beliefSpace.end());

	vector< pair<int, int> > dummy;
	stackHistory.push(dummy);

	int precision = 3;
	int approx = 6;
	// step 1
	int t = 0;
	map<string, bool> visited;
	while (!stackHistory.empty()) {
		vector< pair<int, int> > nowHistory = stackHistory.top();
//		cout << "t=" << t++ << " " <<  stackHistory.size() << endl;
		stackHistory.pop();
		// step 2
		vector<Belief> nextPolytope = beliefSpace;
		vector<Belief> nextNormalCondition = normalCondition;
		for (int i = 0; i < nowHistory.size(); i++) {
			nextPolytope = nextBeliefSet(nextPolytope, nowHistory[i].first, nowHistory[i].second);
			for (int j = 0; j < playerStates; j++) {
				nextNormalCondition[j] = nextBelief(nextNormalCondition[j], nowHistory[i].first, nowHistory[i].second);
			}
		}
		if (visited.find(beliefToString(nextPolytope, approx)) != visited.end()) continue;
		visited.insert(make_pair(beliefToString(nextPolytope, approx), true));

		// check r^\infty
		for (int i = 0; i < playerStates; i++) {
			PreciseNumber maxr = ZERO;
			for (int k = 0; k < playerStates; k++) {
				PreciseNumber t = getRewardFromAlphaVectorAndBelief(alphaVectors[k], nextNormalCondition[i]);
				if (maxr < t) {
					maxr = t;
				}
			}
			nextNormalCondition[i].setReward(maxr);
		}

		if (!checkSeminalBeliefSet(nextNormalCondition)) {
			return 3;
		}

		int checkWin = true;
		Inequality IneNextPolytope = convertBeliefToInequality(nextPolytope);
		for (int i = 0; i < playerStates; i++) {
			Inequality cover = initialBeliefDivision.getInequalitiesOfPolytope(i, 0);
			vector<Belief> points = convertPointToBelief(intersection(IneNextPolytope, cover));
			if (points.size() > 0) {
				for (int j = 0; j < points.size(); j++) {
					points[j].setReward(getRewardFromAlphaVectorAndBelief(alphaVectors[i], points[j]));
				}
				sort(points.begin(), points.end());
				if (!checkSeminalBeliefSet(points)) {
					checkWin = false;
					break;
				}
			}
		}
		/*
		// step 3
		vector<int> cntAlpha(playerStates, 0);
		for (int i = 0; i < opponentStates; i++) {
			PreciseNumber maxr = ZERO;
			for (int k = 0; k < playerStates; k++) {
				PreciseNumber t = getRewardFromAlphaVectorAndBelief(alphaVectors[k], nextPolytope[i]);
				if (maxr < t) {
					maxr = t;
				}
			}
			for (int k = 0; k < playerStates; k++) {
				if (equal(maxr, getRewardFromAlphaVectorAndBelief(alphaVectors[k], nextPolytope[i]))) cntAlpha[k]++;
			}
			nextPolytope[i].setReward(maxr);
		}

		bool sameHyperplane = false;
		for (int k = 0; k < playerStates; k++) {
			if (cntAlpha[k] == playerStates) {
				sameHyperplane = true;
				break;
			}
		}
		 */

		if (!checkWin) {
			// step 4
			bool checkConverged = true;
			for (int i = 0; i < opponentStates - 1; i++) {
				for (int j = i + 1; j < opponentStates; j++) {
					if (!isSameWithPrecision(nextPolytope[i], nextPolytope[j], precision)) {
						checkConverged = false;
						break;
					}
				}
				if (!checkConverged) break;
			}

			if (checkConverged) {
				return 3;
			}
			else {
				for (int a = 0; a < playerActions; a++) {
					for (int o = 0; o < playerSignals; o++) {
//						stackPolytope.push(nextPolytope);
						vector< pair<int, int> > newHistory = nowHistory;
						newHistory.insert(newHistory.begin(), make_pair(a, o));
						stackHistory.push(newHistory);
//						stackNormalCondition.push(nextNormalCondition);
					}
				}
			}
		}
	}

	for (int i = 0; i < playerStates; i++) {
		candidates.push_back(normalCondition[i]);
	}
	return 2;
}

bool RepeatedGame::checkResilentFromBelief(Belief condition) {
	for (int i = 0; i < playerStates; i++) {
		if (targetBeliefDivision.getSizeOfDivisions(i) == 0) {
			display("Not exists target belief division");
			return 0;
		}
	}

	stack< vector< pair<int, int> > > stackHistory;

	vector<Belief> beliefSpace;
	for (int i = 0; i < opponentStates; i++) {
		Belief e(opponentStates);
		e.set(i, ONE);
		beliefSpace.push_back(e);
	}
	sort(beliefSpace.begin(), beliefSpace.end());

	vector< pair<int, int> > dummy;
	stackHistory.push(dummy);

	int precision = 3;
	int approx = 6;
	// step 1
	int t = 0;
	map<string, bool> visited;
	while (!stackHistory.empty()) {
		vector< pair<int, int> > nowHistory = stackHistory.top();
		stackHistory.pop();
		// step 2
		vector<Belief> nextPolytope = beliefSpace;
		Belief nextCondition = condition;
		for (int i = 0; i < nowHistory.size(); i++) {
			nextPolytope = nextBeliefSet(nextPolytope, nowHistory[i].first, nowHistory[i].second);
			nextCondition = nextBelief(nextCondition, nowHistory[i].first, nowHistory[i].second);
		}

		if (visited.find(beliefToString(nextPolytope, approx)) != visited.end()) continue;
		visited.insert(make_pair(beliefToString(nextPolytope, approx), true));

		// check r^\infty
		PreciseNumber maxr = ZERO;
		for (int k = 0; k < playerStates; k++) {
			PreciseNumber t = getRewardFromAlphaVectorAndBelief(alphaVectors[k], nextCondition);
			if (maxr < t) {
				maxr = t;
			}
		}
		nextCondition.setReward(maxr);

		if (!checkSeminalBelief(nextCondition)) {
			return false;
		}

		int checkWin = true;
		Inequality IneNextPolytope = convertBeliefToInequality(nextPolytope);
		for (int i = 0; i < playerStates; i++) {
			Inequality cover = initialBeliefDivision.getInequalitiesOfPolytope(i, 0);
			vector<Belief> points = convertPointToBelief(intersection(IneNextPolytope, cover));
			if (points.size() > 0) {
				for (int j = 0; j < points.size(); j++) {
					points[j].setReward(getRewardFromAlphaVectorAndBelief(alphaVectors[i], points[j]));
				}
				sort(points.begin(), points.end());
				if (!checkSeminalBeliefSet(points)) {
					checkWin = false;
					break;
				}
			}
		}

		if (!checkWin) {
			// step 4
			bool checkConverged = true;
			for (int i = 0; i < opponentStates - 1; i++) {
				for (int j = i + 1; j < opponentStates; j++) {
					if (!isSameWithPrecision(nextPolytope[i], nextPolytope[j], precision)) {
						checkConverged = false;
						break;
					}
				}
				if (!checkConverged) break;
			}

			if (checkConverged) {
				return false;
			}
			else {
				for (int a = 0; a < playerActions; a++) {
					for (int o = 0; o < playerSignals; o++) {
						vector< pair<int, int> > newHistory = nowHistory;
						newHistory.insert(newHistory.begin(), make_pair(a, o));
						stackHistory.push(newHistory);
					}
				}
			}
		}
	}

	return true;
}

int RepeatedGame::checkNoEquilibrium() {
	int result = this->checkResilent();

	if (result == 1) {
		return 1;
	}

	for (int i = 0; i < playerStates; i++) {
		if (targetBeliefDivision.getSizeOfDivisions(i) == 0) {
			display("Not exists target belief division");
			return 1;
		}
	}

	PreciseNumber start;
	PreciseNumber interval;

	if (globalVar.count("start") == 1)
		start = globalVar["start"];
	else
		start = PreciseNumber("1/100");

	if (globalVar.count("interval") == 1)
		interval = globalVar["interval"];
	else
		interval = PreciseNumber("1/100");


	for (PreciseNumber epsilon = start; ONE - (opponentStates - 1) * epsilon > ZERO; epsilon += interval) {
		Belief base(opponentStates);
		for (int i = 0; i < opponentStates; i++) {
			base.set(i, epsilon);
		}
		for (int i = 0; i < opponentStates; i++) {
			Belief cond = base;
			cond.set(i, ONE - (opponentStates - 1) * epsilon);
			if (isInnerPointOfPolytope(cond, targetBeliefDivision.getExtremePointsOfPolytope(i, 0))) {
				// preprocess
				for (int a = 0; a < playerActions; a++) {
					for (int o = 0; o < playerSignals; o++) {
						Belief b = cond;
						while (true) {
							Belief next = nextBelief(b, a, o);
							if (b == next) {
								break;
							}
							b = next;
						}
						bool check = false;
						for (int j = 0; j < opponentStates; j++) {
							if (isInnerPointOfPolytope(b, targetBeliefDivision.getExtremePointsOfPolytope(j, 0))) {
								check = true;
								break;
							}
						}
						if (!check) {
							return 0;
						}
					}
				}

				if (checkResilentFromBelief(cond)) {
					candidates.push_back(cond);
				}
			}
		}
	}

	if (candidates.size() > 0) {
		return 2;
	}

	return 3;
}

vector<Belief> RepeatedGame::getCandidates() {
	return candidates;
}

BeliefDivision RepeatedGame::operatorW(BeliefDivision originalDivisions) {
	vector< vector< vector<Belief> > > nextDivisions(playerStates);

	for (int k = 0; k < playerStates; k++) {
		if (originalDivisions.getSizeOfDivisions(k) == 0) continue;
		for (int i = 0; i < originalDivisions.getSizeOfDivisions(k); i++) {
			vector<Belief> now = originalDivisions.getExtremePointsOfPolytope(k, i);
			if (now.size() == 0) continue;
			for (int a = 0; a < playerActions; a++) {
				for (int o = 0; o < playerSignals; o++) {
					Inequality next = convertBeliefToInequality(nextBeliefDivision(now, a, o));
					for (int l = 0; l < playerStates; l++) {
						Inequality cover = initialBeliefDivision.getInequalitiesOfPolytope(l, 0);
						vector<Belief> points = convertPointToBelief(intersection(next, cover));
						if (points.size() > 0) {
							sort(points.begin(), points.end());
							bool check = true;
							for (int m = 0; m < nextDivisions[l].size(); m++) {
								if (nextDivisions[l][m] == points) {
									check = false;
									break;
								}
							}
							if (check) {
								nextDivisions[l].push_back(points);
							}
						}
					}
				}
			}
		}
	}

	BeliefDivision res(playerStates);
	for (int k = 0; k < playerStates; k++) {
		if (nextDivisions[k].size() == 0) continue;
		for (int i = 0; i < nextDivisions[k].size(); i++) {
			for (int j = 0; j < nextDivisions[k][i].size(); j++) {
				nextDivisions[k][i][j].setReward(getRewardFromAlphaVectorAndBelief(alphaVectors[k], nextDivisions[k][i][j]));
			}
			res.addPolytopeToDivision(k, nextDivisions[k][i]);
		}
		res.makeInequalities(k);
	}

	return res;
}

BeliefDivision RepeatedGame::operatorU(BeliefDivision originalDivisions) {
	vector< vector< vector<Belief> > > nextDivisions(playerStates);

	for (int k = 0; k < playerStates; k++) {
		if (originalDivisions.getSizeOfDivisions(k) == 0) continue;
		for (int i = 0; i < originalDivisions.getSizeOfDivisions(k); i++) {
			vector<Belief> current = originalDivisions.getExtremePointsOfPolytope(k, i);
			nextDivisions[k].push_back(current);
		}
	}

	for (int k = 0; k < playerStates; k++) {
		if (originalDivisions.getSizeOfDivisions(k) == 0) continue;
		for (int i = 0; i < originalDivisions.getSizeOfDivisions(k); i++) {
			vector<Belief> now = originalDivisions.getExtremePointsOfPolytope(k, i);
			if (now.size() == 0) continue;
			for (int a = 0; a < playerActions; a++) {
				for (int o = 0; o < playerSignals; o++) {
					Inequality next = convertBeliefToInequality(nextBeliefDivision(now, a, o));
					for (int l = 0; l < playerStates; l++) {
						Inequality cover = initialBeliefDivision.getInequalitiesOfPolytope(l, 0);
						vector<Belief> points = convertPointToBelief(intersection(next, cover));
						if (points.size() > 0) {
							sort(points.begin(), points.end());
							bool check = true;
							for (int m = 0; m < nextDivisions[l].size(); m++) {
								if (nextDivisions[l][m] == points) {
									check = false;
									break;
								}
							}
							if (check) {
								nextDivisions[l].push_back(points);
							}
						}
					}
				}
			}
		}
	}

	BeliefDivision res(playerStates);
	for (int k = 0; k < playerStates; k++) {
		if (nextDivisions[k].size() == 0) continue;
		for (int i = 0; i < nextDivisions[k].size(); i++) {
			for (int j = 0; j < nextDivisions[k][i].size(); j++) {
				nextDivisions[k][i][j].setReward(getRewardFromAlphaVectorAndBelief(alphaVectors[k], nextDivisions[k][i][j]));
			}
			res.addPolytopeToDivision(k, nextDivisions[k][i]);
		}
		res.makeInequalities(k);
	}

	return res;
}
