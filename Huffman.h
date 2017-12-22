#ifndef HUFFMAN_H
#define HUFFMAN_H

// ---------- INCLUDES ------------------------------------------------------------------
#include <map>
#include <vector>
#include <string>
#include "Node.h"
#include <pthread.h>
using namespace std;

// ---------- HUFFMAN CLASS -------------------------------------------------------------
class Huffman {
private:
	// ---------- VARIABLES -------------------------------------------------------------
	// 保存文件长度
	unsigned int size;
	// 保存词频
	map<char, int> *frequencies;
	// 保存临时根节点
	vector<Node *> huffmanNode_v;
	// 保存构造好的树根节点
	Node* tree;
	// 保存字符-编码映射表
	map<char, string> *codeMap;
	// 保存编码-字符映射表
	map<string, char> *charMap;
	// 字符种类数（MAX = 256）
	int typeNum;
	// 不足BIT_NUM位的最后bit数
	int padding;
	// 编码后的 bitset<BIT_NUM> 个数（与原文件字符数不同）
	unsigned int bitsetNum;
	// ---------- PRIVATE FUNCTIONS -----------------------------------------------------
	map<char, int> *getFrequencies(ifstream &infile);

	// ENCODE

	void buildTree();
	void buildCharCodeMap();
	void getRawData();
	void writeHuffmanData(ifstream &infile, ofstream &outfile);
	void writeToFile(ifstream &infile, string outfile);

	// DECODE
	void readFrequencies(ifstream &infile);
	void readRawData(ifstream &infile);
	void buildCodeCharMap();
	void writeToUncompressed(ifstream &infile, ofstream &outfile);

public:
	// ---------- PUBLIC FUNCTIONS ------------------------------------------------------
	Huffman();
	~Huffman();

	void encode(string filename);
	void decode(string filename, string uncompFilename);
};

#endif