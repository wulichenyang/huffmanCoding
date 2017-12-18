#include <string>
#include <iostream>
#include <fstream>
#include "Huffman.h"

using namespace std;

int main(int argc, char* argv[]) {
	// TEST
	Huffman e, d;
	
	e.encode("README");
	d.decode("README.huff", "UN_README");

	// RUN IN COMMAND 
	//Huffman h;

	//try {
	//	if (argc != 3 && argc != 4) throw "������������ȷ";
	//	if (strcmp(argv[1], "-c") == 0) {
	//		if (argc != 3) throw "������������ȷ";

	//		// ENCODE THE COMPRESSED FILE
	//		h.encode(argv[2]);
	//	}
	//	else if (strcmp(argv[1], "-u") == 0) {
	//		if (argc != 4) throw "������������ȷ";

	//		// DECODE THE COMPRESSED FILE
	//		h.decode(argv[2], argv[3]);
	//	}
	//	else {
	//		throw "��ѹ����������ȷ";
	//	}
	//}
	//catch (char* err) {
	//	cout << "\n\n" << err << ".\n\n";
	//	cout << "\t-ѹ��: -c filename" << endl;
	//	cout << "\t-��ѹ: -u source target" << endl << endl;
	//}

	system("pause");
	return 0;
}