#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <stack>

class StudentUndo : public Undo 
{
public:
	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:

	struct Command
	{
		Command(Action action, int row, int col, char ch = 0)
			: m_action(action), m_row(row), m_col(col), m_ch(ch) 
		{

		}
		Action m_action;
		int m_row;
		int m_col;
		char m_ch;
		std::string m_batch = "";
	};

	std::stack<Command> m_commands;

};

#endif // STUDENTUNDO_H_
