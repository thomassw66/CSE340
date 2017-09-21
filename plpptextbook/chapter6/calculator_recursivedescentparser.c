#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

int token; /* global token variable: holds the current input character for the parse */

/* We need these declarations to do arbitrary recursion */
void command();
int expr();
int term();
int factor();
int number();
int digit();

void error(char * message) {
	printf("parse error: %s\n", message);
	exit(1);
}

void getToken() {
	token = getchar();
	while(token == ' ') token = getchar();
}

void match(char c, char * message) {
	if (token == c) getToken();
	else error(message);
}

void command() {
	/* command ->  expr '\n' */
	int result = expr();
	if (token == '\n') 
		printf("The result is: %d\n", result);
	else error("tokens after end of expression");
}

int expr() {
	/* expr -> term { '+' term } */
	int result = term();
	while (token == '+') {
		match('+', "+ expected");
		result += term();
	}
	return result;
}

int term() {
	/* term -> factor { '*' factor } */
	int result = factor();
	while (token == '*') {
		match('*', "* expected");
		result *= factor();
	}
	return result;
}

int factor() {
	/* factor -> '('' expr ')' | number */
	int result;
	if (token == '(') {
		match('(', "( expected");
		result = expr();
		match(')', ") expected");
	}
	else 
		result = number();
	return result;
}

int number() {
	/* number = digit { digit }*/
	int result = digit();
	while (isdigit(token)) {
		result = result * 10 + digit();
	}
	return result;
}

int digit() {
	/* digit = [0-9] */
	int result;
	if (isdigit(token)) {
		result = token - '0';
		match(token, "different digit than expected");
	}
	else 
		error("digit expected");
	return result;
}

void parse() {
	getToken();
	command();
}

int main() {
	parse();
	return 0;
}