#include "grammar.h"

using namespace std;

void print_rule(Rule r)
{
	cout << r.lhs << " -> ";
	for (int i = 0; i < r.rhs.size(); i++) {
		cout << r.rhs[i] << " ";
	}
	cout << endl;
}

Rule make_rule(Symbol lhs, Symbol * rhs, int size)
{
	Rule r;
	r.lhs = lhs;
	for (int i = 0; i < size; i++){
		r.rhs.push_back(rhs[i]);
	}
	return r;
}

Grammar::Grammar(vector<Rule> rule_list) {
	id_counter = 0; 
	rule_counter = 0;
	start_symbol = "NONE_YET";
	AddSymbol("#", true);
	AddSymbol("$", true);

	for (Rule r : rule_list)
		AddRule(r);

	calculate_generating_symbols();
	calculate_reachable_symbols();
	calculate_first_sets();
	calculate_follow_sets();
}

// precondition we have added all of the rules to our rule list
// 				we have added all of our symbols to univers
void Grammar::calculate_generating_symbols()
{
	map<Symbol, Symbol_Info>::iterator it;
	map<Symbol, vector<Rule> >::iterator rit;
	bool changed_flag;
	// all terminals are generating
	for (it = universe.begin(); it != universe.end(); it++) {
		Symbol id = it->first;
		if (universe[id].is_terminal) 
			universe[id].is_generating = true;
		else 
			universe[id].is_generating = false;
	}
	// epsilon is generating
	universe["#"].is_generating = true;

	changed_flag = true;
	while (changed_flag) {
		changed_flag = false;
		for (rit = rule_list.begin(); rit != rule_list.end(); rit++) {
			Symbol id = rit->first;
			if (universe[id].is_generating == true) continue; 

			vector<Rule> rules = rit->second;
			for (int i = 0; i < rules.size(); i++) {
				bool is_generating = true;
				for (int j = 0; j < rules[i].rhs.size(); j++) {
					Symbol a = rules[i].rhs[j];
					if (universe[a].is_generating == false) {
						is_generating = false;
						break;
					}
				}
				if (is_generating) {
					universe[id].is_generating = true;
					changed_flag = true;
				}
			}
		}
	}
	// for (it = universe.begin(); it != universe.end(); it++) {
	// 	Symbol id = it->first;
	// 	if (universe[id].is_generating)
	// 		cout << id << " is generating" << endl; 
	// }
}

void Grammar::calculate_reachable_symbols(){
	map<Symbol, Symbol_Info>::iterator it;
	for (it = universe.begin(); it != universe.end(); it++)
		it->second.is_reachable = false;
	set_reachable(start_symbol);
	// cout << universe["C"].is_reachable << endl << endl;
}

void Grammar::set_reachable(Symbol id)
{
	if (universe.find(id) == universe.end()) return;
	if (universe[id].is_generating) {
		universe[id].is_reachable = true;
		for (int i = 0; i < rule_list[id].size(); i++) {
		// for each of id's rules
			for (int j = 0; j < rule_list[id][i].rhs.size(); j++){
			// for (symbold in rule.idlist)
				Symbol symbol = rule_list[id][i].rhs[j]; // we'll never reach the rest of the list 
				// cout << "symbol = " << symbol << endl;
				if (universe[symbol].is_generating == false) break;
				if (universe[symbol].is_reachable == false) // avoids an infinite recursion
					set_reachable(symbol);
			}
		}
	}	
}

void Grammar::GetUsefulRules() {
	map<Symbol, vector<Rule> >::iterator it;
	vector<Rule> useful;
	for (it = rule_list.begin(); it != rule_list.end(); it++) {
		Symbol id = it->first;
		for (int i = 0; i < rule_list[id].size(); i++) {
			Rule r = rule_list[id][i];
			bool is_useful = true;
			// if (id == "D") cout << universe[id].is_reachable << endl;
			// if id is not reachable then none of its rules can be useful
			// if (id == "C") cout << universe["C"].is_reachable << endl << endl;
			if (universe[id].is_reachable == false) is_useful = false;
			// otherwise add the useful ones 
			for (int j = 0; j < rule_list[id][i].rhs.size(); j++) {
				Symbol sym = rule_list[id][i].rhs[j];
				if (universe[sym].is_generating == false){
					is_useful = false;
					break;
				}
			}
			if (is_useful) {
				useful.push_back(rule_list[id][i]);
			}
		}
	}
	sort(useful.begin(), useful.end(), Rule_Key());
	for (Rule r : useful)
		print_rule(r);
}

