/*
 * File: statement.cpp
 * -------------------
 * Implements the statement.h interface.
 */

#include <string>
#include "statement.h"
#include "parser.h"
#include "graphics.h"
using namespace std;

/* Implementation of the Statement class */

/* Static constants */
static const double COLUMN_WIDTH = 400;				//WINDOW_WIDTH/2
static const double COLUMN_HEIGHT =	220;			//WINDOW_HEIGHT - 80
static const string BG_FILE = "bg.jpg";
static const double INIT_HEIGHT = 55;
static const double PRINT_HEIGHT = COLUMN_HEIGHT + 25;


/* Global variables keeping track of graphics order for all elements */
int orderB = 0;			// Y-coordinate for Before Execution column
int orderA = 0;			// Y-coordinate for After Execution column

Statement::Statement() {
	setColor("#fbcc62");
}

Statement::~Statement() {
   /* Empty */
}

/*
 * Method: PrintStmt
 * Usage: Statement *stmt = new PrintStmt(scanner);
 * -------------------------------------------------
 * Checks for extraneous tokens, and creates a PrintStmt
 * object that stores the parsed representation of the 
 * expression in an instance variable.
 *
 * Optionally accepts a list of expressions separated by a 
 * comma and stores PrintStmt objects for each one in a vector.
 * Also optionally accepts an optional beginning with a string 
 * bound by "".
 */
PrintStmt::PrintStmt(TokenScanner & scanner) {
	string test = scanner.nextToken();
	handleGraphicsB();
	testInitComment(scanner, test);
	addRest(scanner);
	if (scanner.hasMoreTokens()) {
		error("Extraneous token " + scanner.nextToken());
	}
}

/*
 * Method: ~PrintStmt()
 * ---------------------
 * Destructor for PrintStmt subclass.
 */
PrintStmt::~PrintStmt()	{
}

/*
 * Method: execute
 * Usage: program.getParsedStatement(index)->execute(state);
 * ----------------------------------------------------------
 * Iterates through vec and reads all stored expressions, 
 * evaluates each expression in the context of the corresponding 
 * EvalState object and sends the result to cout.
 */
void PrintStmt::execute(EvalState & state) {
	cout << comment;
	handleGraphicsA();
	if (comment != "") {
		drawString("Printed: " + comment, 
			getWindowWidth()/2 + 20, orderA);
	}
	printExps(state);
	cout << endl;
	state.setNextLine("NULL");
}

/*
 * Method: testInitComment
 * Usage: testInitComment(scanner, test);
 * -------------------------------------------------
 * Tests if input begins with an initial string, and updates
 * 'comment' accordingly for printing. If no initial string is
 * detected, then the first expression is added to vec.
 */
void PrintStmt::testInitComment(TokenScanner & scanner, string test){
    if(test == "\"") {
		string temp = scanner.nextToken();
		while(temp != "\""){
			comment += temp + " ";
			temp = scanner.nextToken();
		}
	} else {
		comment = "";
		scanner.saveToken(test);
		Expression *exp = readE(scanner);
		vec.add(exp);
		drawString("To be printed: " + exp->toString(), 20, orderB);
	}
}

/*
 * Method: addRest
 * Usage: addRest(scanner);
 * -------------------------------------------------
 * Using commas (,) as tokenizers, creates and adds
 * a list of expressions in the input to vec.
 */
void PrintStmt::addRest(TokenScanner & scanner){
	while(scanner.nextToken() == ","){
		Expression *exp = readE(scanner);
		vec.add(exp);
		orderB += 15;
		drawString("To be printed: " + exp->toString(), 20, orderB);
	}
}

/*
 * Method: printExps
 * Usage: printExps();
 * ---------------------------------------------------------
 * Reads all expressions stored in ve and prints out their 
 * evaluated states to the console. Also updates graphics window.
 */
void PrintStmt::printExps(EvalState & state){
	foreach(Expression * exp in vec){
		double result = exp->eval(state);
		cout << realToString(result) << " ";
		drawString("Printed: " + realToString(result), 
						getWindowWidth()/2 + 20, orderA);
		orderA += 15;
	}
}

/*
 * Method: handleGraphicsB
 * Usage: handleGraphicsB();
 * ------------------------------------------------------------
 * Handles initialization of printing points in for 'Before
 * Execution' column in graphics window. 
 */
