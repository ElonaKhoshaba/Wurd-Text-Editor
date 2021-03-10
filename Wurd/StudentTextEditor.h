#ifndef STUDENTTEXTEDITOR_H_
#define STUDENTTEXTEDITOR_H_

#include "TextEditor.h"
#include "StudentUndo.h"
#include <list>
#include <string>

class Undo;

class StudentTextEditor : public TextEditor 
{
public:
	StudentTextEditor(Undo* undo);	// Initializes current editing position and line list
	~StudentTextEditor();			// Does nothing, no dynamic memory allocation used

	//////////////////////
	// Public Functions //
	//////////////////////

	// Loads the contents of a text file off disk into the editor
	bool load(std::string file);

	// Saves the contents of the text editor to a file on your disk 
	// overwriting any previous data in the file with new contents
	bool save(std::string file);

	// Clears text editor's contents and resets the editing position to the top of the file
	// Also resets undo state, so no possible undos are possible after the reset 
	void reset();

	// Adjusts current editing position
	void move(Dir dir);

	// Deletes a character at the given position, or merges the line below with the current
	// line if the cursor is just past the end of the given line
	void del();

	// Deletes a character at the position one before the given position, or merges
	// the line above with the current line if the cursor is at the first character of the given line
	void backspace();

	// Inserts a character at the given position, but if the character is a tab
	// inserts four spaces 
	void insert(char ch);

	// Splits the line into two at the current position
	void enter();

	// Returns the current row and col position
	void getPos(int& row, int& col) const;

	// Gets the lines in the text editor from startRow to numRows
	// Returns the number of lines gotten
	int getLines(int startRow, int numRows, std::vector<std::string>& lines) const;
	
	// Undoes the lastest change (insert, delete, enter, merge) made by the user 
	void undo();

private:
	int m_curRow;									// Current row being edited
	int m_curCol;									// Current col being edited
	std::list<std::string>::iterator m_curRowIter;	// Points to current row being edited
	std::list<std::string> m_lines;					// Lines being edited	

	//////////////////////////////
	// Private Helper Functions //
	//////////////////////////////

	// Inserts a character at the given position (\t not accounted for)
	void simpleInsert(char ch);
};

#endif // STUDENTTEXTEDITOR_H_
