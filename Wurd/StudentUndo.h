#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <stack>
#include <queue>
#include <list>

class StudentUndo : public Undo 
{
public:
	//////////////////////
	// Public Functions //
	//////////////////////

	// Adds the given undo command to the undo stack, batching 
	// an insertion/deletion command with the top command if necessary
	void submit(Action action, int row, int col, char ch = 0);

	// Returns the relevant information of the lastest undo command to the text editor
	Action get(int& row, int& col, int& count, std::string& text);
	
	// Clears undo stack of all undo commands
	void clear();

private:
	///////////////////////////////////////////
	// Command Struct to Track Undo Commands //
	///////////////////////////////////////////
	struct Command
	{
		Command(Action act, int r)
			: action(act), row(r), count(0)
		{}
		Action action;				// Action done by user
		int row;					// Row where action occurred
		std::queue<int> col;		// Col where action occurred (needs to be a queue for batch cases)
		int count;					// How many chars were deleted (for inserts)
		std::list<char> restore;	// Tracks deleted chars to undo backspaces/deletions
	};

	std::stack<Command> m_commands;	// Stack of undo commands
};

#endif // STUDENTUNDO_H_