void PrintStmt::handleGraphicsB(){
	if (orderB > PRINT_HEIGHT) {
		drawImage(BG_FILE, 0, 45, COLUMN_WIDTH, COLUMN_HEIGHT);
		orderB = 0;
	}
	if (orderB == 0) orderB = INIT_HEIGHT;
	orderB += 15;
}


/*
 * Method: handleGraphicsA
 * Usage: handleGraphicsA();
 * ------------------------------------------------------------
 * Handles initialization of printing points in for 'After
 * Execution' column in graphics window. 
 */
void PrintStmt::handleGraphicsA(){
	if (orderA > PRINT_HEIGHT) {
		drawImage(BG_FILE, getWindowWidth()/2 + 10,45, COLUMN_WIDTH, COLUMN_HEIGHT);
		orderA = 0;
	}
	if (orderA == 0) orderA = INIT_HEIGHT;
	orderA += 15;
}

/*
 * Method: RemStmt
 * Usage: Statement *stmt = new RemStmt(scanner);
 * -------------------------------------------------
 * Creates a blank RemStmt object.
 */
RemStmt::RemStmt(TokenScanner & scanner) {
	while(scanner.hasMoreTokens()){
		str += scanner.nextToken() + " ";
	}
	handleGraphicsB();
	drawString("Comment: " + str, 20, orderB);
}

/*
 * Method: ~RemStmt()
 * ---------------------
 * Destructor for RemStmt subclass.
 */
RemStmt::~RemStmt()	{
}

/*
 * Method: execute
 * Usage: program.getParsedStatement(index)->execute(state);
 * ----------------------------------------------------------
 * Ensures execution order is restored after IF and GOTO 
 * disruptions have been handled.
 */
void RemStmt::execute(EvalState & state) {
	state.setNextLine("NULL");
	handleGraphicsA();
	drawString("Skipped comment: " + str, 
			getWindowWidth()/2 + 20, orderA);
}

/*
 * Method: handleGraphicsB
 * Usage: handleGraphicsB();
 * ------------------------------------------------------------
 * Handles initialization of printing points in for 'Before
 * Execution' column in graphics window. 
 */
void RemStmt::handleGraphicsB(){
	if (orderB > PRINT_HEIGHT) {
		drawImage(BG_FILE, 0, 45, COLUMN_WIDTH, COLUMN_HEIGHT);
		orderB = 0;
	}
	if (orderB == 0) orderB = INIT_HEIGHT;
	orderB += 15;
}

/*
 * Method: handleGraphicsA
 * Usage: handleGraphicsA();
 * ------------------------------------------------------------
 * Handles initialization of printing points in for 'After
 * Execution' column in graphics window. 
 */
void RemStmt::handleGraphicsA(){
	if (orderA > PRINT_HEIGHT) {
		drawImage(BG_FILE, getWindowWidth()/2 + 10,45, COLUMN_WIDTH, COLUMN_HEIGHT);
		orderA = 0;
	}
	if (orderA == 0) orderA = INIT_HEIGHT;
	orderA += 15;
}

/*
 * Method: InputStmt
 * Usage: Statement *stmt = new InputStmt(scanner);
 * -------------------------------------------------
 * Ensures first token after statement is a valid lvalue.
 * Checks for extraneous tokens, and creates an InputStmt
 * object that stores the lvalue in an instance variable.
 */
InputStmt::InputStmt(TokenScanner & scanner) {
	var = scanner.nextToken();
	if(scanner.getTokenType(var) != WORD) error("Only letters allowed.");
	if (scanner.hasMoreTokens()) {
		error("Extraneous token " + scanner.nextToken());
	}
	handleGraphicsB();
	drawString("Variable stored: " + var + " = ?", 20, orderB);
}

/*
 * Method: ~InputStmt()
 * ---------------------
 * Destructor for InputStmt subclass.
 */
InputStmt::~InputStmt()	{
}

/*
 * Method: execute
 * Usage: program.getParsedStatement(index)->execute(state);
 * ----------------------------------------------------------
 * Asks the user to key in an integer value, and sets the stored
 * lvalue equal to the input.
 */
