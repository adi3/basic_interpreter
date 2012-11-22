/*
 * File: evalstate.h
 * -----------------
 * This interface exports a class called EvalState, which keeps track
 * of additional information required by the evaluator, most notably
 * the values of variables and disruptions in execution order.
 */

#ifndef _evalstate_h
#define _evalstate_h

#include <string>
#include "map.h"
#include "strlib.h"

/*
 * Class: EvalState
 * ----------------
 * This class is passed by reference through the recursive levels
 * of the evaluator and contains information from the evaluation
 * environment that the evaluator may need to know. This class
 * contains a symbol table that maps variable names into their 
 * values. In addition, this class keeps track of disruptions in 
 * execution order by IF and GOTO statements.
 */

class EvalState {

public:

/*
 * Constructor: EvalState
 * Usage: EvalState state;
 * -----------------------
 * Creates a new EvalState object with no variable bindings.
 */

   EvalState();

/*
 * Destructor: ~EvalState
 * Usage: usually implicit
 * -----------------------
 * Frees all heap storage associated with this object.
 */

   ~EvalState();

/*
 * Method: setValue
 * Usage: state.setValue(var, value);
 * ----------------------------------
 * Sets the value associated with the specified var.
 */

   void setValue(std::string var, double value);

/*
 * Method: getValue
 * Usage: int value = state.getValue(var);
 * ---------------------------------------
 * Returns the value associated with the specified variable.
 */

   double getValue(std::string var);

/*
 * Method: isDefined
 * Usage: if (state.isDefined(var)) . . .
 * --------------------------------------
 * Returns true if the specified variable is defined.
 */

   bool isDefined(std::string var);

/*
 * Method: setNextLine
 * Usage: state.setNextLine(string lineNum);
 * ------------------------------------------
 * Sets the index of the next line to be executed.
 */
   void setNextLine(string lineNum);

/*
 * Method: getNextLine
 * Usage: state.getNextLine();
 * ----------------------------------------
 * Returns the index of the next line to be executed.
 */
   int getNextLine();

private:

   Map<std::string,double> symbolTable;
   int nextLine;

};

#endif
