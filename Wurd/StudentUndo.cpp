#include "StudentUndo.h"
#include <stack>
#include <string>
using namespace std;

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) 
{

	Command newCommand(action, row, col, ch);
	m_commands.push(newCommand);
	//}
	//else
	//{
	//	Command curCommand = m_commands.top();
	//	switch (action)
	//	{
	//	case Undo::Action::INSERT:
	//		
	//	case Undo::Action::DELETE:
	//		if (curCommand.m_action == action && curCommand.m_row == row && curCommand.m_col == col)
	//		{

	//		}
	//	case Undo::Action::JOIN:
	//		
	//	case Undo::Action::SPLIT:
	//		
	//	}
	//}

	// m_commands.top().m_batch += ch;
}

StudentUndo::Action StudentUndo::get(int &row, int &col, int& count, std::string& text) 
{
	if (m_commands.empty())
		return Action::ERROR;  // TODO


	Action undoAction = Action::ERROR;
	switch (m_commands.top().m_action)
	{
	case Undo::Action::INSERT:
		undoAction = Undo::Action::DELETE;
		break;
	case Undo::Action::DELETE:
		undoAction = Undo::Action::INSERT;
		break;
	case Undo::Action::JOIN:
		undoAction = Undo::Action::SPLIT;
		break;
	case Undo::Action::SPLIT:
		undoAction = Undo::Action::JOIN;
		break;
	}
	text = m_commands.top().m_batch;
	row = m_commands.top().m_row;
	col = m_commands.top().m_col;
	m_commands.pop();
	return undoAction;
}

void StudentUndo::clear() {
	// TODO
}


