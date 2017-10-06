/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "lexer.h"
#include "grammar.h"

using namespace std;

void parseRuleList();
Rule parseRule();
void parseIDList();
vector<Symbol> parseRightHandSide();
void parseID();
void parseArrow();

LexicalAnalyzer lexer;
Token token;

vector<Rule> rules;

void getToken()
{
    token = lexer.GetToken();
}

void error(string message)
{
    cout << message << endl;
    exit(0);
}

string matchToken(TokenType type, string message) 
{
    string ret = token.lexeme;
    if (token.token_type == type) getToken();
    else error(message);
    return ret;
}

void parseRuleList() 
{
    Rule r = parseRule();
    rules.push_back(r);
    if (token.token_type != DOUBLEHASH) 
        parseRuleList();
}

Rule parseRule() 
{
    Rule r;
    r.lhs = matchToken(ID, "expected ID");
    matchToken(ARROW, "expected ARROR");
    r.rhs = parseRightHandSide();
    return r;
}

vector<Symbol> parseRightHandSide() 
{
    vector<Symbol> symbols;
    while (token.token_type != HASH) {
        Symbol id = matchToken(ID, "expected ID");
        symbols.push_back(id);
    }
    matchToken(HASH, "expected HASH");
    if (symbols.size() == 0)
        symbols.push_back("#");
    return symbols;
}

int main (int argc, char* argv[])
{
    int task;

    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);

    // TODO: Read the input grammar at this point from standard input
    getToken();
    parseRuleList();
    // for (Rule r : rules) {
    //     print_rule(r);;
    // }
    Grammar g (rules);
    /*
       Hint: You can modify and use the lexer from previous project
       to read the input. Note that there are only 4 token types needed
       for reading the input in this project.

       WARNING: You will need to modify lexer.cc and lexer.h to only
       support the tokens needed for this project if you are going to
       use the lexer.
     */

    switch (task) {
        case 1:
            // TODO: perform task 1.
            g.ListTerminals();
            g.ListNonTerminals();
            cout << endl;
            break;

        case 2:
            // TODO: perform task 2.
            g.GetUsefulRules();
            break;

        case 3:
            // TODO: perform task 3.
            g.ListFirstSet();
            break;

        case 4:
            // TODO: perform task 4.
            g.ListFollowSet();
            break;

        case 5:
            // TODO: perform task 5.
            g.HasPredictiveParser();
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }
    return 0;
}

