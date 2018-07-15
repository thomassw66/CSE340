
using namespace std;

class Rule {
	string lhs;
	vector<string> rhs;

public:
	Rule(string l, vector<string> r) {
		lhs = string(l);
		for (int i = 0; i < r.size(); i++) {
			rhs.push_back(r[i]);
		}
	}

	string getLHS() {
		return lhs;
	}

	int getRHSSize() {
		return rhs.size();
	}

	string getRHS(int index) {
		if (index >= rhs.size()) return "";
		return rhs[index];
	}

	void print() {
		cout << lhs << " -> ";
		for (string r : rhs) {
			cout << r << " ";
		}
		cout << endl;
	}
};
