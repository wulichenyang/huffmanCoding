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
	// �����ļ�����
	unsigned int size;
	// �����Ƶ
	map<char, int> *frequencies;
	// ������ʱ���ڵ�
	vector<Node *> huffmanNode_v;
	// ���湹��õ������ڵ�
	Node* tree;
	// �����ַ�-����ӳ���
	map<char, string> *codeMap;
	// �������-�ַ�ӳ���
	map<string, char> *charMap;
	// �ַ���������MAX = 256��
	int typeNum;
	// ����BIT_NUMλ�����bit��
	int padding;
	// ������ bitset<BIT_NUM> ��������ԭ�ļ��ַ�����ͬ��
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