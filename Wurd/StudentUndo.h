#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <stack>
#include <queue>
#include <list>

class StudentUndo : public Undo 
{
public:
	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:

	struct Command
	{
		Command(Action act, int r, char cr = 0)
			: action(act), row(r), ch(cr), count(0)
		{

		}
		Action action;
		int row;
		std::queue<int> col;
		char ch;
		int count;
		std::string batch = "";
		std::list<char> restore;
	};

	std::stack<Command> m_commands;

};

#endif // STUDENTUNDO_H_
