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
Rule * parseRule();
void parseIDList();
vector<string> parseRightHandSide();
void parseID();
void parseArrow();

LexicalAnalyzer lexer;
Token token;

vector<Rule*> rules;

static string reserved[] = { "END_OF_FILE", "ARROW", "HASH", "DOUBLEHASH", "ID", "ERROR" };

void print_set(set<string> * s) {
    cout << "{ ";
    for (string member : *s)
        cout << member << " ";
    cout << "}" << endl;
}

void getToken()
{
    token = lexer.GetToken();
}

void error(string message)
{
    cout << message << endl;
    exit(0);
}

Token matchToken(TokenType type, string message) 
{
    Token ret = token;
    if (token.token_type != type) {
        cout << "expected: " << reserved[(int) type] << endl;
        cout << "read: " << reserved[(int) token.token_type] << endl;
        cout << "at line # " << token.line_no << endl;
        error(message);

    }
    getToken();
    return ret;
}

void parseRuleList() 
{
    Rule * r = parseRule();
    rules.push_back(r);
    if (token.token_type != DOUBLEHASH) 
        parseRuleList();
}

Rule * parseRule() 
{
    Token t = matchToken(ID, "expected ID");
    string lhs = t.lexeme;
    matchToken(ARROW, "expected ARROW");
    vector<string> rhs = parseRightHandSide();
    return new Rule(lhs, rhs);
}

vector<string> parseRightHandSide() 
{
    vector<string> symbols;
    while (token.token_type != HASH) {
        Token t = matchToken(ID, "expected ID");
        string id = t.lexeme;
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
    vector<string> * nonTerminals;
    vector<string> * terminals;
    set<string> * generating; 

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
    // for (Rule * r : rules) {
    //     r->print();
    // }
    Grammar * g = new Grammar (rules);
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
            // Print nonTerminals
            nonTerminals = g->getNonTerminals();
            // TODO: sort ?
            for (string nt : *nonTerminals) {
                cout << nt << endl;
            }
            cout << endl;
            // Print terminals
            terminals = g->getTerminals();
            // TODO: sort ?
            for (string t : *terminals) {
                cout << t << endl;
            }
            cout << endl;

            delete nonTerminals;
            delete terminals; 

            break;
        case 2:
            // TODO: perform task 2.
            generating = g->getGeneratingSymbols();
            print_set(generating);
            break;
        case 3:
            // TODO: perform task 3.
            // g->ListFirstSet();
            cout << "Task 3" << endl;;
            nonTerminals = g->getNonTerminals();
            cout << nonTerminals->size() << endl;
            for (string nt : *nonTerminals) {
                cout << "FIRST(" << nt<<") = " << endl;
                set<string> * firstSet = g->getFirstSet(nt);
                print_set(firstSet);
            }
            delete nonTerminals;
            break;

        case 4:
            // TODO: perform task 4.
            nonTerminals = g->getNonTerminals();
            // g->ListFollowSet();
            for (string nt : *nonTerminals) {
                cout << "FOLLOW(" << nt << ") = " << endl;
                set<string> * followSet = g->getFollowSet(nt);
                print_set(followSet);
            }
            delete nonTerminals;
            break;

        case 5:
            // TODO: perform task 5.
            // g->HasPredictiveParser();
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }
    return 0;
}

