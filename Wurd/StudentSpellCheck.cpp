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
	removeAll(m_root);
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

bool StudentSpellCheck::spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions) 
{
	if (search(word, m_root))	// Search runs in O(2L) time
		return true;

	suggestions.clear();	// Runs in O(maxSuggestions) time

	for (int i = 0; i < word.size(); i++) // Runs L*O(L) times
	{ 
		string temp = word;	// Temp has a length of L
		for (int j = 0; j < 27; j++)	// Runs 27*O(2L) => O(L)
		{	
			if (j == 26)
				temp[i] = '\'';
			else
				temp[i] = j + 'a';

			if (suggestions.size() != maxSuggestions && search(temp, m_root)) // Search runs in O(2L) time
				suggestions.push_back(temp);
		}
		temp = word;
	}
	return false;
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) 
{
	int start = 0;
	int prevStart = -1;
	int end = 0;
	int count = 0;
	vector<Position> positions;
	vector<string> words;
	string temp; 
	for (int i = 0; i < line.size(); i++) // Runs linear to length of line O(S)
	{
		if (isalpha(line[i]) || line[i] == '\'')
		{
			if (temp == "")
				start = i;
			temp.push_back(line[i]);
		}
		else
		{
			if (i != 0 && temp != "")
				words.push_back(temp);
			end = i;
			temp = "";
		}
		if (start < end && start != prevStart) // Doesn't repeat previous invalid words
		{
			if (!isalpha(line[start]) && line[start] != '\'')	// Invalid word, don't want the position
				continue;
			prevStart = start;
			Position p; p.start = start; p.end = end - 1;
			positions.push_back(p);
		}
	}
	// Check if last word is valid
	if (!temp.empty() && (isalpha(temp[temp.size() - 1]) || temp[temp.size() - 1] == '\''))
	{
		words.push_back(temp);
		Position p; p.start = start; p.end = line.size() - 1;
		positions.push_back(p);
	}

	for (int i = 0; i < words.size(); i++)	// Runs linear to number of words in the line 2L*O(W) => O(W*L)
	{
		if (!search(words[i], m_root)) // Search runs in O(2L) where L is the length of the word being searched for
			problems.push_back(positions[i]);
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

void StudentSpellCheck::removeAll(StudentSpellCheck::TrieNode* curr)
{
	if (curr == nullptr)
		return;
	for (int i = 0; i < 27; i++)
	{
		removeAll(curr->children[i]);

	}
	delete curr;
}