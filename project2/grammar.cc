#include <iostream>
#include <vector>
#include <map>

using namespace std;

typedef string ID;

struct Rule {
	ID lhs;
	vector<ID> rhs;
};

struct ID_Info {
	bool is_terminal;
	bool is_generating;
	ID id;
	int order;
};

struct ID_Info_Key {
	inline bool operator() (const ID_Info& info1, const ID_Info& info2) {
		return (info1.order < info2.order);
	}
};

class Grammar {
public:
	void GetFirstSet();
	void GetFollowSet();
	bool HasPredictiveParser();
	void ListTerminals();
	void ListNonTerminals();
	Grammar();
private:
	// implementation specific details 
	map<ID, vector<Rule > > rule_list;
	map<ID, ID_Info> universe;
	// vector<ID> universe;
	void AddRule(Rule r);
	void AddID(ID id, bool is_terminal);
	void UpdateID(ID id, bool is_terminal);
	void calculate_generating_symbols();
	int id_counter;
};

void print_rule(Rule r)
{
	cout << r.lhs << " => ";
	for (int i = 0; i < r.rhs.size(); i++) {
		cout << r.rhs[i] << " ";
	}
	cout << endl;
}

Rule make_rule(ID lhs, ID * rhs, int size)
{
	Rule r;
	r.lhs = lhs;
	for (int i = 0; i < size; i++){
		r.rhs.push_back(rhs[i]);
	}
	return r;
}

Grammar::Grammar() {
	id_counter = 0; 
	// Generate a list of rules in here for now 
	ID rlst1[] = {"id", "id1", "id2"};
	Rule rule1 = make_rule("id1", rlst1, 3);

	ID rlst2[] = {"id", "A", "COMMA"};
	Rule rule2 = make_rule("id2", rlst2, 3);

	AddRule(rule1);
	AddRule(rule2);
	calculate_generating_symbols();
	// rule_list[rule1.lhs].push_back(rule1);
	// for (int i = 0; i < rule_list["id1"][0].rhs.size(); i++) {
	// 	cout << rule_list[rule1.lhs][0].rhs[i] << endl;
	// }
	for (map<ID, vector<Rule> >::iterator rl_it = rule_list.begin(); rl_it != rule_list.end(); rl_it++){
		ID lhs = rl_it->first;
		vector<Rule> rlist = rl_it->second;
		for (vector<Rule>::iterator rule_it = rlist.begin(); rule_it != rlist.end(); rule_it++){
			Rule r = *rule_it;
			print_rule(r);
		}
	}
}

// precondition we have added all of the rules to our rule list
// 				we have added all of our symbols to univers
void Grammar::calculate_generating_symbols()
{
	map<ID, ID_Info>::iterator it;
	map<ID, vector<Rule> >::iterator rit;
	bool changed_symbols;
	for (it = universe.begin(); it != universe.end(); it++) {
		ID id = it->first;
		if (universe[id].is_terminal) 
			universe[id].is_generating = true;
		else 
			universe[id].is_generating = false;
	}
	changed_symbols = true;
	while (changed_symbols) {
		changed_symbols = false;
		for (it = universe.begin(); it != universe.end(); it++) {
			ID id = it->first;
			ID_Info info = it->second;
			if (info.is_generating == false) {
				for (int i = 0; i < rule_list[id].size(); i++) {
					bool is_generating = true; 
					for (int j = 0; j < rule_list[id][i].rhs.size(); j++) {
						if (universe[rule_list[id][i].rhs[j]].is_generating == false) {
							is_generating = false;
							break;
						} 
					}
					if (is_generating) {
						universe[id].is_generating = true;
						changed_symbols = true;
					}
				}
			}
		}
	}
}

void Grammar::AddRule(Rule r)
{
	if (universe.find(r.lhs) == universe.end()) {
		AddID(r.lhs, false); // no lhs is terminal
	} else {
		UpdateID(r.lhs, false);
	}
	for (int i = 0; i < r.rhs.size(); i++) {
		ID id = r.rhs[i];
		if (universe.find(id) == universe.end()) {
			AddID(id, true);
		}
	}
	rule_list[r.lhs].push_back(r);
}

void Grammar::AddID(ID id, bool is_terminal) 
{
	ID_Info info;
	info.id = id;
	info.order = id_counter++;
	info.is_terminal = is_terminal;
	universe[id] = info;
}

void Grammar::UpdateID(ID id, bool is_terminal) {
	if (universe.find(id) == universe.end()) 
		return;
	universe[id].is_terminal = is_terminal;
}

void Grammar::GetFirstSet() 
{

}

void Grammar::GetFollowSet()
{

}

bool Grammar::HasPredictiveParser() 
{
	return true;
}

void Grammar::ListTerminals()
{
	vector<ID_Info> tmp; 
	for (std::map<ID, ID_Info>::iterator it = universe.begin(); it != universe.end(); ++it) {
		ID_Info info = it->second;
		if (info.is_terminal == true) {
			tmp.push_back(info);
		}
	}
	sort(tmp.begin(), tmp.end(), ID_Info_Key());
	for (int i = 0; i < tmp.size(); i++) {
		cout << tmp[i].id << " ";
	}
}

void Grammar::ListNonTerminals()
{
	vector<ID_Info> tmp;
	for (std::map<ID, ID_Info>::iterator it = universe.begin(); it != universe.end(); ++it) {
		ID_Info info = it->second;
		if (info.is_terminal == false) {
			tmp.push_back(info);
		}
	}
	sort(tmp.begin(), tmp.end(), ID_Info_Key());
	for (int i = 0; i < tmp.size(); i++) {
		cout << tmp[i].id << " ";
	}
}

int main()
{
	Grammar g;
	cout << g.HasPredictiveParser() << endl;
	cout << "Terminals: " << endl;
	g.ListTerminals();
	cout << endl;
	cout << "Non-Terminals" << endl;
	g.ListNonTerminals();
	cout << endl;
}














