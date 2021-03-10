#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

// Initializes current editing position and line list
StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo), m_curRow(0), m_curCol(0)
{
	m_lines.push_back("");
	m_curRowIter = m_lines.begin();
}

// No dynamic memory allocation used for this class or Undo
StudentTextEditor::~StudentTextEditor()
{

}

// Loads the contents of a text file off disk into the editor
bool StudentTextEditor::load(std::string file) 
{
	// If opening the file fails do nothing
	ifstream textFile(file);
	if (!textFile) 
		return false;
	
	// If already editing an eisting file, reset old content of text editor
	reset();

	// Load the new file, removing the \r's 
	string line;
	while (getline(textFile, line))
	{
		if (!line.empty() && line.back() == '\r')
		{
			line.pop_back();
		}
		m_lines.push_back(line);
	}
	m_curRowIter = m_lines.begin();
	return true;  
}

// Saves the contents of the text editor to a file on your disk 
// overwriting any previous data in the file with new contents
bool StudentTextEditor::save(std::string file) 
{
	// If file opening fails, do nothing
	ofstream saveFile(file);
	if (!saveFile) 
		return false;

	auto iter = m_lines.begin();
	for (int i = 0; i < m_lines.size() && iter != m_lines.end(); i++)
	{	
		saveFile << *iter << endl;	// endl appends \n after each line
		iter++;
	}
	return true;
}

// Clears text editor's contents and resets the editing position to the top of the file
// Also resets undo state, so no possible undos are possible after the reset 
void StudentTextEditor::reset() 
{
	while (!m_lines.empty())
		m_lines.pop_back();
	m_curRow = 0;
	m_curCol = 0;
	m_curRowIter = m_lines.begin();
	if(getUndo() != nullptr)
		getUndo()->clear();
}

// Adjusts current editing position
void StudentTextEditor::move(Dir dir) 
{
	size_t newLineSize;
	switch (dir)
	{
		case Dir::UP:
			if (m_curRow == 0)						// Cursor on first row, do nothing
				return;
			m_curRowIter--;							// Otherwise move cursor up a row
			newLineSize = (*m_curRowIter).size();
			if (newLineSize < m_curCol)				// If the line above has a longer length than the current line,
				m_curCol = newLineSize;				// move the cursor to be at the end of the above line
			m_curRow--;
			break;
	
		case Dir::DOWN:
			if (m_curRow == m_lines.size() - 1 || m_curRowIter == m_lines.end())  // Cursor on last row, do nothing
				return;
			m_curRowIter++;								// Otherwise move cursor down a row
			newLineSize = (*m_curRowIter).size();		
			if (newLineSize < m_curCol)					// If the line below has a longer length than the current line,
				m_curCol = newLineSize;					// move the cursor to be at the end of the below line
			m_curRow++;
			break;

		case Dir::LEFT:
			if (m_curRow == 0 && m_curCol == 0)	     	// Cursor is at beginning of file, do nothing
				return;
			if (m_curCol == 0)							// If editing position is already on the first character of the line
			{											// move cursor to AFTER the last character on the previous line
				m_curRow--;
				m_curRowIter--;
				m_curCol = (*m_curRowIter).size();
			}
			else 
				m_curCol--;
			break; 
	
		case Dir::RIGHT:
			if (m_curRow == m_lines.size() - 1 && m_curCol == (*m_curRowIter).size())	// Cursor is just after last leter of last line, do nothing
				return;
			if (m_curCol == (*m_curRowIter).size())		// If editing position is on just after the last character on the line
			{											// move cursor to the first character on the next line
				m_curRow++;
				m_curRowIter++;
				m_curCol = 0;
			}
			else
				m_curCol++;
			break;
	
		case Dir::HOME:	// Go to the first character of the line
			m_curCol = 0;	
			break;
	
		case Dir::END:	// Go to just after the last character of the line
			m_curCol = (*m_curRowIter).size();	
	}
}

// Deletes a character at the given position, or merges the line below with the current
// line if the cursor is just past the end of the given line
void StudentTextEditor::del() 
{
	if (m_curCol != (*m_curRowIter).size())
	{
		char ch = (*m_curRowIter).at(m_curCol);
		(*m_curRowIter).erase((*m_curRowIter).begin() + m_curCol);			// Erase character at current column
		getUndo()->submit(Undo::Action::DELETE, m_curRow, m_curCol, ch);
	}
	else // cursor at just past end of one line, merge lines
	{
		if (m_curRow == m_lines.size() - 1)
			return;
		auto iter = m_curRowIter;
		iter++;
		(*m_curRowIter).append(*iter);
		m_lines.erase(iter);
		getUndo()->submit(Undo::Action::JOIN, m_curRow, m_curCol);
	}
}

