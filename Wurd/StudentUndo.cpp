#include "StudentUndo.h"
#include <stack>
#include <queue>
#include <list>
#include <string>
using namespace std;

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) 
{
	// If the stack empty, just push the command to it
	if (m_commands.empty())
	{
		Command newCommand(action, row);
		newCommand.col.push(col);			// Tracks columns
		newCommand.count += 1;				// For insertions
		newCommand.restore.push_back(ch);	// For deletions/backspaces
		m_commands.push(newCommand);
		return;
	}
	// Otherwise, stack not empty
	// Checks if the new command should be batched with the top command for insertions/deletions
	Command& topCommand = m_commands.top();
	switch (action)
	{
		case Undo::Action::INSERT:
			// Consecutive insert
			if (action == topCommand.action && row == topCommand.row && col == topCommand.col.back() + 1)
			{
				topCommand.col.push(col);
				topCommand.count += 1;
				return;
			}
			break;
		case Undo::Action::DELETE:
			if (action == topCommand.action && row == topCommand.row)
			{
				if (col == topCommand.col.back())	// Delete was pushed
				{ 
					topCommand.col.push(col);
					topCommand.restore.push_back(ch);
					return;
				}
				else if (col == topCommand.col.back() - 1)	// Backspace was pushed
				{
					topCommand.col.push(col);
					topCommand.restore.push_front(ch);   // Order of characters must be reversed for backspace, so use push_front
					return;
				}
			}
			break;
		default:
			break;
	}

	// No batching case was detectd, so just regularly push the new command onto stack
	Command newCommand(action, row);
	newCommand.col.push(col);			// Tracks columns
	newCommand.count += 1;				// For insertions
	newCommand.restore.push_back(ch);	// For deletions/backspaces
	m_commands.push(newCommand);
}

// Returns the relevant information of the lastest undo command to the text editor
StudentUndo::Action StudentUndo::get(int &row, int &col, int& count, std::string& text) 
{
	if (m_commands.empty())
		return Action::ERROR;  

	// Assign default values for action, text, count, row, and col
	Command topCommand = m_commands.top();
	Action undoAction = Action::ERROR;
	count = 1; text = ""; row = topCommand.row; col = topCommand.col.back();
	switch (topCommand.action)
	{
		case Undo::Action::INSERT:
		{
			undoAction = Undo::Action::DELETE;
			col = topCommand.col.front() - 1;	// Want to delete where the words ENDS, so one left of where the cursor moved to
			count = topCommand.count;			
			break;
		}
		case Undo::Action::DELETE:
		{
			undoAction = Undo::Action::INSERT;
			// Add contents of restore list into text -> linear to length of text deleted
			for (list<char>::iterator it = topCommand.restore.begin(); it != topCommand.restore.end(); it++)
				text += (*it);		
			break;
		}
		case Undo::Action::SPLIT:
		{
			undoAction = Undo::Action::JOIN;
			break;
		}
		case Undo::Action::JOIN:
		{
			undoAction = Undo::Action::SPLIT;
			break;
		}
		default:
			return Undo::Action::ERROR;
	}
	m_commands.pop();
	return undoAction;
}

// Clears undo stack of all undo commands
void StudentUndo::clear() 
{
	while (!m_commands.empty())
		m_commands.pop();
}


