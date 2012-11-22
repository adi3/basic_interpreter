/*
 * File: statement.h
 * -----------------
 * This file defines the Statement abstract type.  In
 * the finished version, this file will also specify subclasses
 * for each of the statement types.  As you design your own
 * version of this class, you should pay careful attention to
 * the exp.h interface specified in Chapter 17, which is an
 * excellent model for the Statement class hierarchy.
 */

#ifndef _statement_h
#define _statement_h

#include "exp.h"
#include "evalstate.h"
#include "tokenscanner.h"
#include "simpio.h"
#include "strlib.h"
#include "vector.h"

/*
 * Class: Statement
 * ----------------
 * This class is used to represent a statement in a program.
 * The model for this class is Expression in the exp.h interface.
 * Like Expression, Statement is an abstract class with subclasses
 * for each of the statement and command types required for the
 * BASIC interpreter.
 */

class Statement {

public:

/*
 * Constructor: Statement
 * ----------------------
 * The base class constructor is empty.  Each subclass must provide
 * its own constructor.
 */

   Statement();

/*
 * Destructor: ~Statement
 * Usage: delete stmt;
 * -------------------
 * The destructor deallocates the storage for this expression.
 * It must be declared virtual to ensure that the correct subclass
 * destructor is called when deleting a statement.
 */

   virtual ~Statement();

/*
 * Method: execute
 * Usage: stmt->execute(state);
 * ----------------------------
 * This method executes a BASIC statement.  Each of the subclasses
 * defines its own execute method that implements the necessary
 * operations.  As was true for the expression evaluator, this
 * method takes an EvalState object for looking up variables or
 * controlling the operation of the interpreter.
 */

   virtual void execute(EvalState & state) = 0;

};

/*
 * Class: PrintStmt
 * ----------------------------
 * Represents a PRINT statement. Prepares a corresponding executable 
 * from input. Optionally accepts a list of expressions separated by 
 * a comma and stores PrintStmt objects for each one in a vector. 
 * Reads all stored expressions from vec, evaluates each expression
 * in the context of  the corresponding EvalState object and sends 
 * the result to cout.
 * Also optionally accepts an optional beginning with a string 
 * bound by "".
 */
class PrintStmt: public Statement {
	public:
		PrintStmt(TokenScanner & scanner);
		virtual ~PrintStmt();
		virtual void execute(EvalState & state);
	private:
		Vector<Expression *> vec;
		string comment;
		void testInitComment(TokenScanner & scanner, string test);
		void addRest(TokenScanner & scanner);
		void printExps(EvalState & state);
		void handleGraphicsB();
		void handleGraphicsA();
};

/*
 * Class: RemStmt
 * ----------------------------
 * Represents a REM statement. Executable serves no purpose apart from
 * ensuring execution order is restored after IF and GOTO disruptions
 * have been handled.
 */
class RemStmt: public Statement {
	public:
		RemStmt(TokenScanner & scanner);
		virtual ~RemStmt();
		virtual void execute(EvalState & state);
	private:
		string str;
		void handleGraphicsB();
		void handleGraphicsA();
};

/*
 * Class: InputStmt
 * ----------------------------
 * Represents an INPUT statement. Prepares a corresponding executable 
 * that stores the variable whose value will be read in from the user.
 * During execution, asks the user to key in an integer value.
 */
class InputStmt: public Statement {
	public:
		InputStmt(TokenScanner & scanner);
		virtual ~InputStmt();
		virtual void execute(EvalState & state);
	private:
		string var;
		void handleGraphicsB();
		void handleGraphicsA();
};

/*
 * Class: LetStmt
 * ----------------------------
 * Represents a LET statement. Prepares a corresponding executable 
 * that stores the given variable and expression. During execution,
 * evaluates the expression and assigns it to the variable. Stores
 * the pair.
 */
class LetStmt: public Statement {
	public:
		LetStmt(TokenScanner & scanner);
		virtual ~LetStmt();
		virtual void execute(EvalState & state);
	private:
		string var;
		Expression *exp;
		void handleGraphicsB();
		void handleGraphicsA();
};

/*
 * Class: GotoStmt
 * ----------------------------
 * Represents a GOTO statement. Prepares a corresponding executable 
 * that forces program to execute the stored line number, instead of 
 * what is in its usual order. Execution order is restored thereafter.
 */
class GotoStmt: public Statement {
	public:
		GotoStmt(TokenScanner & scanner);
		virtual ~GotoStmt();
		virtual void execute(EvalState & state);
	private:
		string next;
		void handleGraphicsB();
		void handleGraphicsA();
};

/*
 * Class: IfStmt
 * ----------------------------
 * Represents an IF statement. Prepares a corresponding executable 
 * that forces program to execute the given line number next, instead
 * of what is in its usual order, if the given condition holds. Else
 * program executes in normal order.
 */
class IfStmt: public Statement {
	public:
		IfStmt(TokenScanner & scanner);
		virtual ~IfStmt();
		virtual void execute(EvalState & state);
	private:
		Expression *expL;
		Expression *expR;
		string op;
		string next;
		void storeExp(TokenScanner & scanner);
		bool processCondition(EvalState & state);
		void displayResult(bool result, EvalState & state);
		void handleGraphicsB();
		void handleGraphicsA();
};

/*
 * Class: EndStmt
 * ----------------------------
 * Represents an END statement. Prepares a corresponding executable 
 * that halts program execution.
 */
class EndStmt: public Statement {
	public:
		EndStmt(TokenScanner & scanner);
		virtual ~EndStmt();
		virtual void execute(EvalState & state);
	private:
		void handleGraphicsB();
		void handleGraphicsA();
};

#endif