// Deletes a character at the position one before the given position, or merges
// the line above with the current line if the cursor is at the first character of the given line
void StudentTextEditor::backspace() 
{
	if (m_curCol == 0 && m_curRow == 0)		// Can't backspace at the beginning of a file
		return;

	if (m_curCol > 0)
	{
		char ch = (*m_curRowIter).at(m_curCol - 1);
		(*m_curRowIter).erase((*m_curRowIter).begin() + m_curCol - 1);	// Erase character just before cursor
		m_curCol--;														// Move cursor left
		getUndo()->submit(Undo::Action::DELETE, m_curRow, m_curCol, ch);	
	}
	else if (m_curCol == 0)		// Merge lines
	{
		auto iter = m_curRowIter;
		string lineBelow = *iter;
		iter--;
		m_curCol = (*iter).size();
		(*iter).append(lineBelow);	// Append current line to the line above it (*iter = line above)
		m_curRow--; 
		m_curRowIter--;
		auto iterErase = m_curRowIter;
		iterErase++;
		m_lines.erase(iterErase);	// Erase the current line
		getUndo()->submit(Undo::Action::JOIN, m_curRow, m_curCol);
	}
	
}

// Inserts a character at the given position, but if the character is a tab
// insert four spaces 
void StudentTextEditor::insert(char ch) 
{
	if (ch == '\t')
	{
		for (int i = 0; i < 4; i++)
			simpleInsert(' ');
	}
	else
		simpleInsert(ch);
		
}

// Inserts a character at the given position (\t not accounted for)
void StudentTextEditor::simpleInsert(char ch)
{
	(*m_curRowIter).insert((*m_curRowIter).begin() + m_curCol, ch);
	m_curCol++;
	getUndo()->submit(Undo::Action::INSERT, m_curRow, m_curCol, ch);
}

// Splits the line into two at the current position
void StudentTextEditor::enter() 
{
	getUndo()->submit(Undo::Action::SPLIT, m_curRow, m_curCol);
	// Split current line into two parts
	string beforeEnter = (*m_curRowIter).substr(0, m_curCol);
	string afterEnter = (*m_curRowIter).substr(m_curCol);
	*m_curRowIter = beforeEnter;
	auto iter = m_curRowIter;
	// Want to insert at row just after current
	iter++;
	m_lines.insert(iter, afterEnter);
	m_curRow++;
	m_curRowIter++;
	m_curCol = 0;
}

// Returns the current row and col position
void StudentTextEditor::getPos(int& row, int& col) const 
{
	row = m_curRow;
	col = m_curCol;
}

// Gets the lines in the text editor from startRow to numRows
// Returns the number of lines gotten
int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const
{
	if (startRow < 0 || numRows < 0 || startRow > m_lines.size())
		return -1;
	
	// Increment the current position of iter to the point to start row
	auto iter = m_curRowIter;
	int distance = abs(m_curRow - startRow);
	while (distance > 0)
	{
		if (m_curRow < startRow)
			iter++;
		else
			iter--;
		distance--;
	}

	// Clear previous data in lines
	for (int i = 0; i != lines.size(); i++)
		lines.pop_back();

	if (startRow == m_lines.size())	  // Invalid starting position
		return 0;

	for (int j = 0; j < numRows && iter != m_lines.end(); j++)
	{
		lines.push_back(*iter);
		iter++;
	}
	return lines.size();
}

// Undoes the lastest change (insert, delete, enter, merge) made by the user 
void StudentTextEditor::undo() 
{
	int count = 0;
	string text = "";
	int row = 0;
	int col = 0;
	// Gets top undo command from undo stack
	Undo::Action command = getUndo()->get(row, col, count, text);
	
	if (command == Undo::Action::ERROR)
		return;
	
	// Changes current position of iterator to point to where the last change happened
	int distance = abs(m_curRow - row);
	while (distance > 0)
	{
		if (m_curRow < row)
			m_curRowIter++;
		else
			m_curRowIter--;
		distance--;
	}
	m_curRow = row;
	m_curCol = col;

	switch (command)
	{
		case Undo::Action::INSERT:		// Undoes delete
		{
			(*m_curRowIter).insert(m_curCol, text);
			break;
		}
		case Undo::Action::DELETE:		// Undoes insert
		{
			(*m_curRowIter).erase(m_curCol, count);
			break;
		}
		case Undo::Action::JOIN:		// Undoes enter (split)
		{
			if (m_curRow == m_lines.size() - 1)
				return;
			auto iter = m_curRowIter;
			iter++;
			(*m_curRowIter).append(*iter);
			m_lines.erase(iter);
			break;
		}
		case Undo::Action::SPLIT:
		{
			// Split current line into two parts
			string beforeEnter = (*m_curRowIter).substr(0, m_curCol);
			string afterEnter = (*m_curRowIter).substr(m_curCol);
			*m_curRowIter = beforeEnter;
			auto iter = m_curRowIter;
			// Want to insert at row just after current
			iter++;
			m_lines.insert(iter, afterEnter);
			break;
		}
	}
}
