/*
 * File: exp.cpp
 * -------------
 * This file implements the Expression class and its subclasses.
 */

#include <string>
#include "exp.h"
#include "error.h"
#include "strlib.h"
using namespace std;

/*
 * Implementation notes: the Expression class
 * ------------------------------------------
 * Declares no instance variables and needs no code.
 */

Expression::Expression() {
   /* Empty */
}

Expression::~Expression() {
   /* Empty */
}

/*
 * Implementation notes: the ConstantExp subclass
 * ----------------------------------------------
 * Declares a single instance variable that stores the value of
 * the constant. The eval method doesn't use the value of state
 * but needs it to match the general prototype for eval.
 */

ConstantExp::ConstantExp(double value) {
   this->value = value;
}

double ConstantExp::eval(EvalState & state) {
   return value;
}

string ConstantExp::toString() {
   return realToString(value);
}

ExpressionType ConstantExp::getType() {
   return CONSTANT;
}

double ConstantExp::getValue() {
   return value;
}

/*
 * Implementation notes: the IdentifierExp subclass
 * ------------------------------------------------
 * eclares a single instance variable that stores the name of 
 * the variable. The implementation of eval must look up this
 * variable in the evaluation state.
 */

IdentifierExp::IdentifierExp(string name) {
   this->name = name;
}

double IdentifierExp::eval(EvalState & state) {
   if (!state.isDefined(name)) error(name + " is undefined");
   return state.getValue(name);
}

string IdentifierExp::toString() {
   return name;
}

ExpressionType IdentifierExp::getType() {
   return IDENTIFIER;
}

string IdentifierExp::getName() {
   return name;
}

/*
 * Implementation notes: the CompoundExp subclass
 * ----------------------------------------------
 * Declares instance variables for the operator and the left and right
 * subexpressions.  The implementation of eval evaluates the subexpressions
 * recursively and then applies the operator.
 */

CompoundExp::CompoundExp(string op, Expression *lhs, Expression *rhs) {
   this->op = op;
   this->lhs = lhs;
   this->rhs = rhs;
}

CompoundExp::~CompoundExp() {
   delete lhs;
   delete rhs;
}

/*
 * Implementation notes: eval
 * --------------------------
 * The eval method for the compound expression case must check for the
 * assignment operator as a special case.  Unlike the arithmetic operators
 * the assignment operator does not evaluate its left operand.
 */

double CompoundExp::eval(EvalState & state) {
   if (op == "=") {
      if (lhs->getType() != IDENTIFIER) {
         error("Illegal variable in assignment");
      }
      double val = rhs->eval(state);
      state.setValue(((IdentifierExp *) lhs)->getName(), val);
      return val;
   }
   double left = lhs->eval(state);
   double right = rhs->eval(state);
   if (op == "+") return left + right;
   if (op == "-") return left - right;
   if (op == "*") return left * right;
   if (op == "/") return left / right;
   error("Illegal operator in expression");
   return 0;
}

string CompoundExp::toString() {
   return '(' + lhs->toString() + ' ' + op + ' ' + rhs->toString() + ')';
}

ExpressionType CompoundExp::getType() {
   return COMPOUND;
}

string CompoundExp::getOp() {
   return op;
}

Expression *CompoundExp::getLHS() {
   return lhs;
}

Expression *CompoundExp::getRHS() {
   return rhs;
}
