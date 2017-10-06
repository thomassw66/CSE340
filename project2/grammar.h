#include <iostream>
#include <vector>
#include <map>
#include <set>

typedef std::string ID;

struct Rule {
	ID lhs;
	std::vector<ID> rhs;
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
	Grammar(std::vector<Rule> rule_list);
private:
	// implementation specific details 
	std::map<ID, std::vector<Rule > > rule_list;
	std::map<ID, ID_Info> universe;
	std::map<ID, std::set<ID> > first;
	std::map<ID, std::set<ID> > follow;
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