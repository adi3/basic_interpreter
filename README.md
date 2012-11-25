# BASIC INTERPRETER

A tiny but powerful interpreter that allows a user to code, save and run programs in BASIC. The commands and statements accepted by the app are documented here.

## Commands

* **SAVE**: Saves the current program to a text file
* **OLD**: Loads a previous program from a text file
* **RUN**: Runs the stored program
* **DEBUG**: Runs the stored program line by line
* **LIST**: Lists the stored program (w optional limits)
* **CLEAR**: Deletes the stored program
* **HELP**: Displays help information
* **QUIT**: Exits the interpreter

## Statements

* **REM** - *[Usage: REM exp]*: Used for comments.
* **LET** - *[Usage: LET var = exp]*: Assignment statement. Assigns the value of the expression to the variable, replacing any previous value.
* **PRINT** - *[Usage: PRINT exp]*: Prints value of the expression to the console.
* **INPUT** - *[Usage: INPUT var]*: Reads in a variable from the user. Prompts the user by printing " ? ", and assigns the input value to the variable.
* **GOTO** - *[Usage: GOTO n]*: Forces program to execute line n instead of the next stored line.
* **IF** - *[Usage: IF exp1 op exp2 THEN n]*: Conditional operator op accepts =, <, and > to compare exp1 and exp2. If condition holds, executes line n. If not, program executes the next stored line.
* **END** - *[Usage: END]*: Halts program execution.

## Features

* All commands and statements are case-insensitive.
* The minus sign (-) can be used both as a unary and a binary operator.
* LET statement is optional. Assignment works either way.
* PRINT statement accepts an optional beginning with a string bound by " ".
* PRINT statement accepts a list of values/experessions/ variables separated by a comma.
* Program works with floating-point numbers too.
* LIST command accepts an optional range for listing only a part of the program (eg, LIST 50-80).
* Capability to save to and load from text files.
* A graphical debugger that displays program state (all variables, expressions, conditions, current line number, etc) both before and after
* program execution.
* CLEAR command resets the graphical debugger to original state, apart from clearing stored program.
* A debug mode that allows users to run through the program line by line.
* A print() function in *program.cpp* to show program structure in console.
* Typing in an already existing line number with a blank expression removes that line from the program.
