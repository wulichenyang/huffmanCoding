#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <bitset>
#include <pthread.h>
#include "Huffman.h"
#define BUF_SIZE 1024
#define BIT_NUM 256
#define BITSET_SIZE 32

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
	map<char, int> *tempMap = new map<char, int>;
	int start, stop;
	typeNum = 0;
	char buf[1024];
	map<char, int>::iterator m_it;

	infile.seekg(0, ios::beg);
	start = infile.tellg();
	infile.seekg(0, ios::end);
	stop = infile.tellg();
	infile.seekg(0, ios::beg);
	size = stop - start;

	cout << "ͳ�ƴ�Ƶ��..." << endl;

	while (!infile.eof()) {
		infile.read(buf, BUF_SIZE);
		streamsize n = infile.gcount();
		for (size_t i = 0; i < n; i++)
		{
			m_it = tempMap->find(buf[i]);
			if (m_it == tempMap->end()) {
				tempMap->insert(make_pair(buf[i], 1));
				//(*(tempParam->tempMap))[c] = 1;
				// �ַ�����
				typeNum++;
			}
			else {
				m_it->second++;
				/*(*(tempParam->tempMap))[c]++;*/
			}
		}
	}
	// ��� eofbit flag �������²����ļ���
	infile.clear();

	cout << "ͳ�����" << endl;
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
	bitsetNum = bitNum / BIT_NUM;
	padding = (int)(bitNum %= BIT_NUM);
}

void Huffman::writeHuffmanData(ifstream &infile, ofstream &outfile) {
	infile.seekg(0, ios::beg);
	string code;
	string temp;
	int tempSize = 0;
	char buf[BUF_SIZE];

	cout << endl << "ѹ����..." << endl;
	while (!infile.eof()) {
		infile.read(buf, BUF_SIZE);
		streamsize n = infile.gcount();

		// ��buf���ַ������codeд��Ŀ���ļ�
		for (size_t i = 0; i < n; i++) {
			code.append((*codeMap)[buf[i]]);
			tempSize += (*codeMap)[buf[i]].size();
			if (tempSize >= BIT_NUM) {
				temp = code.substr(0, BIT_NUM);
				reverse(temp.begin(), temp.end());
				bitset<BIT_NUM> bitvec(temp, 0, BIT_NUM);
				code.erase(0, BIT_NUM);
				outfile.write((char*)&bitvec, BITSET_SIZE);
				tempSize = code.size();
			}
		}
		// ʣ�಻��BIT_NUMλ��codeд���ļ�
		if (n < BUF_SIZE && tempSize > 0) {
			reverse(code.begin(), code.end());
			bitset<BIT_NUM> bitvecLast(code);
			outfile.write((char*)&bitvecLast, BITSET_SIZE);
		}
	}
	infile.clear();
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
	// д���ַ�������������BIT_NUMλ��λ����bitset<BIT_NUM>����
	outfile.write((char*)&typeNum, 4);
	outfile.write((char*)&padding, 4);
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
	compFile.read((char*)&padding, 4);
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
	bitset<BIT_NUM> bitvec;
	string str;
	map<string, char>::iterator m_it;
	char buf[BUF_SIZE];
	size_t j = 0;

	//Node *node = tree;
	//node = node -> travelTreeWrite(node, tree, infile, outfile, pos, bitvec, i, c, bitsetNum);

	// ����bitset<BIT_NUM>������������
	cout << endl << "��ѹ��..." << endl;
	for (size_t i = 0; i < bitsetNum; i++)
	{
		infile.read((char*)&bitvec, BITSET_SIZE);
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
				if (j < BUF_SIZE) {
					buf[j++] = m_it->second;
					str = "";
				}
				else {
					outfile.write(buf, BUF_SIZE);
					j = 0;
					buf[j++] = m_it->second;
					str = "";
				}
			}
		}
	}
	// ʣ�಻��BUF_SIZE���ַ�д���ѹĿ���ļ�
	outfile.write(buf, j);
	j = 0;

	// ����ʣ�²���BIT_NUMλ������
	infile.read((char*)&bitvec, BITSET_SIZE);
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
			if (j < BUF_SIZE) {
				buf[j++] = m_it->second;
				str = "";
			}
			else {
				outfile.write(buf, BUF_SIZE);
				j = 0;
				buf[j++] = m_it->second;
				str = "";
			}
		}
	}
	// ʣ�಻��BUF_SIZE���ַ�д���ѹĿ���ļ�
	outfile.write(buf, j);
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