void Grammar::AddRule(Rule r)
{
	if (id_counter == 0)
		start_symbol = r.lhs;

	r.order = rule_counter++;

	if (universe.find(r.lhs) == universe.end()) {
		AddSymbol(r.lhs, false); // no lhs is terminal
	} else {
		UpdateSymbol(r.lhs, false);
	}
	for (int i = 0; i < r.rhs.size(); i++) {
		Symbol id = r.rhs[i];
		if (universe.find(id) == universe.end()) {
			AddSymbol(id, true);
		}
	}
	rule_list[r.lhs].push_back(r);
}

void Grammar::AddSymbol(Symbol id, bool is_terminal) 
{
	Symbol_Info info;
	info.id = id;
	if (id == "#") {
		info.is_terminal = true;
		info.order = -1;
	} else if (id == "$") {
		info.is_terminal = true;
		info.order = -2;
	} else {
		info.order = id_counter++;
		info.is_terminal = is_terminal;	
	}
	universe[id] = info;
}

void Grammar::UpdateSymbol(Symbol id, bool is_terminal) {
	if (universe.find(id) == universe.end()) 
		return;
	universe[id].is_terminal = is_terminal;
}

void Grammar::calculate_first_sets() 
{
	map<Symbol, Symbol_Info>::iterator it;
	map<Symbol, vector<Rule> >::iterator r_it;
	// STEP 1 & 2
	for (it = universe.begin(); it != universe.end(); it++) {
		Symbol id = it->first;
		set<Symbol> s;
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
			Symbol id = r_it->first;
			int before = first[id].size();
			int after;
			for (Rule a : r_it->second) {
				for (int i = 0; i < a.rhs.size(); i++) {
					Symbol x = a.rhs[i];
					for (Symbol s : first[x]) 
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
	map<Symbol, Symbol_Info>::iterator it;
	map<Symbol, vector<Rule> >::iterator r_it;
	// STEP 1 & 2
	for (it = universe.begin(); it != universe.end(); it++) {
		Symbol id = it->first;
		set<Symbol> s;
		follow[id] = s;
	}
	if (universe.find(start_symbol) == universe.end()) return;
	follow[start_symbol].insert("$");

	bool changed_flag = true;
	while (changed_flag) {
		changed_flag = false;
		for (r_it = rule_list.begin(); r_it != rule_list.end(); r_it++){
			for (Rule r : r_it->second) {
				Symbol id = r.lhs;
				for (int i = r.rhs.size()-1; i >= 0; i--) {
					Symbol b = r.rhs[i];
					int before = follow[b].size(), after;
					int j = i+1;
					while (j < r.rhs.size()) {
						Symbol x = r.rhs[j];
						for (Symbol a : first[x])
							if (a != "#")
								follow[b].insert(a);
						if (first[x].find("#") == first[x].end())  // IF epsilon is not in first[x] then break
							break;
						j++;
					}
					if (j == r.rhs.size()) 
						for (Symbol a : follow[id])
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
	map<Symbol, set<Symbol> >::iterator it; 
	set<Symbol>::iterator set_it;
	vector<Symbol_Info> ntsymbols;
	for (it = first.begin(); it != first.end(); it++) {
		Symbol id = it->first;
		if (!universe[id].is_terminal) {	
			ntsymbols.push_back(universe[id]);
		}
	}
	sort(ntsymbols.begin(), ntsymbols.end(), Symbol_Info_Key());
	for (Symbol_Info s : ntsymbols) {
		vector<Symbol_Info> sortedset;
		for (set_it = first[s.id].begin(); set_it != first[s.id].end(); set_it++) {
			sortedset.push_back(universe[*set_it]);
		}
		sort(sortedset.begin(), sortedset.end(), Symbol_Info_Key());
		cout << "FIRST(" << s.id << ") = { ";
		for (int i = 0; i < sortedset.size(); i++) {
			if (i != 0) cout << ", ";
			cout << sortedset[i].id;
		}
		cout << " }" << endl;
	}
}

void Grammar::ListFollowSet()
{
	map<Symbol, set<Symbol> >::iterator it; 
	set<Symbol>::iterator set_it;
	vector<Symbol_Info> ntsymbols;
	for (it = follow.begin(); it != follow.end(); it++) {
		Symbol id = it->first;
		if (!universe[id].is_terminal) {	
			ntsymbols.push_back(universe[id]);
		}
	}
	sort(ntsymbols.begin(), ntsymbols.end(), Symbol_Info_Key());
	for (Symbol_Info s: ntsymbols) {
		vector<Symbol_Info> sortedset;
		for (set_it = follow[s.id].begin(); set_it != follow[s.id].end(); set_it++) {
			sortedset.push_back(universe[*set_it]);
		}
		sort(sortedset.begin(), sortedset.end(), Symbol_Info_Key());
		cout << "FOLLOW(" << s.id << ") = { ";
		for (int i = 0; i < sortedset.size(); i++) {
			if (i != 0) cout << ", ";
			cout << sortedset[i].id;
		}
		cout << " }" << endl;
	}
}

bool Grammar::HasPredictiveParser() 
{
	bool pparse = true;
	map<Symbol, Symbol_Info>::iterator it;
	for (it = universe.begin(); it != universe.end(); it++) {
		Symbol a = it->first;
		if (universe[a].is_terminal == false && universe[a].is_reachable) {
			for (int i = 0; i < rule_list[a].size(); i++) {
				for (int j = i+1; j < rule_list[a].size(); j++) {
					Rule r1 = rule_list[a][i];
					Rule r2 = rule_list[a][j];
					if (r1.rhs.size() >= 1 && r2.rhs.size() >= 1) {
						Symbol x1 = r1.rhs[0];
						Symbol x2 = r2.rhs[0];
						set<Symbol> f1 = first[x1];
						set<Symbol> f2 = first[x2];
						if (universe[x1].is_terminal == true && universe[x2].is_terminal == true) {
							if (x1 == x2) {
								pparse = false;
								// cout << "T and T" << endl;
							}
						} else if (universe[x1].is_terminal == false && universe[x2].is_terminal == false) {	
							set<Symbol> intersect;
							set_intersection(f1.begin(), f1.end(), f2.begin(), f2.end(), inserter(intersect, intersect.begin()));
							if (!intersect.empty()){
								pparse = false;
								// cout << "NT and NT" << endl;
							}
						} else if (universe[x1].is_terminal) {
							if (f2.find(x1) != f2.end()) {
								pparse = false;
								// cout << "T and NT" << endl;
							}
						} else {
							if (f1.find(x2) != f1.end()){
								pparse = false;
								// cout << "NT and T" << endl;
							}
						}
					}
				}
			}			
			if (first[a].find("#") != first[a].end()) {
				set<Symbol> f1 = first[a];
				set<Symbol> f2 = follow[a];
				set<Symbol> intersect;
				set_intersection(f1.begin(), f1.end(), f2.begin(), f2.end(), inserter(intersect, intersect.begin()));
				if (!intersect.empty())
					pparse = false;
			}
		}
	}
	if (pparse) 
		cout << "YES" << endl;
	else 
		cout << "NO" << endl;
	return pparse;
}

void Grammar::ListTerminals()
{
	vector<Symbol_Info> tmp; 
	for (std::map<Symbol, Symbol_Info>::iterator it = universe.begin(); it != universe.end(); ++it) {
		Symbol_Info info = it->second;
		if (info.is_terminal == true && info.id != "#" && info.id != "$") {
			tmp.push_back(info);
		}
	}
	sort(tmp.begin(), tmp.end(), Symbol_Info_Key());
	for (int i = 0; i < tmp.size(); i++) {
		cout << tmp[i].id << " ";
	}
}

void Grammar::ListNonTerminals()
{
	vector<Symbol_Info> tmp;
	for (std::map<Symbol, Symbol_Info>::iterator it = universe.begin(); it != universe.end(); ++it) {
		Symbol_Info info = it->second;
		if (info.is_terminal == false) {
			tmp.push_back(info);
		}
	}
	sort(tmp.begin(), tmp.end(), Symbol_Info_Key());
	for (int i = 0; i < tmp.size(); i++) {
		cout << tmp[i].id << " ";
	}
}












