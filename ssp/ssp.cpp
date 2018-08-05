#include <iostream>
#include <exception>

#include "Assembler.h"
#include <regex>

using namespace std;

//argc[1] = putanja do fajla
//argc[2] = pocetna adresa
int main(int argc, char* argv[]) {

	const char *file = argv[1];
	int start_address = atoi(argv[2]);

	if (1) {
		try {
			Assembler assembler(file, start_address);
			assembler.first_pass();
			assembler.second_pass();
			assembler.output();
		}
		catch (exception &e) {
			cout << e.what() << endl;
			return 0;
		}
	}
	else {
		string line = ".global a,straja,lab2";
		regex regex("lab2$");
		if (regex_search(line, regex))
			cout << "true" << endl;
		
	}
	cout << "Assembly completed!" << endl;

	return 0;
}

