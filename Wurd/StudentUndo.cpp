#include "StudentUndo.h"
#include <stack>
#include <queue>
#include <string>
#include <iostream>
using namespace std;

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) 
{
	// if stack empty, just push the command to it
	if (m_commands.empty())
	{
		Command newCommand(action, row, ch);
		newCommand.col.push(col);			// Tracks columns
		newCommand.batch += ch;				// For insertions
		newCommand.count += 1;				// For insertions
		newCommand.restore.push_back(ch);	// For deletions/backspaces
		m_commands.push(newCommand);
		return;
	}
	// Otherwise, stack not empty
	// Check if the new command should be batched with the top command
	Command& topCommand = m_commands.top();
	switch (action)
	{
	case Undo::Action::INSERT:
		// Consecutive insert
		if (action == topCommand.action && row == topCommand.row && col == topCommand.col.back() + 1)
		{
			topCommand.col.push(col);
			topCommand.batch += ch;	// Add to running batch
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
	// Otherwise, 
	// No batching, just regularly push the new command onto stack
	Command newCommand(action, row, ch);
	newCommand.col.push(col);			// Tracks columns
	newCommand.batch += ch;				// For insertions
	newCommand.count += 1;				// For insertions
	newCommand.restore.push_back(ch);	// For deletions/backspaces
	m_commands.push(newCommand);
}

StudentUndo::Action StudentUndo::get(int &row, int &col, int& count, std::string& text) 
{
	if (m_commands.empty())
		return Action::ERROR;  // TODO

	Command topCommand = m_commands.top();
	Action undoAction = Action::ERROR;
	count = topCommand.count;
	switch (topCommand.action)
	{
	case Undo::Action::INSERT:
		undoAction = Undo::Action::DELETE;
		col = topCommand.col.front() - 1;
		text = topCommand.batch;
		break;

	case Undo::Action::DELETE:
		undoAction = Undo::Action::INSERT;
		col = topCommand.col.back();
		for (list<char>::iterator it = topCommand.restore.begin(); it != topCommand.restore.end(); it++)
		{
			text += (*it);
		}
		break;
	/*
	case Undo::Action::JOIN:
		undoAction = Undo::Action::SPLIT;
		break;
	case Undo::Action::SPLIT:
		undoAction = Undo::Action::JOIN;
		break;*/
	default:
		break;
	}

	
	row = topCommand.row;
	m_commands.pop();
	return undoAction;
}

void StudentUndo::clear() 
{
	while (!m_commands.empty())
	{
		m_commands.pop();
	}
}


