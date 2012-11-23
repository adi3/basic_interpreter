/*
 * File: Basic.cpp
 * ---------------
 * By: Adi
 * 
 * This program builds an awesome BASIC interpreter and 
 * allows a user to code, save and run programs written in BASIC.
 *
 * --------------------------------------------------------
 * The following commands are accepted by the interpreter:
 *
 * SAVE - Saves the current program to a text file
 * OLD - Loads a previous program from a text file
 * RUN - Runs the stored program
 * DEBUG - Runs the stored program line by line
 * LIST - Lists the stored program (w optional limits)
 * CLEAR - Deletes the stored program
 * HELP - Displays help information
 * QUIT - Exits the interpreter
 *
 * ----------------------------------------------------------------------
 * In addition, the following statements are accepted by the interpreter:
 *
 * REM - [Usage: REM exp]: Used for comments.
 * LET - [Usage: LET var = exp]: Assignment statement. Assigns the value 
 * of the expression to the variable, replacing any previous value.
 * PRINT - [Usage: PRINT exp]: Prints value of the expression to the console.
 * INPUT - [Usage: INPUT var]: Reads in a variable from the user. Prompts
 * the user by printing " ? ", and assigns the input value to the variable.
 * GOTO - [Usage: GOTO n]: Forces program to execute line n instead of the 
 * next stored line.
 * IF - [Usage: IF exp1 op exp2 THEN n]: Conditional operator op accepts =, 
 * <, and > to compare exp1 and exp2. If condition holds, executes line n.
 * If not, program executes the next stored line.
 * END - [Usage: END]: Halts program execution.
 *
 * -------------------------------------------------------------------
 * == FEATURES ==
 * The program comes packaged with the following features:
 *
 * - All commands and statements are case-insensitive.
 * - The minus sign (-) can be used both as a unary and a binary operator.
 * - LET statement is optional. Assignment works either way.
 * - PRINT statement accepts an optional beginning with a string bound by "".
 * - PRINT statement accepts a list of values/experessions/ variables 
 * separated by a comma.
 * - Program works with floating-point numbers too.
 * - LIST command accepts an optional range for listing only a part of the 
 * program (eg, LIST 50-80).
 * - Capability to save to and load from text files.
 * - A graphical debugger that displays program state (all variables,
 * expressions, conditions, current line number, etc) both before and after
 * program execution.
 * - CLEAR command resets the graphical debugger to original state, apart
 * from clearing stored program.
 * - A debug mode that allows users to run through the program line by line.
 * - A print() function in program.cpp to show program structure in console.
 * - Typing in an already existing line number with a blank expression
 * removes that line from the program.
 *
 */

#include <cctype>
#include <iostream>
#include <string>
#include <fstream>

#include "parser.h"
#include "program.h"
#include "statement.h"

#include "graphics.h"
#include "console.h"
#include "tokenscanner.h"
#include "simpio.h"
#include "strlib.h"
#include "filelib.h"
using namespace std;

// Declared to avoid enum conflicts with tokenscanner.h
namespace Win32{

	// Tells program to ignore winsock.h
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif

	#include <windows.h>
}

/* Static constants */
static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 300;
static const string BG_FILE = "bg.jpg";

/* Function prototypes */

void genGraphics();
void saveFile(Program & program);
void loadFile(Program & program, EvalState & state);
void processLine(string line, Program & program, EvalState & state);
void processCode(int lineNum, string line, 
				 TokenScanner & scanner, Program & program);
void processCommand(TokenScanner & scanner, 
					Program & program, EvalState & state);
void findListLimits(TokenScanner & scanner, int & start, int & end);
void listProgram(Program & program, int index, int end);
void clearGraphics();
void run(Program & program, EvalState & state);
void debug(Program & program, EvalState & state);
void reloadCurrentLineGraphics();
void printHelpMsg();
void printCmds();
void printStmts();
void printFeatures();


/* Main program */
int main() {
   string title = "BASIC Interpreter | Win32";
   Win32::SetConsoleTitle(title.c_str());

   EvalState state;
   Program program;
   genGraphics();
   cout << "An Awesome BASIC Interpreter! -- Type HELP for help" << endl << endl;
   while (true) {
      try {
         processLine(getLine("=> "), program, state);
      } catch (ErrorException & ex) {
          cerr << "Error: " << ex.getMessage() << endl;
      }
   }
   return 0;
}

/*
 * Function: genGraphics
 * Usage: genGraphics();
 * ---------------------------------------------------
 * Initializes the graphical window used for displaying 
 * program state and debugging.
 */