void InputStmt::execute(EvalState & state) {
	handleGraphicsA();
	drawString("Requested input for: " + var, 
				getWindowWidth()/2 + 20, orderA);
	double val = getReal(var + " ? ");
	state.setValue(var, val);
	state.setNextLine("NULL");
	handleGraphicsA();
	drawString("Value updated: " + var + " = " + realToString(val),
		getWindowWidth()/2 + 20, orderA);
}

/*
 * Method: handleGraphicsB
 * Usage: handleGraphicsB();
 * ------------------------------------------------------------
 * Handles initialization of printing points in for 'Before
 * Execution' column in graphics window. 
 */
void InputStmt::handleGraphicsB(){
	if (orderB > PRINT_HEIGHT) {
		drawImage(BG_FILE, 0, 45, COLUMN_WIDTH, COLUMN_HEIGHT);
		orderB = 0;
	}
	if (orderB == 0) orderB = INIT_HEIGHT;
	orderB += 15;
}

/*
 * Method: handleGraphicsA
 * Usage: handleGraphicsA();
 * ------------------------------------------------------------
 * Handles initialization of printing points in for 'After
 * Execution' column in graphics window. 
 */
void InputStmt::handleGraphicsA(){
	if (orderA > PRINT_HEIGHT) {
		drawImage(BG_FILE, getWindowWidth()/2 + 10,45, COLUMN_WIDTH, COLUMN_HEIGHT);
		orderA = 0;
	}
	if (orderA == 0) orderA = INIT_HEIGHT;
	orderA += 15;
}

/*
 * Method: LetStmt
 * Usage: Statement *stmt = new LetStmt(scanner);
 * -------------------------------------------------
 * Ensures the valid presence of an assignment operator.
 * Checks for extraneous tokens, and creates a LetStmt
 * object that stores the lvalue and the expression following
 * the operator in instance variables.
 */
LetStmt::LetStmt(TokenScanner & scanner) {
	var = scanner.nextToken();
	string op = scanner.nextToken();
	if (op != "=") error("Illegal operator: " + op);
	exp = readE(scanner);
	if (scanner.hasMoreTokens()) {
		error("Extraneous token " + scanner.nextToken());
	}
	handleGraphicsB();
	drawString("Variable stored: " + var + " = " + 
							exp->toString(), 20, orderB);
}

/*
 * Method: ~LetStmt()
 * ---------------------
 * Destructor for LetStmt subclass.
 */
LetStmt::~LetStmt()	{
	delete exp;
}

/*
 * Method: execute
 * Usage: program.getParsedStatement(index)->execute(state);
 * ----------------------------------------------------------
 * Evaluates the stored expression and assigns it to the stored lvalue.
 */
void LetStmt::execute(EvalState & state) {
	double val = exp->eval(state);
	state.setValue(var, val);
	state.setNextLine("NULL");
	handleGraphicsA();
	drawString("Value updated: " + var + " = " + 
		realToString(val), getWindowWidth()/2 + 20, orderA);
}

/*
 * Method: handleGraphicsB
 * Usage: handleGraphicsB();
 * ------------------------------------------------------------
 * Handles initialization of printing points in for 'Before
 * Execution' column in graphics window. 
 */
void LetStmt::handleGraphicsB(){
	if (orderB > PRINT_HEIGHT) {
		drawImage(BG_FILE, 0, 45, COLUMN_WIDTH, COLUMN_HEIGHT);
		orderB = 0;
	}
	if (orderB == 0) orderB = INIT_HEIGHT;
	orderB += 15;
}

/*
 * Method: handleGraphicsA
 * Usage: handleGraphicsA();
 * ------------------------------------------------------------
 * Handles initialization of printing points in for 'After
 * Execution' column in graphics window. 
 */
void LetStmt::handleGraphicsA(){
	if (orderA > PRINT_HEIGHT) {
		drawImage(BG_FILE, getWindowWidth()/2 + 10,45, COLUMN_WIDTH, COLUMN_HEIGHT);
		orderA = 0;
	}
	if (orderA == 0) orderA = INIT_HEIGHT;
	orderA += 15;
}

/*
 * Method: Goto
 * Usage: Statement *stmt = new GotoStmt(scanner);
 * -------------------------------------------------
 * Checks for extraneous tokens, and creates an InputStmt
 * object that stores the number following the statement in 
 * an instance variable.
 */
