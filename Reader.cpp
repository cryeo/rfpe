/*
 * Reader.cpp
 *
 *  Created on: 2012. 12. 4.
 *      Author: chaerim
 */

#include "Reader.h"

string Reader::numberToRational(string number) {
	int pos = number.find(".");
	string res = "";

	if (pos == string::npos) {
		res = number;
	}
	else {
		bool negative = false;
		if (number[0] == '-') {
			negative = true;

		}
		string part1 = number.substr((negative ? 1 : 0), pos - (negative ? 1 : 0));
		string part2 = number.substr(pos + 1);
		if (part1.length() == 1 && part1[0] == '0') part1 = "";

		string d = "1";
		for (int i = 0; i < part2.length(); i++) d += "0";
		string n = "", nn = part1 + part2;
		bool ck = false;
		for (int i = 0; i < nn.length(); i++) {
			if (nn[i] != '0') ck = true;
			if (ck) n += nn[i];
		}
		res = (negative ? "-" : "") + n + "/" + d;
	}

	return res;
}

Reader::Reader() {
	numPlayers = 0;
}

Reader::Reader(string nameDataFile) {
	numPlayers = 0;
	fin.open(nameDataFile.c_str(), ifstream::in);
}

Reader::~Reader() {
	if (fin) {
		fin.close();
	}
}

int Reader::getIndex(vector<string> &list, string &value) {
	return int(find(list.begin(), list.end(), value) - list.begin());
}

int Reader::getIndexOfPlayers(string player) {
	return getIndex(players, player);
}

READSTATUS Reader::readTitle() {
	READSTATUS result = NOERROR;
	if (!fin) {
		result = FILENOTFOUND;
	}
	else {
		string buf;
		boost::regex expr("Title *: *(.+)");
		boost::smatch what;

		fin.seekg(0, ios_base::beg);
		result = ITEMNOTFOUND;
		while (!fin.eof()) {
			getline(fin, buf);

			if (boost::regex_match(buf, what, expr)) {
				title = what[1];
				result = NOERROR;
				break;
			}
		}
	}

	return result;
}

READSTATUS Reader::readDiscountRate() {
	READSTATUS result = NOERROR;

	if (!fin) {
		result = FILENOTFOUND;
	}
	else {
		string buf;
		boost::regex expr("Discount Rate *: *(.+)");
		boost::smatch what;

		fin.seekg(0, ios_base::beg);
		result = ITEMNOTFOUND;
		while (!fin.eof()) {
			getline(fin, buf);
			if (boost::regex_match(buf, what, expr)) {
				discountRate = numberToRational(what[1]);
				result = NOERROR;
				break;
			}
		}
	}

	return result;
}

READSTATUS Reader::readItem(string tag, vector<string> &data, bool startFirst) {
	READSTATUS result = NOERROR;

	if (!fin) {
		result = FILENOTFOUND;
	}
	else {
		string buf;
		boost::regex expr(tag + " *: *(.+)");
		boost::smatch what;

		result = ITEMNOTFOUND;
		if (startFirst) {
			fin.seekg(0, ios_base::beg);
			while (!fin.eof()) {
				getline(fin, buf);
				if (boost::regex_match(buf, what, expr)) {
					istringstream iss(what[1]);
					string stbuf;
					data.clear();
					while (!iss.eof()) {
						iss >> stbuf;
						data.push_back(stbuf);
					}
					result = NOERROR;
					break;
				}
			}
		}
		else {
			getline(fin, buf);
			if (boost::regex_match(buf, what, expr)) {
				istringstream iss(what[1]);
				string stbuf;
				data.clear();
				while (!iss.eof()) {
					iss >> stbuf;
					data.push_back(stbuf);
				}
				result = NOERROR;
			}
		}
	}

	return result;
}

READSTATUS Reader::readPlayers() {
	READSTATUS result = readItem("Players", players, true);
	if (result == NOERROR) {
		numPlayers = players.size();
	}
	return result;
}

READSTATUS Reader::readVariables() {
	vector<string> vars;
	READSTATUS result = readItem("Variables", vars, true);

	if (result == NOERROR) {
		boost::regex expr("([a-zA-Z]+)=(.+)");
		boost::smatch what;

		PreciseNumber value;
		for (int i = 0; i < vars.size(); i++) {
			if (boost::regex_match(vars[i], what, expr)) {
				value = numberToRational(what[2]);
				if (variables.find(what[1]) == variables.end()) {
					variables.insert(make_pair(what[1], value));
				}
			}
			else {
				result = SYNTAXERROR;
				break;
			}
		}
	}

	return result;
}

READSTATUS Reader::readAutomaton(string namePlayer, Automaton &am) {
	READSTATUS result = NOERROR;

	if (!fin) {
		result = FILENOTFOUND;
	}
	else {
		string buf;

		fin.seekg(0, ios_base::beg);
		result = ITEMNOTFOUND;
		while (!fin.eof()) {
			getline(fin, buf);
			if (buf == "Automaton " + namePlayer) {
				// States
				vector<string> nameStates;
				result = readItem("States", nameStates);
				if (result != NOERROR) {
					return result;
				}
				am.setNameOfStates(nameStates);

				// Actions
				vector<string> nameActions;
				result = readItem("Actions", nameActions);
				if (result != NOERROR) {
					return result;
				}
				am.setNameOfActions(nameActions);

				// Signals
				vector<string> nameSignals;
				result = readItem("Signals", nameSignals);
				if (result != NOERROR) {
					return result;
				}
				am.setNameOfSignals(nameSignals);

				// Action choice
				string buf1, buf2, buf3;
				for (int i = 0; i < am.getNumberOfStates(); i++) {
					fin >> buf1 >> buf2;
					am.setActionChoice(am.getIndexOfStates(buf1), am.getIndexOfActions(buf2));
				}

				// Transition
				for (int i = 0; i < am.getNumberOfStates() * am.getNumberOfSignals(); i++) {
					fin >> buf1 >> buf2 >> buf3;
					am.setStateTransition(am.getIndexOfStates(buf1), am.getIndexOfSignals(buf2), am.getIndexOfStates(buf3));
				}

				result = NOERROR;
				break;
			}
		}
	}

	return result;
}

