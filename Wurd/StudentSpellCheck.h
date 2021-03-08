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
		m_root = new TrieNode('\0');	// root node is empty
	}
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
	struct TrieNode
	{
		TrieNode(char x)
		{
			ch = x;
		}
		~TrieNode()
		{
			for (int i = 0; i < 27; i++)
				delete children[i];
		}
		char ch;
		TrieNode* children[27] = { nullptr }; // English alphabet + apostrophe
		bool isWord = false;
	};
	StudentSpellCheck::TrieNode* m_root;

	void insert(StudentSpellCheck::TrieNode* curr, std::string line);
	bool search(std::string word, StudentSpellCheck::TrieNode* root);
	StudentSpellCheck::TrieNode* getNode(std::string word, StudentSpellCheck::TrieNode* root);
	void removeAll(StudentSpellCheck::TrieNode* node);
};

#endif  // STUDENTSPELLCHECK_H_
