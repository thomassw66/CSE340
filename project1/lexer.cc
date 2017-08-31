/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "IF", "WHILE", "DO", "THEN", "PRINT",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN",
    "NOTEQUAL", "GREATER", "LESS", "LTEQ", "GTEQ",
    "DOT", "NUM", "ID", "ERROR", "REALNUM", "BASE08NUM", "BASE16NUM"
};

#define KEYWORDS_COUNT 5
string keyword[] = { "IF", "WHILE", "DO", "THEN", "PRINT" };


bool LexicalAnalyzer::MatchSequence(string s) 
{
    char c;

    for (int i = 0; i < s.length(); i++) {
        input.GetChar(c);
        if (input.EndOfInput() || c != s[i]) {
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
            for (int j = 0; j < i; j++) {
                input.UngetChar(s[i-j-1]);
            }
            return false;
        }
    }
    return true;
}

bool ispdigit(char c) {
    return c >= '1' && c <= '9';
}

bool ispdigit8(char c) {
    return c >= '1' && c <= '7';
}

bool ispdigit16(char c) {
    return (c >= '1' && c <= '9') || (c >= 'A' && c <= 'F');
}

bool isdigit(char c) {
    return c == '0' || ispdigit(c);
}

bool isdigit8(char c) {
    return c == '0' || ispdigit8(c);
}

bool isdigit16(char c) {
    return c == '0' || ispdigit16(c);
}

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

// Token LexicalAnalyzer::ScanNumber()
// {
//     char c;

//     input.GetChar(c);
//     if (isdigit(c)) {
//         if (c == '0') {
//             tmp.lexeme = "0";
//         } else {
//             tmp.lexeme = "";
//             while (!input.EndOfInput() && isdigit(c)) {
//                 tmp.lexeme += c;
//                 input.GetChar(c);
//             }
//             if (!input.EndOfInput()) {
//                 input.UngetChar(c);
//             }
//         }
//         // TODO: You can check for REALNUM, BASE08NUM and BASE16NUM here!
//         tmp.token_type = NUM;
//         tmp.line_no = line_no;
//         return tmp;
//     } else {
//         if (!input.EndOfInput()) {
//             input.UngetChar(c);
//         }
//         tmp.lexeme = "";
//         tmp.token_type = ERROR;
//         tmp.line_no = line_no;
//         return tmp;
//     }
// }

// Token LexicalAnalyzer::ScanNumber() 
// {
//     char c;

