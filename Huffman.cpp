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

	cout << "统计词频中..." << endl;

	while (!infile.eof()) {
		infile.read(buf, BUF_SIZE);
		streamsize n = infile.gcount();
		for (size_t i = 0; i < n; i++)
		{
			m_it = tempMap->find(buf[i]);
			if (m_it == tempMap->end()) {
				tempMap->insert(make_pair(buf[i], 1));
				//(*(tempParam->tempMap))[c] = 1;
				// 字符种数
				typeNum++;
			}
			else {
				m_it->second++;
				/*(*(tempParam->tempMap))[c]++;*/
			}
		}
	}
	// 清空 eofbit flag 允许重新操作文件流
	infile.clear();

	cout << "统计完毕" << endl;
	cout << "typeNum = " << typeNum << endl;

	return tempMap;
}

void Huffman::buildTree() {
	map<char, int>::iterator m_it = frequencies->begin();

	// vector 排序保存临时节点
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

	cout << endl << "压缩中..." << endl;
	while (!infile.eof()) {
		infile.read(buf, BUF_SIZE);
		streamsize n = infile.gcount();

		// 读buf的字符按查表将code写入目的文件
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
		// 剩余不足BIT_NUM位的code写入文件
		if (n < BUF_SIZE && tempSize > 0) {
			reverse(code.begin(), code.end());
			bitset<BIT_NUM> bitvecLast(code);
			outfile.write((char*)&bitvecLast, BITSET_SIZE);
		}
	}
	infile.clear();
	cout << endl << "压缩完毕！" << endl << endl;
}

void Huffman::writeToFile(ifstream &infile, string filename) {
	// 打开压缩目的文件
	string compressedFile = filename + ".huff";
	ofstream outfile(compressedFile.c_str(), ios::binary);

	if (!outfile) {
		cout << "error in open " << compressedFile << endl;
		exit(0);
	}

	getRawData();
	// 写入字符种类数、不满BIT_NUM位的位数、bitset<BIT_NUM>个数
	outfile.write((char*)&typeNum, 4);
	outfile.write((char*)&padding, 4);
	outfile.write((char*)&bitsetNum, 4);
	// 写入词频表
	map<char, int>::iterator m_it = frequencies->begin();
	while (m_it != frequencies->end()) {
		outfile.write(&(m_it->first), 1);
		outfile.write((char*)&(m_it->second), 4);
		cout << "write: " << m_it->first << " " << m_it->second << endl;
		m_it++;
	}
	// 写入哈夫曼编码数据
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

	// 保存词频
	frequencies = getFrequencies(infile);
	// 将所有树节点生成哈夫曼树
	buildTree();
	// 生成哈夫曼编码映射表
	buildCharCodeMap();
	// 读取源文件，将对应编码表读文件并将字符数、压缩编码总位数、频率映射表、压缩编码写入压缩文件
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

	// 处理bitset<BIT_NUM>整数倍的数据
	cout << endl << "解压中..." << endl;
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
	// 剩余不足BUF_SIZE的字符写入解压目的文件
	outfile.write(buf, j);
	j = 0;

	// 处理剩下不足BIT_NUM位的数据
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
	// 剩余不足BUF_SIZE的字符写入解压目的文件
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
	cout << endl << "解压完成！" << endl << endl;
}
