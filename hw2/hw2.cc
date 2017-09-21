/* Copyright (C) Thomas Wheeler 2017 */

#include <stdlib.h>
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

void error(string message)
{
	cout << message << endl;
	exit(0);
}

void getToken()
{
	cin.get(token);
}

void match(char c, string message) 
{
	if (token == c) getToken();
	else error(message);
}

void parse() 
{
	getToken();
	cout << S() << endl;
}

string S() 
{
	string result = "";
	result += A();
	result += B();
	result += C();
	return result;
}

string A() 
{
	string result = "";
	if (token == 'a') {
		match('a', "a expected");
		result += 'a';
		result += A();
		match('b', "b expected");
		result += 'b';
	} 
	else 
		result += C();
	return result;
}

string B() {
	string result = "";
	if (token == 'c') {
		match('c', "c expected");
		result += 'c';
		result += B();
	}
	else if (token == 'd') {
		match('d', "d expected");
		result += 'd';
	} 
	else 
		error("B expected");
	return result;
}

string C() {
	string result = "";
	if (token == 'e') {
		match('e', "e expected");
		result += 'e';
	}
	return result;
}

int main()
{
	parse();
	return 0;
}