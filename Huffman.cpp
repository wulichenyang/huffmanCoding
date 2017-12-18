#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <bitset>
#include "Huffman.h"

using namespace std;

// ---------- CONSTRUCTOR ---------------------------------------------------------------
Huffman::Huffman() {
	this->frequencies = NULL;
	this->tree = NULL;
	this->codeMap = NULL;
	this->charMap = NULL;
}

// ---------- DECONSTRUCTOR -------------------------------------------------------------
Huffman::~Huffman() {
	delete this->frequencies;
	delete this->tree;
	delete this->codeMap;
	delete this->charMap;
}

map<char, int> *Huffman::getFrequencies(ifstream &infile) {
	char c;
	map<char, int> *tempMap = new map<char, int>;
	map<char, int>::iterator m_it;
	int start, stop;
	typeNum = 0;

	infile.seekg(0, ios::beg);
	start = infile.tellg();
	infile.seekg(0, ios::end);
	stop = infile.tellg();
	infile.seekg(0, ios::beg);
	size = stop - start;

	cout << "ͳ�ƴ�Ƶ��..." << endl;
	for (int i = 0; i < size; i++) {
		infile.read(&c, 1);
		m_it = tempMap->find(c);
		// ����256 ���ַ���ֱ�ӽ���else�޸�Ƶ��
		if (typeNum != 256) {
			if (m_it == tempMap->end()) {
				(*tempMap)[c] = 1;
				// �ַ�����
				typeNum++;
			}
			else {
				(*tempMap)[c]++;
			}
		}
		else {
			(*tempMap)[c]++;
		}
	}
	cout << "typeNum = " << typeNum << endl;
	return tempMap;
}

void Huffman::buildTree() {
	map<char, int>::iterator m_it = frequencies->begin();

	// vector ���򱣴���ʱ�ڵ�
	while (m_it != frequencies->end()) {
		huffmanNode_v.push_back(new Node(m_it->first, m_it->second, LEAF));
		m_it++;
	}
	sort(huffmanNode_v.begin(), huffmanNode_v.end(), [](Node* a, Node* b) { return a->getCount() < b->getCount(); });

	// buildTree
	while (huffmanNode_v.size() != 1) {
		huffmanNode_v.push_back(new Node(huffmanNode_v.at(0), huffmanNode_v.at(1), ROOT));
		huffmanNode_v.erase(huffmanNode_v.begin(), huffmanNode_v.begin() + 2);
		sort(huffmanNode_v.begin(), huffmanNode_v.end(), [](Node* a, Node* b) { return a->getCount() < b->getCount(); });
	}
	tree = huffmanNode_v.front();
}

void Huffman::buildCharCodeMap() {
	codeMap = new map<char, string>;

	tree->travelTree(tree, codeMap);
}

void Huffman::getRawData() {
	unsigned int bitNum = 0;
	map<char, int>::iterator m_it = frequencies->begin();
	while (m_it != frequencies->end()) {
		bitNum += m_it->second * (*codeMap)[m_it->first].size();
		m_it++;
	}
	bitsetNum = bitNum / 32;
	padding = (char)(bitNum %= 32);
}

void Huffman::writeHuffmanData(ifstream &infile, ofstream &outfile) {
	infile.seekg(0, ios::beg);
	char c;
	string code;
	string temp;
	int count = 0;

	cout << endl << "ѹ����..." << endl;
	for (int i = 0; i < size; i++) {
		infile.read(&c, 1);
		code.append((*codeMap)[c]);
		count += (*codeMap)[c].size();
		if (count >= 32) {
			temp = code.substr(0, 32);
			reverse(temp.begin(), temp.end());
			bitset<32> bitvec(temp, 0, 32);
			code.erase(0, 32);
			outfile.write((char*)&bitvec, 4);
			count = code.size();
		}
		if (i == size - 1) {
			// ʣ�಻��32λ��codeд���ļ�
			if (code.size() != 0) {
				reverse(code.begin(), code.end());
				bitset<32> bitvecLast(code);
				outfile.write((char*)&bitvecLast, 4);
			}
		}
	}
	cout << endl << "ѹ����ϣ�" << endl << endl;
}

