#include <iostream>
#include <string>
#include <cstdlib>
#include <map> 

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

map<string, ValueNode*> memory;


struct StatementNode* parse_program();
void parse_var_section();
void parse_id_list();
struct StatementNode* parse_stmt_list();
struct StatementNode* parse_stmt();
struct StatementNode* parse_body();
struct AssignmentStatement* parse_assign_stmt();
struct StatementNode* parse_expr();
ValueNode* parse_primary(); // void for now 
ArithmeticOperatorType parse_op();
struct PrintStatement* parse_print_stmt();
struct IfStatement* parse_while_stmt();
struct IfStatement* parse_if_stmt();
void parse_condition();
ConditionalOperatorType parse_relop();
struct StatementNode* parse_switch_stmt();
struct StatementNode* parse_for_stmt();
void parse_case_list(ValueNode* var, StatementNode* end);
void parse_case();
void parse_default_case();
void error(string message);
void syntax_error();
ValueNode* get_value_node(string id);
ValueNode* get_value_node(int constant);
Token match(TokenType type);
Token peek();
ArithmeticOperatorType get_op(TokenType type);
void declare_var(string id);

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

ConditionalOperatorType parse_relop()
{
	Token t = lexer.GetToken();
	if (t.token_type == NOTEQUAL) {
		return CONDITION_NOTEQUAL;
	} 
	else if (t.token_type == GREATER) {
		return CONDITION_GREATER;
	}
	else if (t.token_type == LESS) {
		return CONDITION_LESS;
	} 
	else {
		syntax_error();
	}

}

ArithmeticOperatorType get_op(TokenType t) {
	if (t == PLUS) {
		return OPERATOR_PLUS;
	}
	if (t == MINUS) {
		return OPERATOR_MINUS;
	}
	if (t == DIV) {
		return OPERATOR_DIV;
	}
	if (t == MULT) {
		return OPERATOR_MULT;
	}
	return OPERATOR_NONE;
}

StatementNode* make_no_op() 
{
	StatementNode* no = new StatementNode;
	no->next = NULL;
	no->type = NOOP_STMT;
	return no;
}

void declare_var(string id) 
{
	memory[id] = new ValueNode;
	memory[id]->name = id;
	memory[id]->value = 0;
}


