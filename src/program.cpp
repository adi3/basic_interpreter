/*
 * File: program.cpp
 * -----------------
 * Implements the program.h interface.
 */

#include <iostream>
#include "program.h"
#include "hashmap.h"
using namespace std;

/*
 * Implementation: Program
 * --------------------------
 * Constructs an empty BASIC program.
 */

Program::Program() {
	firstLineNum = -1;
}

/*
 * Implementation: ~Program
 * --------------------------
 * Frees any heap storage associated with the program.
 */

Program::~Program() {
	clear();
}

/*
 * Implementation: clear
 * -----------------------
 * Removes all lines from the program.
 */

void Program::clear() {
	int index = firstLineNum;
	while(map.size() > 0){
		if(map[index]->next == NULL) break;
		int nextIndex = map[index]->next->lineNum;
		delete map[index];
		index = nextIndex;
	}
	delete map[index];
	map.clear();
	firstLineNum = -1;
}

/*
 * Implementation: addSourceLine
 * -----------------------------------------------
 * Adds a source line to the program with the specified line number.
 * If that line already exists, the text of the line replaces
 * the text of any existing line and the parsed representation
 * (if any) is deleted.  If the line is new, it is added to the
 * program in the correct sequence.
 */

void Program::addSourceLine(int lineNumber, string line) {
	if (firstLineNum == -1) firstLineNum = lineNumber;
	Entry *temp = insertEntry(lineNumber, line);
	int prevLineNumber = findPrevLine(lineNumber);
	connectEntry(lineNumber, prevLineNumber, temp);
	//print();
}

/*
 * Implementation: removeSourceLine
 * --------------------------------------------
 * Removes the line with the specified number from the program,
 * freeing the memory associated with any parsed representation.
 * If no such line exists, this method simply returns without
 * performing any action.
 */

void Program::removeSourceLine(int lineNumber) {
	if(map.containsKey(lineNumber)) removeEntry(lineNumber);
	//print();
}

/*
 * Implementation: getSourceLine
 * -------------------------------------------------------
 * Returns the program line with the specified line number.
 * If no such line exists, this method returns the empty string.
 */

string Program::getSourceLine(int lineNumber) {
   if(map.containsKey(lineNumber)) return map[lineNumber]->command;
   return "";
}

/*
 * Implementation: setParsedStatement
 * ----------------------------------------------------
 * Adds the parsed representation of the statement to the statement
 * at the specified line number.  If no such line exists, this
 * method raises an error.  If a previous parsed representation
 * exists, the memory for that statement is reclaimed.
 */

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
	if(map.containsKey(lineNumber)) {
		map[lineNumber]->stmt = stmt;
	} else {
		error ("Invalid like number" + integerToString(lineNumber));
	}
}

/*
 * Implementation: getParsedStatement
 * ----------------------------------------------------------------
 * Retrieves the parsed representation of the statement at the
 * specified line number.  If no value has been set, this method
 * returns NULL.
 */

Statement *Program::getParsedStatement(int lineNumber) {
	if(map.containsKey(lineNumber)) {
		return map[lineNumber]->stmt;
	} else {
		error("Invalid line number: " + integerToString(lineNumber));
	}
   return NULL;
}

/*
 * Implementation: getFirstLineNumber
 * -----------------------------------------------------
 * Returns the line number of the first line in the program.
 * If the program has no lines, this method returns -1.
 */

int Program::getFirstLineNumber() {
   if(!map.isEmpty()) return firstLineNum;
   return -1;
}

/*
 * Impletementation: getNextLineNumber
 * ------------------------------------------------------------
 * Returns the line number of the first line in the program whose
 * number is larger than the specified one, which must already exist
 * in the program.  If no more lines remain, this method returns -1.
 */

int Program::getNextLineNumber(int lineNumber) {
   if(!map.containsKey(lineNumber)) {
	 int prevLineNumber = findPrevLine(lineNumber);
	 if (prevLineNumber == 0) return map[firstLineNum]->lineNum;
	 if(map[prevLineNumber]->next != NULL) {
		 return map[prevLineNumber]->next->lineNum;
	 }
	 return -1;
   }
   if(map[lineNumber]->next != NULL) return map[lineNumber]->next->lineNum;
   return -1;
}