//     input.GetChar(c);
//     if (isdigit(c)) {
//         if (c == '0') {
//             tmp.lexeme = "0";
//             if (!input.EndOfInput()) {
//                 input.GetChar(c);
//                 if (c == '.') { // DOT
//                     tmp.lexeme += '.'; 
//                     input.GetChar(c);
//                     while (!input.EndOfInput() && c == '0') {
//                         tmp.lexeme += c;
//                         input.GetChar(c);
//                     }
//                     if (isdigit(c) && c != '0') {
//                         tmp.lexeme += c;
//                         input.GetChar(c);
//                         while(!input.EndOfInput() && isdigit(c)) {
//                             tmp.lexeme += c;
//                             input.GetChar(c);
//                         }
//                         if (!input.EndOfInput()) {
//                             input.UngetChar(c);
//                         }
//                         tmp.token_type = REALNUM;
//                         tmp.line_no = line_no;
//                         return tmp;
//                     } else {
//                         if (!input.EndOfInput()) {
//                             input.UngetChar(c);
//                         }
//                         for (int i = 0; i < tmp.lexeme.length() - 2; i++) {
//                             input.UngetChar('0');
//                         }
//                         input.UngetChar('.');
//                         tmp.lexeme = "0";
//                         tmp.token_type = NUM;
//                         tmp.line_no = line_no;
//                         return tmp;
//                     }
//                 } else if (MatchSequence("x16")) { // Base 16
//                     tmp.lexeme += "x16";
//                     tmp.token_type = BASE16NUM;
//                     tmp.line_no = line_no;
//                     return tmp;
//                 } else if (MatchSequence("x08")) { // Base 8
//                     tmp.lexeme += "x08";
//                     tmp.token_type = BASE08NUM;
//                     tmp.line_no = line_no;
//                     return tmp;
//                 }
//                 input.UngetChar(c);
//             }
//             tmp.token_type = NUM;
//             tmp.line_no = line_no;
//             return tmp;
//         } else {
//             if (isdigit8(c)) {
//                 while (!input.EndOfInput() && isdigit8(c)) {
//                     tmp.lexeme += c;
//                     input.GetChar(c);
//                 }
//                 if (input.EndOfInput()) {
//                     tmp.token_type = NUM;
//                     tmp.line_no = line_no;
//                     return tmp;
//                 } else {
//                     input.UngetChar(c);
//                 } 
//                 if (MatchSequence("x08")) {
//                     tmp.lexeme += "x08";
//                     tmp.token_type = BASE08NUM;
//                     tmp.line_no = line_no;
//                     return tmp;
//                 } else if (MatchSequence("x16")) {
//                     tmp.lexeme += "x16";
//                     tmp.token_type = BASE16NUM;
//                     tmp.line_no = line_no;
//                     return tmp;
//                 }
//                 input.GetChar(c);            
//             }
//             if (c == '.') {
//                 // DOT 
//                 string real = ".";
//                 input.GetChar(c);
//                 while(!input.EndOfInput() && isdigit(c)) {
//                     real += c;
//                     input.GetChar(c);
//                 }
//                 if (!input.EndOfInput()) {
//                     input.UngetChar(c);
//                 }
//                 if (real.length() > 1) {
//                     tmp.lexeme += real;
//                     tmp.token_type = REALNUM;
//                     tmp.line_no = line_no;
//                     return tmp;
//                 } else {
//                     input.UngetString(real);
//                     tmp.token_type = NUM;
//                     tmp.line_no = line_no;
//                     return tmp;
//                 }
//             }
//             if (isdigit(c)) {
//                 tmp.lexeme += c;
//                 input.GetChar(c);
//                 while (!input.EndOfInput() && isdigit(c)) {
//                     tmp.lexeme += c;
//                     input.GetChar(c);
//                 }
//                 if (!input.EndOfInput()) {
//                     input.UngetChar(c);
//                 }
//                 if (MatchSequence("x16")) {
//                     tmp.lexeme += "x16";
//                     tmp.token_type = BASE16NUM;
//                     tmp.line_no = line_no;
//                     return tmp;
//                 }
//                 input.GetChar(c);
//                 if (c == '.') {
//                     // DOT 
//                     string real = ".";
//                     input.GetChar(c);
//                     while(!input.EndOfInput() && isdigit(c)) {
//                         real += c;
//                         input.GetChar(c);
//                     }
//                     if (!input.EndOfInput()) {
//                         input.UngetChar(c);
//                     }
//                     if (real.length() > 1) {
//                         tmp.lexeme += real;
//                         tmp.token_type = REALNUM;
//                         tmp.line_no = line_no;
//                         return tmp;
//                     } else {
//                         input.UngetString(real);
//                         tmp.token_type = NUM;
//                         tmp.line_no = line_no;
//                         return tmp;
//                     }
//                 }
//             }
//             if (isdigit16(c)) {
//                 tmp.lexeme += c;
//                 input.GetChar(c);
//                 while(!input.EndOfInput() && isdigit16(c)) {
//                     tmp.lexeme += c;
//                     input.GetChar(c);
//                 }
//                 if (!input.EndOfInput()) {
//                     input.UngetChar(c);
//                 }
//                 if (MatchSequence("x16")) {
//                     tmp.lexeme += "x16";
//                     tmp.token_type = BASE16NUM;
//                     tmp.line_no = line_no;
//                     return tmp;
//                 } else {
//                     // tmp.lexeme = "";
//                     tmp.token_type = ERROR;
//                     tmp.line_no = line_no;
//                     return tmp;
//                 }
//             }
//             // if we get here then we have a valid number so far
//             // but we saw a character that doesn't fit into any number type
//             input.UngetChar(c);

//             tmp.token_type = NUM;
//             tmp.line_no = line_no;
//             return tmp;
//         }
//     } else {
//         if (!input.EndOfInput()) {
//             input.UngetChar(c);
//         }
//         tmp.lexeme = "";
//         tmp.token_type = ERROR;
//         tmp.line_no = line_no;
//         return tmp;
//     }

//     tmp.lexeme = "";
//     tmp.token_type = ERROR;
//     tmp.line_no = line_no;
//     return tmp;
// }

