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

// Initializes current editing position, and everything else to manage data structures
// RUNTIME: O(1)
StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo), m_curRow(0), m_curCol(0)
{
	m_lines.push_back("");
	m_curRowIter = m_lines.begin();
}

// Should NOT free Undo object
// RUNTIME: O(N), where N is the number of lines in the file currently being edited
StudentTextEditor::~StudentTextEditor()
{
	// TODO
	// for each line in linesBeingEdited
	//		delete line
}

// Loads the contents of a text file off disk into the editor
//	The variable file contains a full path name 
//	Each line might have a \r before a \n, REMOVE the \r before
//	adding the line to your internal data structure
// RUNTIME: O(M + N) where M is the number of lines currently being edited and N is the number of lines
//		in the new file being loaded
// Opens specified file using C++ file I/O classes and reads its contents into my data structure(s)
bool StudentTextEditor::load(std::string file) 
{
	//	if specified file cannot be found
	//		do nothing, return false
	ifstream textFile(file);
	if (!textFile) // If the file opening fails
	{
		return false;
	}
	
	//	if already editing an eisting file and the specified file can be found
	//		reset old content of text editor
	reset();

	//	load new file, removing the \r's (text editor must contain contents of the new file)
	//	reset curRow and curCol to beginning of new file (0,0)
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
// Opens specified file using C++ file IO classes
// Writes the lines in the text editor to the file, overwriting any previous data in the file with new contents
// After each line append \n char
// RUNTIME: O(M), where M is the number of lines in the editor currently being edited
bool StudentTextEditor::save(std::string file) 
{
//	if file cannot be opened
//		return false
	ofstream saveFile(file);
	if (!saveFile) // If the file opening fails
	{
		return false;
	}

	auto iter = m_lines.begin();
	for (int i = 0; i < m_lines.size() && iter != m_lines.end(); i++)
	{	
		saveFile << *iter << endl;
		iter++;
	}
	return true;
//	otherwise succesfully save the lines in the editor to the file
//	return true
	
	return false;  // TODO
}

// Clears text editor's contents and resets the editing position to the top of the file
// After done, there should be no text in the editor
// Reset undo state, so no possible undos are possible after the reset 
// RUNTIME: O(N), where N is the number of rows being edited 
void StudentTextEditor::reset() 
{
	// clear text editor's contents
	// curRow = 0;
	// curCol = 0;
	// reset undo state
	while (!m_lines.empty())
	{
		m_lines.pop_back();
	}
	m_curRow = 0;
	m_curCol = 0;
	m_curRowIter = m_lines.begin();
	if(getUndo() != nullptr)
		getUndo()->clear();
	// TODO
}

// Adjusts current editing position
void StudentTextEditor::move(Dir dir) 
{
	// TODO
	size_t newLineSize;
	switch (dir)
	{
	case Dir::UP:
		if (m_curRow == 0)
			return;
		m_curRowIter--;
		newLineSize = (*m_curRowIter).size();
		if (newLineSize < m_curCol)
			m_curCol = newLineSize;
		m_curRow--;
		break;
	
	case Dir::DOWN:
		if (m_curRow == m_lines.size() - 1 || m_curRowIter == m_lines.end())
			return;
		m_curRowIter++;
		newLineSize = (*m_curRowIter).size();		// repositions cursor if the line below is
		if (newLineSize < m_curCol)					// shorter than the one above, it starts at the end
			m_curCol = newLineSize;					// of the shorter line
		m_curRow++;
		break;

	case Dir::LEFT:
		// if editing position is already on the first character of the line
		//	move cursor to AFTER the last character on the previous line
		if (m_curRow == 0 && m_curCol == 0)
			return;
		
		if (m_curCol == 0)
		{
			m_curRow--;
			m_curRowIter--;
			m_curCol = (*m_curRowIter).size();
		}
		else 
			m_curCol--;
		break; 
	
	case Dir::RIGHT:
		// cursor just after last leter of last line, do nothing
		if (m_curRow == m_lines.size() - 1 && m_curCol == (*m_curRowIter).size())
			return;

		if (m_curCol == (*m_curRowIter).size())
		{
			m_curRow++;
			m_curRowIter++;
			m_curCol = 0;
		}
		else
			m_curCol++;
		break;
	
	case Dir::HOME:
		m_curCol = 0;
		break;
	
	case Dir::END:
		m_curCol = (*m_curRowIter).size();
	}
}

// RUNTIME: 
//	Char in MIDDLE of line: O(L), where L is the length of the line containting the current editing position
//	Deletion at END, result in a merge: O(L1 + L2), where L1 is the length of the current line of text and
//  L2 is the length of the next line in the editor
// This function must NOT have a runtime that depends on the number of lines being edited (M)
void StudentTextEditor::del() 
{
	if (m_curCol != (*m_curRowIter).size())
	{
		char ch = (*m_curRowIter).at(m_curCol);
		(*m_curRowIter).erase((*m_curRowIter).begin() + m_curCol);
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

void StudentTextEditor::backspace() 
{
	if (m_curCol == 0 && m_curRow == 0)
		return;

	if (m_curCol > 0)
	{
		char ch = (*m_curRowIter).at(m_curCol - 1);
		(*m_curRowIter).erase((*m_curRowIter).begin() + m_curCol - 1);	// delete character just before cursor
		m_curCol--;		// move cursor left
		getUndo()->submit(Undo::Action::DELETE, m_curRow, m_curCol, ch);	// QUESTIONABLE
	}
	else if (m_curCol == 0)
	{
		auto iter = m_curRowIter;
		string lineBelow = *iter;
		iter--;
		m_curCol = (*iter).size();
		(*iter).append(lineBelow);
		m_curRow--;
		m_curRowIter--;
		auto iterErase = m_curRowIter;
		iterErase++;
		m_lines.erase(iterErase);
		getUndo()->submit(Undo::Action::JOIN, m_curRow, m_curCol);
	}
	
}

// RUNTIME: O(L), where L is the length of the line containing the current editing position
void StudentTextEditor::insert(char ch) 
{
	if (ch == '\t')
	{
		(*m_curRowIter).insert(m_curCol, "    ");
		m_curCol += 4;
	}
	else
	{
		(*m_curRowIter).insert((*m_curRowIter).begin() + m_curCol, ch);
		m_curCol++;
	}
	getUndo()->submit(Undo::Action::INSERT, m_curRow, m_curCol, ch);
}

// RUNTIME: O(L), where L is the length of the line containing the current editing position
// This function must NOT have a runtime that depends on the number of lines being edited (M)
void StudentTextEditor::enter() 
{
	getUndo()->submit(Undo::Action::SPLIT, m_curRow, m_curCol);
	// Split current line into two parts
	string beforeEnter = (*m_curRowIter).substr(0, m_curCol);
	string afterEnter = (*m_curRowIter).substr(m_curCol);
	*m_curRowIter = beforeEnter;
	auto iter = m_curRowIter;
	//want to insert at row just after current
	iter++;
	m_lines.insert(iter, afterEnter);
	m_curRow++;
	m_curRowIter++;
	m_curCol = 0;
}

// RUNTIME: O(1), runtime should not depends on the number of lines being edited or the length of the current line
void StudentTextEditor::getPos(int& row, int& col) const 
{
	row = m_curRow;
	col = m_curCol;
}

// RUNTIME: O(abs(curRow - startRow) + numRows*L)
int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const
{
	if (startRow < 0 || numRows < 0 || startRow > m_lines.size())
		return -1;
	
	int distance = abs(m_curRow - startRow);
	auto iter = m_curRowIter;
	while (distance > 0)
	{
		if (m_curRow < startRow)
			iter++;
		else
			iter--;
		distance--;
	}

	for (int i = 0; i != lines.size(); i++)
		lines.pop_back();

	if (startRow == m_lines.size())
		return 0;

	for (int j = 0; j < numRows && iter != m_lines.end(); j++)
	{
		lines.push_back(*iter);
		iter++;
	}
	return lines.size();// TODO
}

void StudentTextEditor::undo() 
{
	int count = 0;
	string text = "";
	int row = 0;
	int col = 0;
	int distance = 0;
	Undo::Action command = getUndo()->get(row, col, count, text);
	
	if (command == Undo::Action::ERROR)
		return;
	
	distance = abs(m_curRow - row);
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
	case Undo::Action::INSERT:
	/*
		m_curRow = row;
		m_curCol = col;
		(*m_curRowIter).insert(m_curCol, text);
		m_curCol++;*/
		break;
	case Undo::Action::DELETE:
		(*m_curRowIter).erase(m_curCol, count);
		cerr << count;
		cerr << m_curCol;
		break;
	case Undo::Action::SPLIT:
		break;
	case Undo::Action::JOIN:
		break;
	}

}
