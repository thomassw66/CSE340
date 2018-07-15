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


struct StatementNode* parse_generate_intermediate_representation();
struct StatementNode* parse_program();
struct StatementNode* parse_stmt_list();
struct StatementNode* parse_body();

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
		cout << "Error: identifier [" << id << "] accessed but never declared" << endl;
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

struct StatementNode* parse_assign_stmt() 
{
	Token t;
	StatementNode* stmt = new StatementNode;
	stmt->type = ASSIGN_STMT;

	AssignmentStatement* st = new AssignmentStatement;
	stmt->assign_stmt = st;
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

	return stmt;
}

struct StatementNode* parse_print_stmt() 
{
	StatementNode* stmt = new StatementNode;
	stmt->type = PRINT_STMT;

	PrintStatement* p = new PrintStatement;
	stmt->print_stmt = p;

	match(PRINT);
	Token idTok = match(ID);
	match(SEMICOLON);
	p->id = get_value_node(idTok.lexeme);
	
	return stmt;
}

StatementNode* get_last(StatementNode* list) 
{
	while (list->next != NULL) list = list->next;
	return list;
}

struct StatementNode* parse_while_stmt() 
{
	StatementNode* stmt_node = new StatementNode;
	stmt_node->type = IF_STMT;
	stmt_node->if_stmt = new IfStatement;
	stmt_node->next = make_no_op();

	match(WHILE);
	
	stmt_node->if_stmt->condition_operand1 = parse_primary();
	stmt_node->if_stmt->condition_op = parse_relop();
	stmt_node->if_stmt->condition_operand2 = parse_primary();
	stmt_node->if_stmt->true_branch = parse_body();
	
	StatementNode* goto_node = new StatementNode;
	goto_node->type = GOTO_STMT;
	goto_node->goto_stmt = new GotoStatement;
	goto_node->goto_stmt->target = stmt_node;
	goto_node->next = stmt_node->next;

	//
	stmt_node->if_stmt->false_branch = stmt_node->next;
	get_last(stmt_node->if_stmt->true_branch)->next = goto_node;

	return stmt_node;
}

struct StatementNode* parse_if_stmt() 
{
	StatementNode* stmt_node = new StatementNode;
	stmt_node->type = IF_STMT;
	stmt_node->if_stmt = new IfStatement;
	stmt_node->next = make_no_op();

	match(IF);

	stmt_node->if_stmt->condition_operand1 = parse_primary();
	stmt_node->if_stmt->condition_op = parse_relop();
	stmt_node->if_stmt->condition_operand2 = parse_primary();
	stmt_node->if_stmt->true_branch = parse_body();

	// handle branching 
	stmt_node->if_stmt->false_branch = stmt_node->next; 
	get_last(stmt_node->if_stmt->true_branch)->next = stmt_node->next;
	
	return stmt_node;
}

struct StatementNode* parse_for_stmt()
{


	StatementNode* if_node = new StatementNode;
	if_node->type = IF_STMT;
	if_node->next = make_no_op();

	if_node->if_stmt = new IfStatement;
	if_node->if_stmt->false_branch = if_node->next; // noop 

	match(FOR);
	match(LPAREN);
	// e.g. a = 1; 
	StatementNode* assign1 = parse_assign_stmt();
	// a < 10 
	if_node->if_stmt->condition_operand1 = parse_primary();
	if_node->if_stmt->condition_op = parse_relop();
	if_node->if_stmt->condition_operand2 = parse_primary();
	match(SEMICOLON);
	// a = a + 1; 
	StatementNode* assign2 = parse_assign_stmt();
	match(RPAREN);
	match(LBRACE);
	if_node->if_stmt->true_branch = parse_stmt_list();
	match(RBRACE);

	// GOTO loop 
	assign2->next = new StatementNode;
	assign2->next->type = GOTO_STMT;
	assign2->next->next = if_node->next; // noop 
	assign2->next->goto_stmt = new GotoStatement;
	assign2->next->goto_stmt->target = if_node; // loop

	// app/end assignment + goto node to the end of the if true branch
	get_last(if_node->if_stmt->true_branch)->next = assign2;

	assign1->next = if_node;
	return assign1;
}

struct StatementNode* parse_switch_stmt() 
{
	Token t, varTok;
	ValueNode* var_node;
	StatementNode* begin = make_no_op(); 
	StatementNode* end = make_no_op();

	match(SWITCH);
	varTok = match(ID);
	var_node = get_value_node(varTok.lexeme);
	match(LBRACE);
	// ================- CASE LIST -====================
	//parse_case_list(var_node, end);
	StatementNode* prev = begin; 

	t = peek();
	while (t.token_type == CASE) {
		Token numTok;


		StatementNode* stmt = new StatementNode;
		stmt->type = IF_STMT;
		stmt->if_stmt = new IfStatement;
		stmt->next = make_no_op(); // noop 

		stmt->if_stmt->condition_operand1 = var_node;
		stmt->if_stmt->condition_op = CONDITION_NOTEQUAL;

		match(CASE);
		stmt->if_stmt->condition_operand2 = make_value_node_constant(atoi(match(NUM).lexeme));
		match(COLON);

		stmt->if_stmt->false_branch = parse_body();
		stmt->if_stmt->true_branch = stmt->next; // noop 

		StatementNode* goto_stmt = new StatementNode;
		goto_stmt->type = GOTO_STMT;
		goto_stmt->goto_stmt = new GotoStatement;
		goto_stmt->goto_stmt->target = end;
		goto_stmt->next = stmt->next; // noop 
		get_last(stmt->if_stmt->false_branch)->next = goto_stmt;

		prev->next = stmt;
		prev = stmt->next;
		
		t = peek();
	}

	// ============== DEFAULT ==========================
	t = peek();
	if (t.token_type == DEFAULT) {
		match(DEFAULT);
		match(COLON);
		StatementNode* body = parse_body();
		StatementNode* noop = make_no_op();

		prev->next = body; 
		get_last(body)->next = end; 
		prev = noop; 

		t = peek();
	}
	prev->next = end; 
	// =============== CASE LIST END =================== 
	match(RBRACE);
	return begin->next;
}

struct StatementNode* parse_stmt_list()
{
	StatementNode* st; // statement 
	StatementNode* stl; // statement list 

	Token t1, t2;
	t1 = peek();
	st = new StatementNode;
	if (t1.token_type == ID) {
		st = parse_assign_stmt();
	}	
	else if (t1.token_type == PRINT) {
		st = parse_print_stmt();
	}
	else if (t1.token_type == WHILE) {
		st = parse_while_stmt();
	} 
	else if (t1.token_type == IF) {
		st = parse_if_stmt();
	}
	else if (t1.token_type == SWITCH) {
		st = parse_switch_stmt();
	} 
	else if (t1.token_type == FOR) {
		st = parse_for_stmt();
	} 
	else {
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

	// we append the proceeding statement list to the END of the parsed statement regardless of type of NULL
	get_last(st)->next = stl;
	
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

struct StatementNode* parse_generate_intermediate_representation() {
	return parse_program();
}