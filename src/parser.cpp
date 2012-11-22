/*
 * File: parser.cpp
 * ----------------
 * Implements the parser.h interface.
 */

#include <iostream>
#include <string>
#include "parser.h"
#include "error.h"
#include "strlib.h"
using namespace std;

/*
 * Implementation notes: parseExp
 * ------------------------------
 * This code just reads an expression and then checks for extra tokens.
 */

Expression *parseExp(TokenScanner & scanner) {
   Expression *exp = readE(scanner);
   if (scanner.hasMoreTokens()) {
      error("parseExp: Found extra token: " + scanner.nextToken());
   }
   return exp;
}

/*
 * Implementation notes: readE
 * Usage: exp = readE(scanner, prec);
 * ----------------------------------
 * This version of readE uses precedence to resolve the ambiguity in
 * the grammar.  At each recursive level, the parser reads operators and
 * subexpressions until it finds an operator whose precedence is greater
 * than the prevailing one.  When a higher-precedence operator is found,
 * readE calls itself recursively to read in that subexpression as a unit.
 * 
 * If the use of minus sign (-) as a unary operator is detected, adds
 * 0 in front of the expression to evalutate it as (0-exp).
 */

Expression *readE(TokenScanner & scanner, int prec) {
   string temp = scanner.nextToken();
   scanner.saveToken(temp);
   if (temp == "-") scanner.saveToken("0");
   Expression *exp = readT(scanner);
   string token;
   while (true) {
      token = scanner.nextToken();
      int newPrec = precedence(token);
      if (newPrec <= prec) break;
      Expression *rhs = readE(scanner, newPrec);
      exp = new CompoundExp(token, exp, rhs);
   }
   scanner.saveToken(token);
   return exp;
}

/*
 * Implementation notes: readT
 * ---------------------------
 * This function scans a term, which is either an integer, a identifier,
 * or a parenthesized subexpression.
 */

Expression *readT(TokenScanner & scanner) {
   string token = scanner.nextToken();
   TokenType type = scanner.getTokenType(token);
   if (type == WORD) return new IdentifierExp(token);
   if (type == NUMBER) return new ConstantExp(stringToReal(token));
   if (token != "(") error("Illegal term in expression" + token);
   Expression *exp = readE(scanner);
   if (scanner.nextToken() != ")") {
      error("Unbalanced parentheses in expression");
   }
   return exp;
}

/*
 * Implementation notes: precedence
 * --------------------------------
 * This function checks the token against each of the defined operators
 * and returns the appropriate precedence value.
 */

int precedence(string token) {
   if (token == "+" || token == "-") return 1;
   if (token == "*" || token == "/") return 2;
   return 0;
}

/*
 * Implementation notes: processStatement
 * ------------------------------------------------------
 * Receives a scanner and checks the validity of the first
 * statement occurence. If valid, returns the corresponding 
 * executable statement (as defined in documentation intro).
 * If no first statement exists, but token begins with a valid
 * variable term, then a LET executable statement is returned.
 */
Statement *parseStatement(TokenScanner & scanner){
	string statement = scanner.nextToken();
	Statement *stmt;
	if(statement == "REM" || statement == "rem") {
		stmt = new RemStmt(scanner);
	} else if(statement == "LET" || statement == "let") {
		stmt = new LetStmt(scanner);
	} else if(statement == "PRINT" || statement == "print") {
		stmt = new PrintStmt(scanner);
	} else if(statement == "INPUT" || statement == "input") {
		stmt = new InputStmt(scanner);
	} else if(statement == "GOTO" || statement == "goto") {
		stmt = new GotoStmt(scanner);
	} else if(statement == "IF" || statement == "if") {
		stmt = new IfStmt(scanner);
	} else if(statement == "END" || statement == "end") {
		stmt = new EndStmt(scanner);
	} else if(scanner.getTokenType(statement) == WORD) {
		scanner.saveToken(statement);
		stmt = new LetStmt(scanner);
	} else error("Invald statement: " + statement);
	return stmt;
}