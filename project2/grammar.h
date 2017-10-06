#include <iostream>
#include <vector>
#include <map>
#include <set>

typedef std::string Symbol;

struct Rule {
	Symbol lhs;
	std::vector<Symbol> rhs;
	int order;
};

struct Symbol_Info {
	bool is_terminal;
	bool is_generating;
	bool is_reachable;
	Symbol id;
	int order;
};

struct Rule_Key {
	inline bool operator() (const Rule& rule1, const Rule& rule2) {
		return (rule1.order < rule2.order);
	}
};

struct Symbol_Info_Key {
	inline bool operator() (const Symbol_Info& info1, const Symbol_Info& info2) {
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
	Grammar(std::vector<Rule> rule_list);
private:
	// implementation specific details 
	std::map<Symbol, std::vector<Rule > > rule_list;
	std::map<Symbol, Symbol_Info> universe;
	std::map<Symbol, std::set<Symbol> > first;
	std::map<Symbol, std::set<Symbol> > follow;
	// vector<Symbol> universe;
	void AddRule(Rule r);
	void AddSymbol(Symbol id, bool is_terminal);
	void UpdateSymbol(Symbol id, bool is_terminal);
	void calculate_reachable_symbols();
	void calculate_generating_symbols();
	void calculate_first_sets();
	void calculate_follow_sets();
	void set_reachable(Symbol id);
	int id_counter;
	int rule_counter;
	Symbol start_symbol;
};

void print_rule(Rule r);