READSTATUS Reader::readSignalDistribution(Environment &env, vector<Player> &pls) {
	READSTATUS result = NOERROR;

	if (!fin) {
		result = FILENOTFOUND;
	}
	else {
		string buf;
		fin.seekg(0, ios_base::beg);
		result = ITEMNOTFOUND;
		while (!fin.eof()) {
			getline(fin, buf);
			if (buf == "Signal Distribution") {
				for (int i = 0; i < env.getNumberOfActionProfiles(); i++) {
					getline(fin, buf);
					boost::regex expr("([a-zA-Z0-9]+),([a-zA-Z0-9]+) *: *(.+)");
					boost::smatch what;

					if (boost::regex_match(buf, what, expr)) {
						istringstream iss(what[3]);
						string stbuf;
						for (int j = 0; j < env.getNumberOfSignalProfiles(); j++) {
							iss >> stbuf;
							env.setRawSignalDistribution(ActionProfile(pls[0].getIndexOfActions(what[1]), pls[1].getIndexOfActions(what[2])), SignalProfile(j, numSignalsOfPlayer), numberToRational(stbuf));
						}
					}
				}
				result = NOERROR;
				break;
			}
		}
	}

	return result;
}

READSTATUS Reader::readPayoffMatrix(Payoff &po, vector<Player> &pls) {
	READSTATUS result = NOERROR;

	if (!fin) {
		result = FILENOTFOUND;
	}
	else {
		string buf;
		fin.seekg(0, ios_base::beg);
		result = ITEMNOTFOUND;
		while (!fin.eof()) {
			getline(fin, buf);
			if (buf == "Payoff Matrix") {
				for (int i = 0; i < numPlayers; i++) {
					getline(fin, buf);
					boost::regex expr("([a-zA-Z0-9]+) *: *(.+)");
					boost::smatch what;
					if (boost::regex_match(buf, what, expr)) {
						istringstream iss(what[2]);
						string stbuf;
						for (int j = 0; j < po.getNumberOfActionProfiles(); j++) {
							iss >> stbuf;
							po.setRawPayoff(getIndexOfPlayers(what[1]), ActionProfile(j, numActionsOfPlayer), numberToRational(stbuf));
						}
					}
				}
				result = NOERROR;
				break;
			}
		}
	}

	return result;
}

READSTATUS Reader::readCorrelationDevice(Environment &env) {
	READSTATUS result = NOERROR;
	env.setOptionOfCorrelationDevice(false);

	if (!fin) {
		result = FILENOTFOUND;
	}
	else {
		string buf;
		fin.seekg(0, ios_base::beg);
		while (!fin.eof()) {
			getline(fin, buf);
			if (buf == "Correlation Device") {
				env.setOptionOfCorrelationDevice(true);
				getline(fin, buf);
				istringstream iss(buf);
				string stbuf;
				for (int i = 0; i < env.getNumberOfStateProfiles(); i++) {
					iss >> stbuf;
					env.setRawCorrelationDevice(i, numberToRational(stbuf));
				}
				break;
			}
		}
	}

	return result;
}

READSTATUS Reader::read(string &tit, Environment &env, Payoff &po, vector<Player> &pls) {
	READSTATUS result;

	result = readTitle();
	if (result != NOERROR) return result;
	tit = title;

	result = readDiscountRate();
	if (result != NOERROR) return result;
	env.setDiscountRate(discountRate);

	result = readVariables();
	if (result != NOERROR) return result;

	result = readPlayers();
	if (result != NOERROR) return result;

	pls.resize(numPlayers);
	numStatesOfPlayer.resize(numPlayers);
	numActionsOfPlayer.resize(numPlayers);
	numSignalsOfPlayer.resize(numPlayers);

	for (int i = 0; i < numPlayers; i++) {
		Automaton am;
		result = readAutomaton(players[i], am);
		if (result != NOERROR) return result;
		pls[i].configure(players[i], am);
		numStatesOfPlayer[i] = am.getNumberOfStates();
		numActionsOfPlayer[i] = am.getNumberOfActions();
		numSignalsOfPlayer[i] = am.getNumberOfSignals();
	}

	env.configure(discountRate, numStatesOfPlayer, numActionsOfPlayer, numSignalsOfPlayer);
	result = readSignalDistribution(env, pls);
	if (result != NOERROR) return result;

	po.configure(numPlayers, numActionsOfPlayer);
	result = readPayoffMatrix(po, pls);
	if (result != NOERROR) return result;

	result = readCorrelationDevice(env);
	if (result != NOERROR) return result;

	return result;
}

void Reader::setVariable(string name, PreciseNumber value) {
	variables[name] = value;
}

map<string, PreciseNumber> Reader::getVariables() {
	return variables;
}

