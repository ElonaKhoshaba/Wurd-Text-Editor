#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

StudentSpellCheck::~StudentSpellCheck() 
{
	// TODO
}

bool StudentSpellCheck::load(std::string dictionaryFile)
{
	ifstream dictionary(dictionaryFile);
	if (!dictionary) // If the file opening fails
	{
		return false;
	}

	TrieNode* curr = m_root;
	string line;
	while (getline(dictionary, line))
	{
		insert(curr, line);
	}
	return true;
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) 
{
	if (search(word, m_root))	// Runs in 2L time
		return true;

	suggestions.clear();	// Runs in maxSuggestions 

	for (int i = 0; i < word.size(); i++)
	{ // L*2L*27 ~ L^2
		string temp = word;
		for (int j = 0; j < 27; j++)
		{	// 2L*27
			if (j == 26)
				temp[i] = '\'';
			else
				temp[i] = j + 'a';

			if (search(temp, m_root))		// Runs in 2L
				suggestions.push_back(temp);
		}
		temp = word;
	}
	return false; // TODO
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) 
{
//	problems.clear();
	int i = 0;
	size_t start = 0;
	size_t end = 0;

	while (i < line.size())
	{
		if (!isalpha(tolower(line[i])) && line[i] != '\'')
		{
			end = i;
			// new pos= start,end-1
			if (start != 0)
				start++;

			if (end > start) // Executes number of words in line - 1 (W -1)
			{
				string temp = line.substr(start, end - start);	// L
				if (!search(temp, m_root))		// 2L
				{
					Position p; p.start = start; p.end = end - 1;
					problems.push_back(p);
				}
			}
			start = end;
		}
		i++;
	}
	if (i == line.size())
	{
		end = line.size();
		if (end > start + 1) // Last word in message, executes once
		{
			string temp = line.substr(start + 1, end - start);
			if (!search(temp, m_root)) 
			{
				Position p; p.start = start + 1; p.end = end - 1;
				problems.push_back(p);
			}
		}
	}
}

void StudentSpellCheck::insert(StudentSpellCheck::TrieNode* curr, string line)
{
	for (int i = 0; i < line.size(); i++)
	{
		char ch = line.at(i);
		ch = tolower(ch);
		if (isalpha(ch))
		{
			if (curr->children[ch - 'a'] == nullptr)
				curr->children[ch - 'a'] = new TrieNode(ch);
			curr = curr->children[ch - 'a'];
		}
		else if (ch == '\'')
		{
			if (curr->children[26] == nullptr)
				curr->children[26] = new TrieNode(ch);
			curr = curr->children[26];
		}
		else
			continue;
	}
	curr->isWord = true;
}

bool StudentSpellCheck::search(string word, StudentSpellCheck::TrieNode* root)
{
	for (int i = 0; i < word.size(); i++)
	{
		word[i] = tolower(word[i]);
	}
	TrieNode* node = getNode(word, root);
	return node != nullptr && node->isWord;
}

StudentSpellCheck::TrieNode* StudentSpellCheck::getNode(string word, StudentSpellCheck::TrieNode* root)
{
	TrieNode* curr = root;
	for (int i = 0; i < word.size(); i++)
	{
		char ch = word.at(i);
		if (isalpha(ch))
		{
			if (curr->children[ch - 'a'] == nullptr)
				return nullptr;
			curr = curr->children[ch - 'a'];
		}
		else if (ch == '\'')
		{
			if (curr->children[26] == nullptr)
				return nullptr;
			curr = curr->children[26];
		}
		else
			return nullptr;	// word contains something other than alpha or '

	}
	return curr;
}