void parse_id_list()
{
	Token id, l;

	id = match(ID);
	declare_var(id.lexeme);

	while ((l = peek()).token_type == COMMA) {
		match(COMMA);
		id = match(ID);
		declare_var(id.lexeme);
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
	if (memory.find(id) == memory.end()) {
		cout << "Error: identifier [" << id << " accessed but never declared" << endl;
		exit(1);
	}
	return memory[id];
}

ValueNode* make_value_node_constant(int constant) 
{
	ValueNode* n = new ValueNode;
	n->name = "";
	n->value = constant;
	return n;
}

int atoi (string s)
{
	int i = 0; 
	for (int n = 0; n < s.length(); n++) {
		i = i * 10;
		i += (s[n] - '0');
	}
	return i;
}

ValueNode* parse_primary() {
	Token t = lexer.GetToken();
	if (t.token_type == ID) return get_value_node(t.lexeme);
	if (t.token_type == NUM) return make_value_node_constant(atoi(t.lexeme));
	else syntax_error(); 
}

struct AssignmentStatement* parse_assign_stmt() 
{
	Token t;
	AssignmentStatement* st = new AssignmentStatement;
	Token left = match(ID);
	st->left_hand_side = get_value_node(left.lexeme);

	match(EQUAL);
 
	st->operand1 = parse_primary();
	
	t = peek();
	// expression -> primary | primary op primary
	if (is_op(t.token_type)) {
		Token opTok = match(t.token_type);
		st->op = get_op(opTok.token_type);
		st->operand2 = parse_primary();
	} else {
		//
		st->op = OPERATOR_NONE;
		st->operand2 = NULL;
	}
	match(SEMICOLON);

	return st;
}

struct PrintStatement* parse_print_stmt() 
{
	PrintStatement* p = new PrintStatement;
	match(PRINT);
	Token idTok = match(ID);
	match(SEMICOLON);
	p->id = get_value_node(idTok.lexeme);
	return p;
}

struct IfStatement* parse_while_stmt() 
{
	IfStatement* while_stmt = new IfStatement;
	match(WHILE);
	while_stmt->condition_operand1 = parse_primary();
	while_stmt->condition_op = parse_relop();
	while_stmt->condition_operand2 = parse_primary();
	while_stmt->true_branch = parse_body();
	while_stmt->false_branch = NULL;
	return while_stmt;
}

struct IfStatement* parse_if_stmt() 
{
	IfStatement* if_node = new IfStatement;
	match(IF);
	if_node->condition_operand1 = parse_primary();
	if_node->condition_op = parse_relop();
	if_node->condition_operand2 = parse_primary();
	if_node->true_branch = parse_body();
	if_node->false_branch = NULL; // for now 
	return if_node;
}

void parse_case_list(ValueNode* var, StatementNode* end) 
{
	// should return the switch stmt subgraph
	// if var == c1 {  ....; goto end; }
	// if var == c2 { ...; goto end; }
	// end:
	Token t = peek();
	while (t.token_type == CASE) {
		Token numTok;
		StatementNode* body;

		match(CASE);
		numTok = match(NUM);
		match(COLON);
		body = parse_body();

		IfStatement* if_node = new IfStatement;
		if_node->condition_operand1 = var;
		if_node->condition_op = CONDITION_EQUAL;
		if_node->condition_operand2 = get_value_node(numTok.lexeme)
		if_node->true_branch = body;
		if_node->false_branch = NULL;

	}
	t = peek();
	if (t.token_type == DEFAULT) {
		StatementNode* body;

		match(DEFAULT);
		match(COLON);
		body = parse_body();
	}
}

struct StatementNode* parse_switch_stmt() 
{
	ValueNode* var_node;
	StatementNode* end = make_no_op();
	match(SWITCH);
	Token varTok = match(ID);
	var_node = get_value_node(varTok.lexeme);
	match(LBRACE);
	parse_case_list(var_node, end);
	match(RBRACE);
}

struct StatementNode* parse_stmt() 
{
// 	// TODO: 
// 	// stmt -> assign_stmt | print_stmt | while_stmt | if_stmt | switch_stmt 
	StatementNode* st = new StatementNode;
// 	st->next = NULL;
// 	Token t = peek();
// 	if (t.token_type == ID) {
// 		st->type = ASSIGN_STMT;
// 		st->assign_stmt = parse_assign_stmt();
// 	}	
// 	else if (t.token_type == PRINT) {
// 		st->type = PRINT_STMT;
// 		st->print_stmt = parse_print_stmt();
// 	}
// 	else if (t.token_type == WHILE) {
// 		st->type = IF_STMT;
// 		st->if_stmt = parse_while_stmt();
// 	} 
// 	else if (t.token_type == IF) {
// 		st->type = IF_STMT;
// 		st->if_stmt = parse_if_stmt();
// 	}
// 	else if (t.token_type == SWITCH) {
// 		// st
// 		parse_switch_stmt();
// 	} else {
// 		syntax_error();
// 	}
	return st; 
}

void tail_if_stmt (StatementNode* node, StatementNode* tail) 
{
	StatementNode* noop = make_no_op();
	noop->next = tail;
	node->next = noop;
	node->if_stmt->false_branch = noop;

	StatementNode* tmp = node->if_stmt->true_branch;
	while (tmp->next != NULL) tmp = tmp->next;
	tmp->next = noop;
}

void tail_while_stmt(StatementNode* node, StatementNode* tail)
{
	StatementNode* noop = make_no_op();
	noop->next = tail;
	node->next = noop; 
	node->if_stmt->false_branch = noop;

	StatementNode* goto_node = new StatementNode;
	goto_node->type = GOTO_STMT;
	goto_node->goto_stmt = new GotoStatement;
	goto_node->goto_stmt->target = node;

	StatementNode* tmp = node->if_stmt->true_branch;
	while(tmp->next != NULL) tmp = tmp->next;
	tmp->next = goto_node;
}

struct StatementNode* parse_stmt_list()
{
	StatementNode* st; // statement 
	StatementNode* stl; // statement list 

	Token t1, t2;
	t1 = peek();
	st = new StatementNode;
	if (t1.token_type == ID) {
		st->type = ASSIGN_STMT;
		st->assign_stmt = parse_assign_stmt();
	}	
	else if (t1.token_type == PRINT) {
		st->type = PRINT_STMT;
		st->print_stmt = parse_print_stmt();
	}
	else if (t1.token_type == WHILE) {
		st->type = IF_STMT;
		st->if_stmt = parse_while_stmt();
	} 
	else if (t1.token_type == IF) {
		st->type = IF_STMT;
		st->if_stmt = parse_if_stmt();
	}
	else if (t1.token_type == SWITCH) {
		// st
		parse_switch_stmt();
	} else {
		syntax_error();
	}


	// st = parse_stmt();
	t2 = peek(); 

	if (t2.token_type != RBRACE) {
		stl = parse_stmt_list();
	} 
	else {
		stl = NULL;
	}

	if (t1.token_type == IF) {
		tail_if_stmt(st, stl);
	} 
	else if (t1.token_type == WHILE) {
		tail_while_stmt(st, stl);
	}
	// else if (t1.token_type == SWITCH) {}
	else {
		// st -> stl 
		st->next = stl;
	}
	
	// generate statement list subgraph from a statement and preceding statement list graph
	
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