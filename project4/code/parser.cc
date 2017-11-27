#include <iostream>
#include <string>
#include <cstdlib>

#include "compiler.h"
#include "ir_debug.h"
#include "parser.h"
#include "lexer.h"

using namespace std;

#ifndef DEBUG
#define DEBUG 1
#endif

Token t;
LexicalAnalyzer lexer;


struct StatementNode* parse_program();
void parse_var_section();
void parse_id_list();
struct StatementNode* parse_stmt_list();
struct StatementNode* parse_stmt();
struct StatementNode* parse_body();
struct StatementNode* parse_assign_stmt();
struct StatementNode* parse_expr();
void parse_primary(); // void for now 
ArithmeticOperatorType parse_op();
struct StatementNode* parse_print_stmt();
struct StatementNode* parse_while_stmt();
struct StatementNode* parse_if_stmt();
void parse_condition();
void parse_relop();
struct StatementNode* parse_switch_stmt();
struct StatementNode* parse_for_stmt();
void parse_case_list();
void parse_case();
void parse_default_case();
void error(string message);
void syntax_error();
ValueNode* get_value_node(string id);
ValueNode* get_value_node(int constant);
Token match(TokenType type);
Token peek();


void syntax_error() 
{
	cout << "Syntax Error" << endl;
	exit(1);
}

void error(string message) 
{
	cout << "Error: " << message << endl;
	exit(1);
} 

Token peek()
{
	Token t = lexer.GetToken();
	lexer.UngetToken(t);
	return t; 
}

Token match(TokenType expected_type)
{
	Token t = lexer.GetToken();
	if (t.token_type != expected_type) {
		if (DEBUG) {
			t.Print();
		}
		syntax_error();
	}
	return t; // is returning it actually useful? cuz we can 
}


void parse_id_list()
{
	Token id, l;

	id = match(ID);

	while ((l = peek()).token_type == COMMA) {
		match(COMMA);
		match(ID);
		// store them in some datastructure?
	}
	// return hypothetical datastructure ? 
}

void parse_var_section()
{
	parse_id_list();
	match(SEMICOLON);
}

bool is_op(TokenType type) 
{
	return type == PLUS || type == MINUS || type == MULT || type == DIV;
}

ValueNode* get_value_node(string id)
{
	// should try not faking it
	return NULL;
}

ValueNode* get_value_node(int constant) 
{
	ValueNode n = new ValueNode;
	n->name = "";
	n->value = constant;
	return n;
}

struct AssignmentStatement* parse_assign_stmt() 
{
	Token t;
	AssignmentStatement* st = new AssignmentStatement;
	Token left = match(ID);
	st->left_hand_side = get_value_node(left.lexeme);

	match(EQUAL);
 
	Token r1 = parse_primary();
	st->operand1 = get_value_node(r1.lexeme);
	
	t == peek();
	// expression -> primary | primary op primary
	if (is_op(t.token_type)) {
		Token opTok = match(t.token_type);
		Token r2 = parse_primary();

		st->op = get_op(opTok.token_type);
		st->operand2 = get_value_node(r2.lexeme);
	} else {
		//
		st->op = NULL;
		st->operand2 = NULL;
	}
	match(SEMICOLON);

	return st;
}

struct StatementNode* parse_print_stmt() 
{
	return NULL;
}

struct StatementNode* parse_while_stmt() 
{
	return NULL;
}

struct StatementNode* parse_if_stmt() 
{
	return NULL;
}

struct StatementNode* parse_switch_stmt() 
{
	return NULL;
}

struct StatementNode* parse_stmt() 
{
	// TODO: 
	// stmt -> assign_stmt | print_stmt | while_stmt | if_stmt | switch_stmt 
	StatementNode* st = new StatementNode;
	Token t = peek();
	if (t.token_type == ID) {
		st->type = ASSIGN_STMT;
		st->assign_stmt = parse_assign_stmt();
	}	
	else if (t.token_type == PRINT) {
		st->type = PRINT_STMT;
		st->print_stmt = parse_print_stmt();
	}
	else if (t.token_type == WHILE) {
		return parse_while_stmt();
	} 
	else if (t.token_type == IF) {
		return parse_if_stmt();
	}
	else if (t.token_type == SWITCH) {
		return parse_switch_stmt();
	} else {
		syntax_error();
	}
	return st; 
}

struct StatementNode* parse_stmt_list()
{
	StatementNode* st; // statement 
	StatementNode* stl; // statement list 

	Token t; 

	st = parse_stmt();
	t = peek(); 

	if (t.token_type != RBRACE) 
		stl = parse_stmt_list();
	
	// generate statement list subgraph from a statement and preceding statement list graph
	st->next = stl;
	return st; // such code! very return! yay! 
}


struct StatementNode* parse_body() 
{
	struct StatementNode* st1;

	match(LBRACE);
	st1 = parse_stmt_list();
	match(RBRACE);

	return st1;
}

struct StatementNode* parse_program() 
{
	struct StatementNode * n1; 
	
	parse_var_section(); // variables declared here
	n1 = parse_body(); 	// program code graph generated in here

	return n1; 	
}