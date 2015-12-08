#include "rfpe.h"
#include <boost/regex.hpp>

namespace Message {
	static void display(string msg) {
		cout << msg << endl;
	}
	static void alert(string msg) {
		cout << "* Error : " << msg << endl;
	}
}


void checkRFPE(string datafile, string prefix, int argc, char **argv) {
	boost::timer timer;

	Reader reader(datafile);

	Environment env;
	Payoff pd;
	vector<Player> players;
	RepeatedGame rg;
	string title;

	READSTATUS result = reader.read(title, env, pd, players);
	if (result != NOERROR) {
		Message::alert("Fail to read datafile");
		Message::display("Abort");
		return;
	}
	else {
		map<string, PreciseNumber> variables = reader.getVariables();

		boost::regex expr("([a-zA-Z]+)=(.+)");
		boost::smatch what;

		PreciseNumber value;
		for (int i = 3; i < argc; i++) {
			string exp = argv[i];
			if (boost::regex_match(exp, what, expr)) {
				value = reader.numberToRational(what[2]);
				variables[what[1]] = value;
			}
			else {
				Message::alert("Fail to parse variables in arguments");
				Message::display("Abort");
				return;
			}
		}

		if (players[0].getAutomaton() == players[1].getAutomaton()) {
			env.setSignalDistributionFromRawSignalDistribution(variables);
			env.setCorrelationDeviceFromRawCorrelationDevice(variables);

			pd.setPayoffFromRawPayoff(variables);

			if (!env.checkEnvironment()) {
				Message::alert("Signal distribution / Correlation Device error");
				Message::display("Abort");
				return;
			}

			rg.setVariables(variables);
			rg.configure(env, pd, players);
			rg.solve();

			int result = rg.checkNoEquilibrium();
			Writer resultRFSE(prefix + "-result.txt");
			cout << result << endl;
			resultRFSE.put(toString(result));
			if (result == 2) {
				vector<Belief> candidates = rg.getCandidates();
				for (int i = 0; i < candidates.size(); i++) {
					cout << candidates[i] << endl;
					resultRFSE.put(toString(candidates[i]));
				}
			}
		}
		else {
			Environment env2;
			Payoff pd2;
			vector<Player> players2;
			RepeatedGame rg2;

			env2.setEnvironmentFromEnvironment(env, true);
			pd2.setPayoffFromPayoff(pd, true);

			players2 = players;
			reverse(players2.begin(), players2.end());

			env.setSignalDistributionFromRawSignalDistribution(variables);
			pd.setPayoffFromRawPayoff(variables);

			if (!env.checkEnvironment()) {
				Message::alert("Signal distribution error");
				Message::display("Abort");
				return;
			}

			env2.setSignalDistributionFromRawSignalDistribution(variables);
			pd2.setPayoffFromRawPayoff(variables);

			if (!env2.checkEnvironment()) {
				Message::alert("Signal distribution error");
				Message::display("Abort");
				return;
			}

			rg.setVariables(variables);
			rg.configure(env, pd, players);
			rg.solve();

			rg2.setVariables(variables);
			rg2.configure(env2, pd2, players2);
			rg2.solve();

			Writer resultRFSE(prefix + "-result.txt");
			int result = rg.checkNoEquilibrium();
			cout << result << endl;
			resultRFSE.put(toString(result));
			if (result == 2) {
				vector<Belief> candidates = rg.getCandidates();
				for (int i = 0; i < candidates.size(); i++) {
					cout << candidates[i] << endl;
					resultRFSE.put(toString(candidates[i]));
				}
			}

			int result2 = rg2.checkNoEquilibrium();
			cout << result2 << endl;
			resultRFSE.put(toString(result2));
			if (result == 2) {
				vector<Belief> candidates = rg2.getCandidates();
				for (int i = 0; i < candidates.size(); i++) {
					cout << candidates[i] << endl;
					resultRFSE.put(toString(candidates[i]));
				}
			}
		}
		Message::display("End gracefully");
	}
}

int main(int argc, char **argv) {
	if (argc >= 3) {
		checkRFPE(string(argv[1]), string(argv[2]), argc, argv);
	}
	else {
		Message::display("Usage : rfpe [datafile] [output-prefix] {[variables], ...}");
		Message::display("variables :");
		Message::display("	[name]=[value]    e.g) p=0.5 q=1/6");
	}

	return EXIT_SUCCESS;
}
