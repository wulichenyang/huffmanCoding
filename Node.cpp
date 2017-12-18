#include <string.h>
#include <iostream>
#include <fstream>
#include "Node.h"

using namespace std;

Node::Node() {
	data = NULL;
	code = "";
	count = 0;
	left = NULL;
	right = NULL;
}


Node::Node(char d, int c, NODE_TYPE t)
{
	data = d;
	code = "";
	count = c;
	type = t;
	left = NULL;
	right = NULL;
}
Node::Node(Node *l, Node *r, NODE_TYPE t)
{
	data = NULL;
	code = "";
	count = l->count + r->count;
	type = t;
	left = l;
	right = r;
}

Node::~Node(void) {
	delete left;
	delete right;
}

void Node::appendCode(string str) {
	code.append(str);
}


int Node::getCount() {
	return count;
}

char Node::getData() {
	return data;
}
string Node::getCode() {
	return code;
}

bool Node::hasChild() {
	return left != NULL || right != NULL;
}

// 递归遍历生成char_code_map
void Node::travelTree(Node *node, map<char, string> *codeMap) {
	if (this->hasChild()) {
		if (left != NULL) {
			node->left->appendCode(node->getCode());
			node->left->appendCode("0");
			left->travelTree(node->left, codeMap);
		}
		if (right != NULL) {
			node->right->appendCode(node->getCode());
			node->right->appendCode("1");
			right->travelTree(node->right, codeMap);
		}
	}
	else {
		// 访问到叶子节点停止循环写入编码
		cout << this->getData() << " -> " << this->getCode() << endl;
		codeMap->insert(make_pair(this->getData(), this->getCode()));
	}
}
// 递归遍历生成code_char_map
void Node::travelTree(Node *node, map<string, char> *charMap) {
	if (this->hasChild()) {
		if (left != NULL) {
			node->left->appendCode(node->getCode());
			node->left->appendCode("0");
			left->travelTree(node->left, charMap);
		}
		if (right != NULL) {
			node->right->appendCode(node->getCode());
			node->right->appendCode("1");
			right->travelTree(node->right, charMap);
		}
	}
	else {
		// 访问到叶子节点停止循环写入编码
		cout << this->getData() << "<- " << this->getCode() << endl;
		charMap->insert(make_pair(this->getCode(), this->getData()));
	}
}


// 该函数还有问题，通过huffman code遍历到叶子节点得到字符
Node *Node::travelTreeWrite(Node *node, Node *root, ifstream &infile, ofstream &outfile, int &pos, bitset<32> &bitvec, int &i, char &c, unsigned int bitsetNum) {
	if (pos == 0) {
		infile.read((char*)&bitvec, 4);
		//
		//for (int i = 0; i < 32; i++) {
		//	cout << bitvec[i];
		//}
		//cout << endl;

		i++;
	}
	if (pos == 32) {
		pos = 0;
		if (i == bitsetNum) {
			return node;
		}
		travelTreeWrite(node, root, infile, outfile, pos, bitvec, i, c, bitsetNum);
	}
	if (!node->hasChild()) {
		c = node->getData();
		cout << c;
		outfile.write(&c, 1);
		node = root;
		travelTreeWrite(node, root, infile, outfile, pos, bitvec, i, c, bitsetNum);
	}
	else {
		if (bitvec[pos] == 0) {
			if (node->left != NULL) {
				pos++;
				travelTreeWrite(node->left, root, infile, outfile, pos, bitvec, i, c, bitsetNum);
			}
		}
		else if (bitvec[pos] == 1) {
			if (node->right != NULL) {
				pos++;
				travelTreeWrite(node->right, root, infile, outfile, pos, bitvec, i, c, bitsetNum);
			}
		}

	}
}



