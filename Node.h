#ifndef NODE_H
#define NODE_H

// ---------- INCLUDES ------------------------------------------------------------------
#include <map>
#include <vector>
#include <string>
#include <bitset>

using namespace std;

enum NODE_TYPE { LEAF, ROOT };

// ---------- NODE CLASS ----------------------------------------------------------------
class Node
{
private:
	NODE_TYPE type;
	int count;
	// �ַ�
	char data;
	// ����
	string code;
	Node *left;
	Node *right;

public:
	Node();
	Node(char d, int c, NODE_TYPE t);
	Node(Node *l, Node *r, NODE_TYPE t);
	~Node(void);

	void appendCode(string str);
	int getCount();
	char getData();
	string getCode();
	bool hasChild();
	// char_code_map
	void travelTree(Node *node, map<char, string> *map);
	// code_char_map
	void travelTree(Node *node, map<string, char> *map);

	// ����������
	Node *travelTreeWrite(Node *node, Node *root, ifstream &infile, ofstream &outfile, int &pos, bitset<32> &bitvec, int &i, char &c, unsigned int bitsetNum);

};

#endif