Token LexicalAnalyzer::ScanNumber() 
{
    string lex;

    lex = ScanRealNumIfPossible();
    if (lex.length() > 0) {
        tmp.lexeme = lex;
        tmp.token_type = REALNUM;
        tmp.line_no = line_no;
        return tmp;
    }
    lex = ScanBase08NumIfPossible();
    if (lex.length() > 0) {
        tmp.lexeme = lex;
        tmp.token_type = BASE08NUM;
        tmp.line_no = line_no;
        return tmp;
    }
    lex = ScanBase16NumIfPossible();
    if (lex.length() > 0) {
        tmp.lexeme = lex; 
        tmp.token_type = BASE16NUM;
        tmp.line_no = line_no;
        return tmp;
    }
    lex = ScanNumIfPossible();
    if (lex.length() > 0) {
        tmp.lexeme = lex;
        tmp.token_type = NUM;
        tmp.line_no = line_no;
        return tmp;
    }

    tmp.lexeme = "";
    tmp.token_type = ERROR;
    tmp.line_no = line_no;
    return tmp;
}

string LexicalAnalyzer::ScanNumIfPossible()
{
    char c;
    string scanned = "";

    input.GetChar(c);
    scanned += c;
    if (isdigit(c)) {
        if (c == '0') {
            return scanned;
        } else {
            input.GetChar(c);
            while (!input.EndOfInput() && isdigit(c)) {
                scanned += c;
                input.GetChar(c);
            }
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
            return scanned;
        }
    } 
    input.UngetString(scanned);
    return "";
}

string LexicalAnalyzer::ScanRealNumIfPossible() 
{
    char c;
    bool pdigit_scanned_digit_after_dot = false;
    string scanned = "";
    input.GetChar(c);
    scanned += c;
    if (c == '0') {
        if (MatchSequence(".")) {
            scanned += '.';
            input.GetChar(c);
            while(!input.EndOfInput() && c == '0') {
                scanned += c;
                input.GetChar(c);
            }
            if (!input.EndOfInput() && ispdigit(c)) {
                while(!input.EndOfInput() && isdigit(c)) {
                    scanned += c; 
                    input.GetChar(c);
                }
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                return scanned;
            }
        }
    } else if (ispdigit(c)) {
        input.GetChar(c);
        while(!input.EndOfInput() && isdigit(c)) {
            scanned += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        if (MatchSequence(".")) {
            scanned += '.';
            pdigit_scanned_digit_after_dot = false;
            input.GetChar(c);
            while (!input.EndOfInput() && isdigit(c)) {
                pdigit_scanned_digit_after_dot = true;
                scanned += c;
                input.GetChar(c);
            }
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
            if (pdigit_scanned_digit_after_dot) {
                return scanned;
            }
        }
    }
    input.UngetString(scanned);
    return "";
}

// returns the lexeme of the longest base16num that exists or empty string 
// if the next token cannot be a base16num
string LexicalAnalyzer::ScanBase16NumIfPossible()
{
    char c; 
    string scanned = "";
    input.GetChar(c);
    scanned += c;   
    if (c == '0') {
        if (MatchSequence("x16")) {
            scanned += "x16";
            return scanned;
        }
    } else if (ispdigit16(c)) {
        input.GetChar(c);
        while (!input.EndOfInput() && isdigit16(c)) {
            scanned += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        if (MatchSequence("x16")) {
            scanned += "x16";
            return scanned;
        }
    }
    input.UngetString(scanned);
    return "";
}

// Returns the lexeme of the base08num to scan or empty string if 
// there is not a valid base08num
string LexicalAnalyzer::ScanBase08NumIfPossible() 
{
    char c;
    string scanned = "";
    input.GetChar(c);
    scanned += c;
    if (c == '0') {
        if (MatchSequence("x08")) {
            scanned += "x08";
            return scanned; 
        }
    } else if (ispdigit8(c)) {
        input.GetChar(c);
        while(!input.EndOfInput() && isdigit8(c)) {
            scanned += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        if (MatchSequence("x08")) {
            scanned += "x08";
            return scanned;
        }
    }
    input.UngetString(scanned);
    return "";
}


Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
        case '.':
            tmp.token_type = DOT;
            return tmp;
        case '+':
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.token_type = MINUS;
            return tmp;
        case '/':
            tmp.token_type = DIV;
            return tmp;
        case '*':
            tmp.token_type = MULT;
            return tmp;
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '[':
            tmp.token_type = LBRAC;
            return tmp;
        case ']':
            tmp.token_type = RBRAC;
            return tmp;
        case '(':
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.token_type = RPAREN;
            return tmp;
        case '<':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = LTEQ;
            } else if (c == '>') {
                tmp.token_type = NOTEQUAL;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = LESS;
            }
            return tmp;
        case '>':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = GTEQ;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = GREATER;
            }
            return tmp;
        default:
            if (isdigit(c)) {
                input.UngetChar(c);
                return ScanNumber();
            } else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}

int main()
{
    LexicalAnalyzer lexer;
    Token token;

    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }
}
