/* Copyright (C) Thomas Wheeler 2017 */

#include <iostream>
#include <signal.h>
#include <string>
#include <map>
#include <list>

/* 
	Problem 3: Grammar

	S -> A B C
	A -> a A b | C
	B -> c B | d
	C -> e C | (empty)
*/

using namespace std;

string S();
string A();
string B();
string C();

char token;

void getToken() {
	cin.get(token);
}

void parse() {
	getToken();
	S();
}

int main()
{
	parse();
	return 0;
}