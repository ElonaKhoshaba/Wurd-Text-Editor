#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"
#include <string>
#include <vector>

class StudentSpellCheck : public SpellCheck 
{
public:
    StudentSpellCheck() 
	{ 
		m_root = new TrieNode('\0');	// Root node is empty, dummy node
	}
	virtual ~StudentSpellCheck();		// Deletes all dynamically allocated trie nodes 
	
	//////////////////////
	// Public Functions //
	//////////////////////

	// Loads a new dictionary, deleting the old one if it exists
	bool load(std::string dict_file);

	// Checks to see if a given word is in the dictionary; if so, then its not misspelled.
	// Otherwise, it checks to see if every one-letter variant of the word would result in a 
	// valid word in the dictionary. If so, displays those suggestions to the user
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);

	// Finds all the positions of misspelled words (ie, those not present in the dictionary)
	// in a given line and places them in the problems vector
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
	////////////////////////////////////////
	// Trie Data Structure for Dictionary //
	////////////////////////////////////////
	struct TrieNode
	{
		TrieNode(char x)
		{
			ch = x;
		}
		char ch;
		TrieNode* children[27] = { nullptr }; // English alphabet + apostrophe
		bool isWord = false;
	};
	StudentSpellCheck::TrieNode* m_root;	  // Pointer to root (dummy node)

	//////////////////////////////
	// Private Helper Functions //
	//////////////////////////////

	// Inserts a new word into the trie
	void insert(StudentSpellCheck::TrieNode* curr, std::string line);
	
	// Searches to see if a given word is in the dictionary (case insensitive) by seeing
	// if following each char in the word results in a complete path in the trie, and if by
	// the end of said path the last char is denoted as a word
	bool search(std::string& word, StudentSpellCheck::TrieNode* root);
	
	// Returns a pointer to the node containing the last char of the string word, 
	// given a starting point to follow
	StudentSpellCheck::TrieNode* getNode(std::string word, StudentSpellCheck::TrieNode* root);
	
	// Removes all nodes in a given trie starting from the curr node
	void removeAll(StudentSpellCheck::TrieNode* node);
};

#endif  // STUDENTSPELLCHECK_H_
