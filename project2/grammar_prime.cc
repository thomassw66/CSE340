#include <string>

#define MAX_SYMBOL 1000
#define MAX_RULES 1000

using namespace std;

class Rule {
public:
	string lhs;
	RuleNode rhs;
}

class RuleNode {
public:
	string id;
	RuleNode next;
}

class GrammarPrime {
public:
	void AddRule(Rule r);
	bool isTerminal(ID id);
private:
	void AddSymbol(string id);
	int indexInUniverse(string id);

	string universe[MAX_SYMBOL];
	Rule rules[MAX_RULES];

	bool isTerminal[MAX_SYMBOL];


	int ruleCounter = 0;
	int idCounter = 0;
	string startSymbol;
}

GrammarPrime::GrammarPrime() 
{
	AddSymbol("#"); // Epsilon
	AddSymbol("$"); // End Of File 
}

void GrammarPrime::AddSymbol(string id) 
{
	if (indexInUniverse(id) != -1) return; // Don't add duplicate symbols 
	universe[idCounter++] = id;
}

int GrammarPrime::indexOf(ID id) 
{
	for (int i = 0; i < idCounter; i++) 
		if (symbols[i] == id) return i;
	return -1;
}

void GrammarPrime::AddRule(Rule r) 
{
	if (ruleCounter == 0) { // handle setting the start symbol 
		startSymbol = r.lhs;
	}

	// add all unseen symbols to our universe
	AddSymbol(r.lhs);
	for (ListNode c = r.rhs; c != NULL; c = c->next) {
		AddSymbol(c.id);
	}

	rules[ruleCounter++] = r; 
}