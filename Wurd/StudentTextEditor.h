#ifndef STUDENTTEXTEDITOR_H_
#define STUDENTTEXTEDITOR_H_

#include "TextEditor.h"
#include "StudentUndo.h"
#include <list>
#include <string>

class Undo;

class StudentTextEditor : public TextEditor {
public:
	// Add NO new public functions/varialbes
	
	// 
	StudentTextEditor(Undo* undo);
	~StudentTextEditor();
	bool load(std::string file);
	bool save(std::string file);
	void reset();
	void move(Dir dir);
	void del();
	void backspace();
	void insert(char ch);
	void enter();
	void getPos(int& row, int& col) const;
	int getLines(int startRow, int numRows, std::vector<std::string>& lines) const;
	void undo();

private:
	// can't use vectors, deques or std::array
	// CAN use list, map, set

	// CORE VARIABLES:
	// 1. list linesBeingEdited
	// 2. current editing position curRow, curCol
	int m_curRow;
	int m_curCol;
	std::list<std::string> m_lines;
	std::list<std::string>::iterator m_curRowIter;
	// 3. data to faciliate undo operations? pointer to StudentUndo

	// other variables

	// GOAL: 
	// insert and delete characters
	// load and save text files
	// undo your changes
	void simpleInsert(char ch);
	
};

#endif // STUDENTTEXTEDITOR_H_
