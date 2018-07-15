#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include "rule.h"

using namespace std;

// typedef std::string Symbol;

class Symbol {
	bool is_terminal;
	bool is_generating;
	bool is_reachable;
	string name;
	set<Symbol*> first;
	set<Symbol*> follow;

public: 
	Symbol(string id) {
		// this->is_terminal = true;
		// this->is_generating = false;
		// this->is_reachable = false;
		this->name = id;
		// this->first = new std::set<Symbol*>();
		// this->follow = new std::set<Symbol*>();
	}
};

class Grammar {
	vector<Rule*> rule_list;
	set<string> universe;
	// map<string, Symbol*> universe;

public:
	Grammar(vector<Rule*> rules);

	// TERMINALS & NONTERMINALS
	vector<string> * getNonTerminals();
	vector<string> * getTerminals();
	// FIRST & FOLLOW
	set<string> * getFirstSet(string id); 
	set<string> * getFollowSet(string id); 
	
	set<string> * getGeneratingSymbols();
	set<string> * getReachableSymbols();

private:
	// implementation specific details 
	// vector<Symbol> universe;
	void addRule(Rule *r);
	void addSymbol(string id);
	bool isTerminal(string id);

	void calculateReachableSymbols() {}
	void calculateGeneratingSymbols() {}
	void calculateFirstSets() {}
	void calculateFollowSets() {}
	string startSymbol;
};

void print_rule(Rule r);
