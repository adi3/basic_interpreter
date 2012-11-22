/*
 * File: programpriv.h
 * -------------------
 * This file contains the private section of the Program class.
 */

private:

	/* Instance variables */
	struct Entry{
		int lineNum;
		string command;
		Statement *stmt;
		Entry *next;
	};
	HashMap<int, Entry*> map;
	int firstLineNum;

	/* Function prototypes */
	Entry *insertEntry(int lineNumber, string line);
	int findPrevLine(int lineNumber);
	void connectEntry(int lineNumber, int prevLineNumber, Entry *temp);
	void insertInBetween(int lineNumber, int prevLineNumber, Entry *temp);
	void removeEntry(int lineNumber);
	void print();