/*--PRIVATE METHODS--*/

/*
 * Function: insertEntry
 * Usage: Entry *temp = insertEntry(lineNumber, line);
 * ------------------------------------------------------
 * Receives a line number followed by a line of code.
 * Generates a corresponding Entry object and inserts it
 * into the map that stores the whole sorted program.
 */
Program::Entry *Program::insertEntry(int lineNumber, string line) {
	Entry *temp = new Entry;
	temp->lineNum = lineNumber;
	temp->command = line;
	temp->stmt = NULL;
	if(!map.containsKey(lineNumber)) {
		temp->next = NULL;
	} else {
		temp->next = map[lineNumber]->next;
	}
	map[lineNumber] = temp;
	return temp;
}
	
/*
 * Function: findPrevLine
 * Usage: int prevLineNumber = findPrevLine(lineNumber);
 * ------------------------------------------------------
 * Returns the line number index that comes right before
 * the given line number in the map.
 */
int Program::findPrevLine(int lineNumber){
	for(int i = lineNumber-1; i >= 0; i--){
		if(map.containsKey(i)) return i;
	}
	return -1;
}

/*
 * Function: connectEntry
 * Usage: connectEntry(lineNumber, prevLineNumber, temp);
 * ------------------------------------------------------
 * Inserts an entry into the existing map, and ensures that
 * all pointers point to elements in ascending order of 
 * line numbers.
 */
void Program::connectEntry(int lineNumber, int prevLineNumber, Entry *temp) {
	if (lineNumber < firstLineNum){
		map[lineNumber]->next = map[firstLineNum];
		firstLineNum = lineNumber;
	} else {
		insertInBetween(lineNumber, prevLineNumber, temp);
	}
}

/*
 * Function: insertInBetween
 * Usage: insertInBetween(lineNumber, prevLineNumber, temp);
 * ---------------------------------------------------------
 * Inserts an entry "temp" between two already existing 
 * entries at line number "lineNumber".
 * Ensures the entry right before this new entry points to 
 * it and the new entry in turn points to the entry right
 * after it.
 */
void Program::insertInBetween(int lineNumber, int prevLineNumber, Entry *temp) {
	if(lineNumber > firstLineNum) {
		if(map[prevLineNumber]->next != NULL) {
			Entry *store = new Entry;
			store = map[prevLineNumber]->next;
			if(map[prevLineNumber]->next->lineNum != 
				map[lineNumber]->lineNum) map[lineNumber]->next = store;
		}
		map[prevLineNumber]->next = temp;
	} 
}

/*
 * Function: removeEntry
 * Usage: removeEntry(lineNumber);
 * ------------------------------------------------------
 * Removes entry corresponding to received line number from map.
 * Ensures that the entry originally before now points to the
 * entry that was originally after it, thereby mainiting program
 * execution chain.
 */
void Program::removeEntry(int lineNumber){
	if(lineNumber != firstLineNum){
		int lastIndex;
		for(int i = lineNumber-1; i >= 0; i--){
			if(map.containsKey(i) && map[i]->next != NULL){
				if (map[i]->next->lineNum == lineNumber) lastIndex = i;
			}
		}
		map[lastIndex]->next = map[lineNumber]->next;
	} else {
		if(map.size() != 1) firstLineNum = map[lineNumber]->next->lineNum;
	}
	delete map[lineNumber]->stmt;
	delete map[lineNumber];
	map.remove(lineNumber);	
}

/*
 * Function: print
 * Usage: print();
 * ------------------------------------------------------
 * Prints out all line numbers and their next connections
 * in ascending order beginning from first line number.
 * Useful for debugging.
 */
void Program::print(){
	int index = firstLineNum;
	while(map.size() > 0){
		cout <<  "this: " << integerToString(map[index]->lineNum);
		if(map[index]->next == NULL) break;
		cout << " -> next: " << integerToString(map[index]->next->lineNum) << endl;
		index = map[index]->next->lineNum;
	}
	cout << endl;
}