GotoStmt::GotoStmt(TokenScanner & scanner) {
	next = scanner.nextToken();
	if (scanner.hasMoreTokens()) {
		error("Extraneous token " + scanner.nextToken());
	}
	handleGraphicsB();
	drawString("Will skip to line " + next + 
		" during execution.", 20, orderB);
}

/*
 * Method: ~GotoStmt()
 * ---------------------
 * Destructor for GotoStmt subclass.
 */
GotoStmt::~GotoStmt()	{
}

/*
 * Method: execute
 * Usage: program.getParsedStatement(index)->execute(state);
 * ----------------------------------------------------------
 * Forces program to execute the stored line number, instead of 
 * what is in its usual order.
 */
void GotoStmt::execute(EvalState & state) {
	state.setNextLine(next);
	handleGraphicsA();
	drawString("Skipped to line: " + next, 
		getWindowWidth()/2 + 20, orderA);
}

/*
 * Method: handleGraphicsB
 * Usage: handleGraphicsB();
 * ------------------------------------------------------------
 * Handles initialization of printing points in for 'Before
 * Execution' column in graphics window. 
 */
void GotoStmt::handleGraphicsB(){
	if (orderB > PRINT_HEIGHT) {
		drawImage(BG_FILE, 0, 45, COLUMN_WIDTH, COLUMN_HEIGHT);
		orderB = 0;
	}
	if (orderB == 0) orderB = INIT_HEIGHT;
	orderB += 15;
}

/*
 * Method: handleGraphicsA
 * Usage: handleGraphicsA();
 * ------------------------------------------------------------
 * Handles initialization of printing points in for 'After
 * Execution' column in graphics window. 
 */
void GotoStmt::handleGraphicsA(){
	if (orderA > PRINT_HEIGHT) {
		drawImage(BG_FILE, getWindowWidth()/2 + 10,45, COLUMN_WIDTH, COLUMN_HEIGHT);
		orderA = 0;
	}
	if (orderA == 0) orderA = INIT_HEIGHT;
	orderA += 15;
}

/*
 * Method: IfStmt
 * Usage: Statement *stmt = new IfStmt(scanner);
 * -------------------------------------------------
 * Ensures the validity of statement syntax, and checks for 
 * extraneous tokens. Creates an IfStmt object that stores 
 * the operator and the expressions on either side of the
 * operator in instance variable.
 */
IfStmt::IfStmt(TokenScanner & scanner) {
	storeExp(scanner);
	if (scanner.hasMoreTokens()) {
		error("Extraneous token " + scanner.nextToken());
	}
	handleGraphicsB();
	drawString("Condition stored: " + expL->toString() + " " 
					+ op + " " + expR->toString(), 20, orderB);
}

/*
 * Method: ~IfStmt()
 * ---------------------
 * Destructor for IfStmt subclass.
 */
IfStmt::~IfStmt()	{
	delete expL;
	delete expR;
}

/*
 * Method: execute
 * Usage: program.getParsedStatement(index)->execute(state);
 * ----------------------------------------------------------
 * Compares the two stored expressions according to operator (=, > or <).
 * If condition holds, forces program to execute the stored line number
 * next, instead of what is in its usual order.  Else program executes 
 * in normal order.
 */
void IfStmt::execute(EvalState & state) {
	bool result = processCondition(state);
	handleGraphicsA();
	displayResult(result, state);
}

/*
 * Method: storeExp
 * Usage:storeExp(scanner);
 * -------------------------------------------------
 * Receives a scanner and parses the expression to
 * the condition components and redirection command. 
 */
void IfStmt::storeExp(TokenScanner & scanner){
	expL = readE(scanner);
	op = scanner.nextToken();
	expR = readE(scanner);
	string then = scanner.nextToken();
	if(then != "THEN" && then != "then") error("Incorrect command format.");
	next = scanner.nextToken();
	if(scanner.getTokenType(next) != NUMBER){
		error("Condition result needs to be an integer line number");
	}
}

/*
 * Method: processCondition
 * Usage: processCondition();
 * ----------------------------------------------------------
 * Compares the stored expressions in accordance with input operator,
 * and returns of the condition holds or not.
 */
