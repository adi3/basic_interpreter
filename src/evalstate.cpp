/*
 * File: evalstate.cpp
 * -------------------
 * This file implements the EvalState class, which defines a symbol
 * table for keeping track of the value of identifiers.
 */

#include <string>
#include "evalstate.h"
using namespace std;

/* Implementation of the EvalState class */

EvalState::EvalState() {
   /* Empty */
}

EvalState::~EvalState() {
   /* Empty */
}

void EvalState::setValue(string var, double value) {
   symbolTable.put(var, value);
}

double EvalState::getValue(string var) {
   return symbolTable.get(var);
}

void EvalState::setNextLine(string lineNum) {
	nextLine = (lineNum != "NULL") ? stringToInteger(lineNum) : NULL;
}

int EvalState::getNextLine() {
   return nextLine;
}

bool EvalState::isDefined(string var) {
   return symbolTable.containsKey(var);
}