void Huffman::writeToFile(ifstream &infile, string filename) {
	// ��ѹ��Ŀ���ļ�
	string compressedFile = filename + ".huff";
	ofstream outfile(compressedFile.c_str(), ios::binary);

	if (!outfile) {
		cout << "error in open " << compressedFile << endl;
		exit(0);
	}


	getRawData();
	// д���ַ�������������32λ��λ����bitset<32>����
	outfile.write((char*)&typeNum, 4);
	outfile.write(&padding, 1);
	outfile.write((char*)&bitsetNum, 4);
	// д���Ƶ��
	map<char, int>::iterator m_it = frequencies->begin();
	while (m_it != frequencies->end()) {
		outfile.write(&(m_it->first), 1);
		outfile.write((char*)&(m_it->second), 4);
		cout << "write: " << m_it->first << " " << m_it->second << endl;
		m_it++;
	}
	// д���������������
	writeHuffmanData(infile, outfile);

	outfile.close();
}


// ---------- ENCODE STRING -------------------------------------------------------------
void Huffman::encode(string filename) {
	ifstream infile(filename.c_str(), ios::binary);
	int size;

	if (!infile) {
		cout << "error in open " << filename << endl;
		infile.close();
		system("pause");
		exit(0);
	}

	// �����Ƶ
	frequencies = getFrequencies(infile);
	// ���������ڵ����ɹ�������
	buildTree();
	// ���ɹ���������ӳ���
	buildCharCodeMap();
	// ��ȡԴ�ļ�������Ӧ�������ļ������ַ�����ѹ��������λ����Ƶ��ӳ���ѹ������д��ѹ���ļ�
	writeToFile(infile, filename);

	infile.close();
}

// ---------- DECODE STRING -------------------------------------------------------------

void Huffman::readFrequencies(ifstream &compFile) {
	frequencies = new map<char, int>;
	int count;
	char c;
	for (int i = 0; i < typeNum; i++) {
		compFile.read(&c, 1);
		compFile.read((char*)&count, 4);

		cout << "read: " << c << " " << count << endl;
		frequencies->insert(make_pair(c, count));
	}
}


void Huffman::readRawData(ifstream &compFile) {
	compFile.read((char*)&typeNum, 4);
	compFile.read(&padding, 1);
	compFile.read((char*)&bitsetNum, 4);

	readFrequencies(compFile);

	cout << "typeNum= " << (int)typeNum << "  padding= " << (int)padding;
	cout << " bitsetNum= " << bitsetNum << endl;
}
void Huffman::buildCodeCharMap() {
	charMap = new map<string, char>;

	tree->travelTree(tree, charMap);
}

void Huffman::writeToUncompressed(ifstream &infile, ofstream &outfile) {
	bitset<32> bitvec;
	string str;
	map<string, char>::iterator m_it;

	//Node *node = tree;
	//node = node -> travelTreeWrite(node, tree, infile, outfile, pos, bitvec, i, c, bitsetNum);

	// ����bitset<32>������������
	cout << endl << "��ѹ��..." << endl;
	for (size_t i = 0; i < bitsetNum; i++)
	{
		infile.read((char*)&bitvec, 4);
		for (size_t pos = 0; pos < bitvec.size(); pos++)
		{
			if (bitvec[pos] == 0) {
				str.append("0");
			}
			else {
				str.append("1");
			}
			m_it = charMap->find(str);
			if (m_it != charMap->end()) {
				outfile.write(&(m_it->second), 1);
				str = "";
			}
		}
	}

	// ����ʣ�²���32λ������
	infile.read((char*)&bitvec, 4);
	for (size_t pos = 0; pos < (int)padding; pos++)
	{
		if (bitvec[pos] == 0) {
			str.append("0");
		}
		else {
			str.append("1");
		}
		m_it = charMap->find(str);
		if (m_it != charMap->end()) {
			outfile.write(&(m_it->second), 1);
			str = "";
		}
	}
}

void Huffman::decode(string filename, string uncompFilename) {
	ifstream compFile(filename.c_str(), ios::binary);
	ofstream uncompFile(uncompFilename.c_str(), ios::binary);

	if (!compFile) {
		cout << "error in open " << filename << endl;
		compFile.close();
		system("pause");
		exit(0);
	}
	if (!uncompFile) {
		cout << "error in open " << uncompFilename << endl;
		uncompFile.close();
		system("pause");
		exit(0);
	}

	readRawData(compFile);
	buildTree();
	buildCodeCharMap();
	writeToUncompressed(compFile, uncompFile);


	compFile.close();
	uncompFile.close();
	cout << endl << "��ѹ��ɣ�" << endl << endl;
}