bool IfStmt::processCondition(EvalState & state){
	if(op == "=") {
		if (expL->eval(state) == expR->eval(state)) return true;
	}
	if(op == ">") {
		if (expL->eval(state) > expR->eval(state)) return true;
	}
	if(op == "<") {
		if (expL->eval(state) < expR->eval(state)) return true;
	}
	return false;
}

/*
 * Method: displayResult
 * Usage: void displayResult(bool result);
 * -------------------------------------------------
 * Receives a boolean and accordingly prints the result
 * on the graphics window and reroutes program execution.
 */
void IfStmt::displayResult(bool result, EvalState & state){
	if(result) {
		state.setNextLine(next);
		drawString("Condition " + expL->toString() + " " + op 
			+ " " + expR->toString() + " is TRUE. Skippin to line " 
			+ next,  getWindowWidth()/2 + 20, orderA);
	} else {
		state.setNextLine("NULL");
		drawString("Condition " + expL->toString() + " " + op 
			+ " " + expR->toString() + " is FALSE. Execution "
			+ "order remains.", getWindowWidth()/2 + 20, orderA);
	}
}

/*
 * Method: handleGraphicsB
 * Usage: handleGraphicsB();
 * ------------------------------------------------------------
 * Handles initialization of printing points in for 'Before
 * Execution' column in graphics window. 
 */
void IfStmt::handleGraphicsB(){
	if (orderB > PRINT_HEIGHT) {
		drawImage(BG_FILE, 0, 45, COLUMN_WIDTH, COLUMN_HEIGHT);
		orderB = 0;
	}
	if (orderB == 0) orderB = INIT_HEIGHT;
	orderB += 15;
}

/*
 * Method: handleGraphicsA
 * Usage: handleGraphicsA();
 * ------------------------------------------------------------
 * Handles initialization of printing points in for 'After
 * Execution' column in graphics window. 
 */
void IfStmt::handleGraphicsA(){
	if (orderA > PRINT_HEIGHT) {
		drawImage(BG_FILE, getWindowWidth()/2 + 10,45, COLUMN_WIDTH, COLUMN_HEIGHT);
		orderA = 0;
	}
	if (orderA == 0) orderA = INIT_HEIGHT;
	orderA += 15;
}

/*
 * Method: End
 * Usage: Statement *stmt = new EndStmt(scanner);
 * -------------------------------------------------
 * Checks for extraneous tokens, and creates a blank EndStmt object.
 */
EndStmt::EndStmt(TokenScanner & scanner) {
	cout << endl;
	if (scanner.hasMoreTokens()) {
		error("Extraneous token " + scanner.nextToken());
	}
	handleGraphicsB();
	drawString("Program will halt at this point.", 20, orderB);
}

/*
 * Method: ~EndStmt()
 * ---------------------
 * Destructor for EndStmt subclass.
 */
EndStmt::~EndStmt()	{
}

/*
 * Method: execute
 * Usage: program.getParsedStatement(index)->execute(state);
 * ----------------------------------------------------------
 * Halts program execution.
 */
void EndStmt::execute(EvalState & state) {
	state.setNextLine("-1");
	handleGraphicsA();
	drawString("Program halted.", getWindowWidth()/2 + 20, orderA);
}

/*
 * Method: handleGraphicsB
 * Usage: handleGraphicsB();
 * ------------------------------------------------------------
 * Handles initialization of printing points in for 'Before
 * Execution' column in graphics window. 
 */
void EndStmt::handleGraphicsB(){
	if (orderB > PRINT_HEIGHT) {
		drawImage(BG_FILE, 0, 45, COLUMN_WIDTH, COLUMN_HEIGHT);
		orderB = 0;
	}
	if (orderB == 0) orderB = INIT_HEIGHT;
	orderB += 15;
}

/*
 * Method: handleGraphicsA
 * Usage: handleGraphicsA();
 * ------------------------------------------------------------
 * Handles initialization of printing points in for 'After
 * Execution' column in graphics window. 
 */
void EndStmt::handleGraphicsA(){
	if (orderA > PRINT_HEIGHT) {
		drawImage(BG_FILE, getWindowWidth()/2 + 10,45, COLUMN_WIDTH, COLUMN_HEIGHT);
		orderA = 0;
	}
	if (orderA == 0) orderA = INIT_HEIGHT;
	orderA += 15;
}