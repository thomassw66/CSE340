#include <iostream>
#include <vector>
#include <map>
#include <set>

using namespace std;

typedef string ID;

struct Rule {
	ID lhs;
	vector<ID> rhs;
};

struct ID_Info {
	bool is_terminal;
	bool is_generating;
	bool is_reachable;
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
	void ListFirstSet();
	void ListFollowSet();
	void GetUsefulRules();
	bool HasPredictiveParser();
	void ListTerminals();
	void ListNonTerminals();
	Grammar();
private:
	// implementation specific details 
	map<ID, vector<Rule > > rule_list;
	map<ID, ID_Info> universe;
	map<ID, set<ID> > first;
	map<ID, set<ID> > follow;
	// vector<ID> universe;
	void AddRule(Rule r);
	void AddID(ID id, bool is_terminal);
	void UpdateID(ID id, bool is_terminal);
	void calculate_reachable_symbols();
	void calculate_generating_symbols();
	void calculate_first_sets();
	void calculate_follow_sets();
	void set_reachable(ID id);
	int id_counter;
	ID start_symbol;
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
	ID rlst1[] = {"idList", "colon", "ID"};
	Rule rule1 = make_rule("decl", rlst1, 3);

	ID rlst2[] = {"ID", "idList1"};
	Rule rule2 = make_rule("idList", rlst2, 2);

	ID rlst3[] = {"#"};
	Rule rule3 = make_rule("idList1", rlst3, 1);

	ID rlst4[] = {"COMMA", "ID", "idList1"};
	Rule rule4 = make_rule("idList1", rlst4, 3);

	AddRule(rule1);
	AddRule(rule2);
	AddRule(rule3);
	AddRule(rule4);

	// ID rlst1[] = {"A", "B"};
	// Rule r1 = make_rule("S", rlst1, 2);

	// ID rlst2[] = {"C"};
	// Rule r2 = make_rule("S", rlst2, 1);

	// ID rlst3[] = {"c"};
	// Rule r3 = make_rule("C", rlst3, 1);

	// ID rlst4[] = {"a"};
	// Rule r4 = make_rule("S", rlst4, 1);

	// ID rlst5[] = {"a", "A"};
	// Rule r5 = make_rule("A", rlst5, 2);

	// ID rlst6[] = {"b"};
	// Rule r6 = make_rule("B", rlst6, 1);

	// AddRule(r1);
	// AddRule(r2);
	// AddRule(r3);
	// AddRule(r4);
	// AddRule(r5);
	// AddRule(r6);

	calculate_generating_symbols();
	calculate_reachable_symbols();
	calculate_first_sets();
	calculate_follow_sets();
	// rule_list[rule1.lhs].push_back(rule1);
	// for (int i = 0; i < rule_list["id1"][0].rhs.size(); i++) {
	// 	cout << rule_list[rule1.lhs][0].rhs[i] << endl;
	// }
	// for (map<ID, vector<Rule> >::iterator rl_it = rule_list.begin(); rl_it != rule_list.end(); rl_it++){
	// 	ID lhs = rl_it->first;
	// 	vector<Rule> rlist = rl_it->second;
	// 	for (vector<Rule>::iterator rule_it = rlist.begin(); rule_it != rlist.end(); rule_it++){
	// 		Rule r = *rule_it;
	// 		print_rule(r);
	// 	}
	// }
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
	// for (it = universe.begin(); it != universe.end(); it++) {
	// 	ID id = it->first;
	// 	if (universe[id].is_generating)
	// 		cout << id << " is generating" << endl; 
	// }
}

void Grammar::calculate_reachable_symbols(){
	set_reachable(start_symbol);
}

void Grammar::set_reachable(ID id)
{
	universe[id].is_reachable = true;
	if (universe[id].is_generating == false) return;

	for (int i = 0; i < rule_list[id].size(); i++) {
	// for each of id's rules
		for (int j = 0; j < rule_list[id][i].rhs.size(); j++){
		// for (symbold in rule.idlist)
			ID symbol = rule_list[id][i].rhs[j]; // we'll never reach the rest of the list 
			if (universe[symbol].is_generating == false) 
				break;
			if (universe[symbol].is_reachable == false) // avoids an infinite recursion
				set_reachable(symbol);
		}
	}
}

