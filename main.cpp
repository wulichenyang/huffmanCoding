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
	//	if (argc != 3 && argc != 4) throw "参数个数不正确";
	//	if (strcmp(argv[1], "-c") == 0) {
	//		if (argc != 3) throw "参数个数不正确";

	//		// ENCODE THE COMPRESSED FILE
	//		h.encode(argv[2]);
	//	}
	//	else if (strcmp(argv[1], "-u") == 0) {
	//		if (argc != 4) throw "参数个数不正确";

	//		// DECODE THE COMPRESSED FILE
	//		h.decode(argv[2], argv[3]);
	//	}
	//	else {
	//		throw "解压缩参数不正确";
	//	}
	//}
	//catch (char* err) {
	//	cout << "\n\n" << err << ".\n\n";
	//	cout << "\t-压缩: -c filename" << endl;
	//	cout << "\t-解压: -u source target" << endl << endl;
	//}

	system("pause");
	return 0;
}