void genGraphics() {
   initGraphics(WINDOW_WIDTH, WINDOW_HEIGHT);
   setWindowTitle("BASIC Interpreter | Debugger");
   drawImage(BG_FILE, 0, 0);

   setColor("#fbcc62");
   drawLine(0, 20, WINDOW_WIDTH, 20);
   drawLine(WINDOW_WIDTH/4 - getStringWidth("BEFORE EXECUTION")/2, 40,
			WINDOW_WIDTH/4 + getStringWidth("BEFORE EXECUTION")/2, 40);
   drawLine(3*WINDOW_WIDTH/4 - getStringWidth("AFTER EXECUTION")/2, 40,
			3*WINDOW_WIDTH/4 + getStringWidth("AFTER EXECUTION")/2, 40);
   drawLine(WINDOW_WIDTH/2, 25, WINDOW_WIDTH/2, WINDOW_HEIGHT-25);
   drawLine(0, WINDOW_HEIGHT-20, WINDOW_WIDTH, WINDOW_HEIGHT-20);
   drawString("PROGRAM STATE", (WINDOW_WIDTH - 
				getStringWidth("PROGRAM STATE"))/2, 15);
   drawString("BEFORE EXECUTION", WINDOW_WIDTH/4 - 
				getStringWidth("BEFORE EXECUTION")/2, 38);
   drawString("AFTER EXECUTION", 3*WINDOW_WIDTH/4 - 
				getStringWidth("AFTER EXECUTION")/2, 38);
   drawString("Current Line: ", 5, WINDOW_HEIGHT-5);
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Scans a single line entered by the user to deduce if it 
 * is a command or a line of code. Processes input accordingly.  
 */
void processLine(string line, Program & program, EvalState & state) {
   TokenScanner scanner;
   scanner.ignoreWhitespace();
   scanner.scanNumbers();
   scanner.setInput(line);
   string firstTerm = scanner.nextToken();
   if (scanner.getTokenType(firstTerm) == NUMBER) {
	   int lineNum = stringToInteger(firstTerm);
	   processCode(lineNum, line, scanner, program);
   } else {
	   scanner.saveToken(firstTerm);
	   processCommand(scanner, program, state);
   }
}

/*
 * Function: processCode
 * Usage: processCode(lineNum, line, scanner, program);
 * ------------------------------------------------------
 * Receives a single line of code, and stores it in both a 
 * string and a parsed executable form. Storage is according 
 * to number at the beginning of the line.
 * If line only has a number, and if some code is already
 * stored corresponding to that number, the corresponding code 
 * is deleted from storage.
 */
void processCode(int lineNum, string line, 
				 TokenScanner & scanner, Program & program){
	if (scanner.hasMoreTokens()){
	  program.addSourceLine(lineNum, line);
	  Statement *stmt = parseStatement(scanner);
	  program.setParsedStatement(lineNum, stmt);
	} else {
	  program.removeSourceLine(lineNum);
	}
}

/*
 * Function: processCommand
 * Usage: processCommand(firstTerm, program, state);
 * ------------------------------------------------------
 * Receives a single command and checks its validity.
 * If valid, carries out the corresponding operation (as 
 * defined in documentation intro).
 */
void processCommand(TokenScanner & scanner, 
					Program & program, EvalState & state){
   string firstTerm = toUpperCase(scanner.nextToken());
   if(firstTerm == "SAVE") {
	   saveFile(program);
   } else if(firstTerm == "OLD") {
	   loadFile(program, state);
   } else if(firstTerm == "RUN") {
	   run(program, state);
   } else if(firstTerm == "DEBUG") {
	  debug(program, state);
   } else if(firstTerm == "LIST") {
	   int start, end;
	   findListLimits(scanner, start, end);
	   listProgram(program, start, end);
   } else if(firstTerm == "CLEAR") {
	   clearGraphics();
	   program.clear();
   } else if(firstTerm == "HELP") {
	   printHelpMsg();
   } else if(firstTerm == "QUIT") {
	   exit(0);
   } else {
	   error("Invalid beginning: " + firstTerm + ". Type HELP for help.");
   }
}

/*
 * Function: saveFile
 * Usage: saveFile(program);
 * --------------------------------------------------------
 * Prompts user to enter file name. Receives a program and 
 * saves its contents to that file.
 */
void saveFile(Program & program){
	ofstream outfile;
	promptUserForFile(outfile, "Choose filename (w extension .txt): ");
	int index = program.getFirstLineNumber();
	while (index != -1){
		outfile << program.getSourceLine(index) << endl;
		index = program.getNextLineNumber(index);
	}
	cout << "Program saved." << endl;
	outfile.close();
}

/*
 * Function: loadFile
 * Usage: loadFile(program, state);
 * --------------------------------------------------
 * Prompts user for the file name containing code. Loads 
 * its contents and stores them in the Program object of 
 * the current execution.
 */
void loadFile(Program & program, EvalState & state){
   ifstream infile;
   promptUserForFile(infile, "Enter filename containing code: ");
   clearGraphics();
   while(!infile.eof()){
	   string str;
	   getline (infile, str);
	   if (str != "") processLine(str, program, state);
   }
   cout << "Program loaded -- Type LIST to view." << endl;
   infile.close();
}

/*
 * Function: findListLimits
 * Usage:  findListLimits(scanner, start, end);
 * ------------------------------------------------
 * Finds and returns start and end limits for listing stored 
 * program. If user has not given any limits, then start and 
 * end are desginated as NULL, and the entire program is listed.
 */
void findListLimits(TokenScanner & scanner, int & start, int & end){
   start = NULL;
   end = NULL;
   if(scanner.hasMoreTokens()) start = stringToInteger(scanner.nextToken());
   scanner.nextToken();
   if(scanner.hasMoreTokens()) end = stringToInteger(scanner.nextToken());
}

/*
 * Function: listProgram
 * Usage:  listProgram(program);
 * -----------------------------------------------
 * Receives a stored program and prints it out line-by-line.
 */
void listProgram(Program & program, int index, int end){
	if (index == NULL) index = program.getFirstLineNumber();
	if (end == NULL) {
		while (index != -1){
			cout << program.getSourceLine(index) << endl;
			index = program.getNextLineNumber(index);
		}
	} else {
		while (index != -1 && index <= end){
			cout << program.getSourceLine(index) << endl;
			index = program.getNextLineNumber(index);
		}
	}
	cout << endl;
}

/*
 * Function: clearGraphics
 * Usage: clearGraphics();
 * --------------------------------------------------------
 * Draws a white rectangle the size of the window and calls
 * genGraphics to give an effect of graphics reset.
 * Also, Statement objects detect the current color as "#FFFFFF"
 * (white) and begin printing graphics from initial points.
 */
void clearGraphics(){
   drawImage(BG_FILE, 0, 0);
   genGraphics();
}

/*
 * Function: run
 * Usage:  run(program, state);
 * ----------------------------------------------------
 * Receives a stored program and executes its statements 
 * by line order. Reloads graphics showing current line if
 * it reaches end of screen.
 * If an IF or GOTO command disrupts execution order with 
 * a given line number, the statement corresponding to that 
 * given number is executed. Normal line order execution 
 * resumes thereafter.
 */
void run(Program & program, EvalState & state){
	reloadCurrentLineGraphics();
	double order = getStringWidth("Current Line: ") + 5;
	drawString("START -> ", order + 5, (WINDOW_HEIGHT-5));
	order += getStringWidth("START -> ") + 5;
	int index = program.getFirstLineNumber();
	while(index != -1){
		drawString(integerToString(index) + " -> ", order, (WINDOW_HEIGHT-5));
		order += 30;
		program.getParsedStatement(index)->execute(state);
		if(state.getNextLine() != NULL) {
			index = state.getNextLine();
		} else {
			index = program.getNextLineNumber(index);
		}
		if (order > WINDOW_WIDTH) {
			reloadCurrentLineGraphics();
			order = getStringWidth("Current Line: ") + 5; 
		}
	}
	cout << endl;
	drawString("END!", order + 5, (getWindowHeight()-5));
}

/*
 * Function: debug
 * Usage:  debug(program, state);
 * ----------------------------------------------------
 * Receives a stored program and executes its statements 
 * by line order, line by line. Reloads graphics showing 
 * current line if it reaches end of screen. After every 
 * line execution, the program waits for user to click in 
 * the graphics window before proceeding. For more info on 
 * execution, see documentation for run.
 */
void debug(Program & program, EvalState & state){
	reloadCurrentLineGraphics();
	double order = getStringWidth("Current Line: ") + 5;
	drawString("START -> ", order + 5, (WINDOW_HEIGHT-5));
	order += getStringWidth("START -> ") + 5;
	int index = program.getFirstLineNumber();
	while(index != -1){
		drawString(integerToString(index) + " -> ", order, (getWindowHeight()-5));
		order += 30;
		program.getParsedStatement(index)->execute(state);
		if(state.getNextLine() != NULL) {
			index = state.getNextLine();
		} else {
			index = program.getNextLineNumber(index);
		}
		waitForClick();
		if (order > WINDOW_WIDTH) {
			reloadCurrentLineGraphics();
			order = getStringWidth("Current Line: ") + 5; 
		}
	}
	cout << endl;
	drawString("END!", order + 5, (getWindowHeight()-2));
}

/*
 * Function: reloadCurrentLineGraphics
 * Usage: reloadCurrentLineGraphics();
 * --------------------------------------------------------
 * Draws a white rectangle the size of the current-line-display row 
 * to give an effect of graphics reloading.
 */
void reloadCurrentLineGraphics(){
	drawImage(BG_FILE, 0, WINDOW_HEIGHT-14, WINDOW_WIDTH, 14);
	drawString("Current Line: ", 5, (WINDOW_HEIGHT-5));
}

/*
 * Function: printHelpMsg
 * Usage: printHelpMsg();
 * -------------------------------------------
 * Prints out a message with coding instructions for this interpreter.
 */
void printHelpMsg(){
	cout << "This program allows you to code, save and run programs written in BASIC." << endl;
	cout << "--------------------------------------------" << endl << endl;
	cout << "Each valid line of code begins with either a number or a"; 
	cout << " valid command." << endl;
	cout << "If line begins with number, the code after the number is";
	cout << " stored for execution." << endl;
	cout << "Program is sequenced according to these line numbers." << endl;
	cout << "--------------------------------------------" << endl << endl;
	printCmds();
	printStmts();
	printFeatures();
}

/*
 * Function: printCmds
 * Usage: printCmds();
 * ---------------------------------------
 * Prints out a list of commands accepted by the interpreter and their effects.
 */
void printCmds(){
	cout << "The following commands are accepted by the interpreter:" << endl;
	cout << "SAVE - Saves the current program to a text file" << endl;
	cout << "OLD - Loads a previous program from a text file" << endl;
	cout << "RUN - Runs the stored program" << endl;
	cout << "DEBUG - Runs the stored program line by line" << endl;
	cout << "LIST - Lists the stored program" << endl;
	cout << "CLEAR - Deletes the stored program" << endl;
	cout << "HELP - Displays help information" << endl;
	cout << "QUIT - Exits the interpreter" << endl;
	cout << "--------------------------------------------" << endl << endl;
}

/*
 * Function: printStmts
 * Usage: printStmts();
 * ----------------------------------------
 * Prints out a list of statements accepted by the interpreter and their effects.
 */
void printStmts(){
	cout << "The term following the line number needs to be a valid statement, so the";
	cout << " interpreter knows how to execute that line of code." << endl;
	cout << "The following statements are accepted by the interpreter:" << endl << endl;
	cout << "REM - [Usage: REM exp]" << endl;
	cout << "	Used for comments." << endl;
	cout << "LET - [Usage: LET var = exp]" << endl;
	cout << "	Assignment statement. Assigns the value of the expression to the";
	cout << " variable, replacing any previous value." << endl;
	cout << "PRINT - [Usage: PRINT exp]" << endl;
	cout << "	Prints value of the expression to the console." << endl;
	cout << "INPUT - [Usage: INPUT var]" << endl;
	cout << "	Reads in a variable from the user. Prompts the user by printing \" ? \","; 
	cout << " and assigns the input value to the variable." << endl;
	cout << "GOTO - [Usage: GOTO n]" << endl;
	cout << "	Forces program to execute line n instead of the next stored line." << endl;
	cout << "IF - [Usage: IF exp1 op exp2 THEN n]" << endl;
	cout << "	Conditional operator op accepts =, <, and > to compare exp1 and exp2."; 
	cout << " If condition holds, executes line n instead of the next stored line. If";
	cout << " not, program executes the next stored line." << endl;
	cout << "END - [Usage: END]" << endl;
	cout << "	Halts program execution" << endl;
	cout << "--------------------------------------------" << endl << endl;
}

/*
 * Function: printExtns
 * Usage: printExtns();
 * ----------------------------------------
 * Prints out a list of special features of this interpreter.
 */
void printFeatures(){
 cout << "== FEATURES ==" << endl;
 cout << "The program comes packaged with the following features:" << endl << endl;
 cout << "- All commands and statements are case-insensitive." << endl;
 cout << "- The minus sign (-) can be used both as a unary and binary operator." << endl;
 cout << "- LET statement is optional. Assignment works either way." << endl;
 cout << "- PRINT statement accepts an optional beginning with a string bound by \"\"" << endl;
 cout << "- PRINT statement accepts a list of values/experessions/ variables";
 cout << " separated by a comma." << endl;
 cout << "- Program works with floating-point numbers too.";
 cout << "- LIST command accepts an optional range for listing only a part of the";
 cout << " program (eg, LIST 50-80)." << endl;
 cout << "- Capability to save to and load from text files." << endl;
 cout << "- A graphical debugger that displays program state (all variables,";
 cout << " expressions, conditions, current line number, etc) both before";
 cout << " and after execution." << endl;
 cout << "- CLEAR command resets the graphical debugger to original state, apart ";
 cout << "from clearing stored program." << endl;
 cout << "- A debug mode that allows users to run through the program line by line." << endl;
 cout << "- A print() function in program.cpp to show program structure in console.";
 cout << "- Typing in an already existing line number with a blank expression removes ";
 cout << "that line from the program." << endl;
 cout << "--------------------------------------------" << endl << endl;
}
