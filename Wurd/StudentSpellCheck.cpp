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

// Deletes all dynamically allocated trie nodes 
StudentSpellCheck::~StudentSpellCheck() 
{
	removeAll(m_root);
}

// Loads a new dictionary, deleting the old one if it exists
bool StudentSpellCheck::load(std::string dictionaryFile)
{
	ifstream dictionary(dictionaryFile);
	if (!dictionary) // If the file opening fails
		return false;


	// Delete old dictionary and make a new one (Deletion runs in O(N))
	delete m_root;	
	m_root = new TrieNode('\0');

	// Load the new dictionary using the contexts of dictionaryFile
	TrieNode* curr = m_root;
	string line;
	while (getline(dictionary, line))
		insert(curr, line);

	return true;	// Dictionary successfully loaded
}

// Checks to see if a given word is in the dictionary; if so, then its not misspelled 
// If not, it checks to see if every one letter off variant of the word would result in a 
// valid word in the dictionary. If so, displays those suggestions to the user
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

// Finds all the positions of misspelled words in a given line
void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) 
{
	int start = 0;
	int prevStart = -1;
	int end = 0;
	int count = 0;
	vector<Position> positions;		// All possible positions of all words in line
	vector<string> words;			// All possible words (including ones spelled correctly)
	string temp; 
	for (int i = 0; i < line.size(); i++) // Runs linear to length of line O(S)
	{
		if (isalpha(line[i]) || line[i] == '\'')	// If valid char, add it to possible word
		{
			if (temp == "")
				start = i;				// If you've just started a new word, track starting position of it
			temp.push_back(line[i]);
		}
		else    // You've reached an invalid char, so the word has ended
		{
			if (i != 0 && temp != "")	
				words.push_back(temp);		// Add the word to the word vector
			end = i;						// and track its ending position
			temp = "";						// Reset the temp word to empty
		}
		// If a new VALID word has been tracked, add its position to the position vector
		if (start < end && start != prevStart)
		{
			if (!isalpha(line[start]) && line[start] != '\'')	// Invalid word, don't want the position
				continue;
			prevStart = start;
			Position p; p.start = start; p.end = end - 1;
			positions.push_back(p);
		}
	}

	// Check if last word is valid, and add it to both vectors if so
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

// Inserts a new word into the trie
void StudentSpellCheck::insert(StudentSpellCheck::TrieNode* curr, string line)
{
	for (int i = 0; i < line.size(); i++)
	{
		char ch = line.at(i);
		ch = tolower(ch);
		// Adds each character in word to tree only if needed, and advances the curr pointer in that direction
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
	curr->isWord = true;	// Denote the last reached position as a word
}

// Searches to see if a given word is in the dictionary (case insensitive) by seeing
// if following each char in the word results in a complete path in the trie, and if by
// the end of said path the last char is denoted as a word
bool StudentSpellCheck::search(string& word, StudentSpellCheck::TrieNode* root)
{
	for (int i = 0; i < word.size(); i++)
		word[i] = tolower(word[i]);
	TrieNode* node = getNode(word, root);
	return node != nullptr && node->isWord;
}

// Returns a pointer to the node containing the last char of the string word, given a starting
// point to follow
StudentSpellCheck::TrieNode* StudentSpellCheck::getNode(string word, StudentSpellCheck::TrieNode* root)
{
	TrieNode* curr = root;
	for (int i = 0; i < word.size(); i++)	
	{
		char ch = word.at(i);
		// Given a valid character in the word, check to see if the current node
		// has a child for said character, if so advance to the next char
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
			return nullptr;		// Word contains something other than alpha or '
	}
	return curr;
}


// Removes all nodes in a given trie starting from the curr node
void StudentSpellCheck::removeAll(StudentSpellCheck::TrieNode* curr)
{
	if (curr == nullptr)
		return;
	for (int i = 0; i < 27; i++)
		removeAll(curr->children[i]);
	delete curr;
}