void Grammar::GetUsefulRules() {
	map<ID, vector<Rule> >::iterator it;
	for (it = rule_list.begin(); it != rule_list.end(); it++) {
		ID id = it->first;
		for (int i = 0; i < rule_list[id].size(); i++) {
			bool useful = true;
			for (int j = 0; j < rule_list[id][i].rhs.size(); j++) {
				ID sym = rule_list[id][i].rhs[j];
				if (universe[sym].is_generating == false || universe[sym].is_reachable == false){
					useful = false;
					break;
				}
			}
			if (useful) {
				cout << id << " -> ";
				for (int j = 0; j < rule_list[id][i].rhs.size(); j++) {
					cout << rule_list[id][i].rhs[j] << " ";
				}
				cout << endl;
			}
		}
	}
}

void Grammar::AddRule(Rule r)
{
	if (id_counter == 0)
		start_symbol = r.lhs;

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

void Grammar::calculate_first_sets() 
{
	map<ID, ID_Info>::iterator it;
	map<ID, vector<Rule> >::iterator r_it;
	// STEP 1 & 2
	for (it = universe.begin(); it != universe.end(); it++) {
		ID id = it->first;
		set<ID> s;
		first[id] = s;
		if (universe[id].is_terminal) {
			first[id].insert(id);
		} else if (id == "#") {
			first["#"].insert("#");
		}
	}
	bool changed_flag = true;
	while (changed_flag) {	
		changed_flag = false;
		// STEP 3 4 and 5 
		for (r_it = rule_list.begin(); r_it != rule_list.end(); r_it++) {
			ID id = r_it->first;
			int before = first[id].size();
			int after;
			for (Rule a : r_it->second) {
				for (int i = 0; i < a.rhs.size(); i++) {
					ID x = a.rhs[i];
					for (ID s : first[x]) 
						if (s != "#") 
							first[id].insert(s);

					if (first[x].find("#") == first[x].end()) break;
					if (i == a.rhs.size() - 1) first[id].insert("#");
				}
			}
			after = first[id].size();
			if (after > before)
				changed_flag = true;
		}
	}
}	

void Grammar::calculate_follow_sets() 
{
	map<ID, ID_Info>::iterator it;
	map<ID, vector<Rule> >::iterator r_it;
	// STEP 1 & 2
	for (it = universe.begin(); it != universe.end(); it++) {
		ID id = it->first;
		set<ID> s;
		follow[id] = s;
	}
	follow[start_symbol].insert("$");
	bool changed_flag = true;
	while (changed_flag) {
		changed_flag = false;
		for (r_it = rule_list.begin(); r_it != rule_list.end(); r_it++){
			for (Rule r : r_it->second) {
				ID id = r.lhs;
				for (int i = r.rhs.size()-1; i >= 0; i--) {
					ID b = r.rhs[i];
					int before = follow[b].size(), after;
					int j = i+1;
					while (j < r.rhs.size()) {
						ID x = r.rhs[j];
						if (first[x].find("#") == first[x].end()) 
							break;
						j++;
					}
					if (j == r.rhs.size()) 
						for (ID a : follow[id])
							follow[b].insert(a);
					if (j < r.rhs.size())
						for (ID a : first[r.rhs[j]])
							if (a != "#")
								follow[b].insert(a);
					after = follow[b].size();
					if (after > before)
						changed_flag = true;
				}
			}
		}
	}
}

void Grammar::ListFirstSet() 
{
	map<ID, set<ID> >::iterator it; 
	set<ID>::iterator set_it;
	for (it = first.begin(); it != first.end(); it++) {
		ID id = it->first;
		if (!universe[id].is_terminal) {		
			cout << "FIRST(" << id << ") = { ";
			for (set_it = it->second.begin(); set_it != it->second.end(); set_it++) {
				if (set_it != it->second.begin()) cout << ", ";
				cout << *set_it;
			}
			cout << " }" << endl;
		}
	}
}

void Grammar::ListFollowSet()
{
	map<ID, set<ID> >::iterator it; 
	set<ID>::iterator set_it;
	for (it = follow.begin(); it != follow.end(); it++) {
		ID id = it->first;
		if (!universe[id].is_terminal) {		
			cout << "FOLLOW(" << id << ") = { ";
			for (set_it = it->second.begin(); set_it != it->second.end(); set_it++) {
				if (set_it != it->second.begin()) cout << ", ";
				cout << *set_it;
			}
			cout << " }" << endl;
		}
	}
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
	g.ListFirstSet();
	g.ListFollowSet();
}














