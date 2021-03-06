#include <iostream>
#include <exception>

#include "Assembler.h"

using namespace std;

//argc[1] = putanja do fajla
//argc[2] = pocetna adresa
int main(int argc, char* argv[]) {

	if (1) {
		try {
			const char *file;
			int start_address;
			if (argv[1]) file = argv[1];
			else throw invalid_argument("Wrong file path!");
			if (argv[2]) start_address = atoi(argv[2]);
			else throw invalid_argument("Wrong starting address!");

			Assembler assembler(file, start_address);
			assembler.first_pass();
			assembler.second_pass();
			assembler.output();
		}
		catch (exception &e) {
			cout << e.what() << endl;
			return 0;
		}
		cout << "Assembly completed!" << endl;
	}
	else {
		string line = ".data\t\t1100\t\t1120\t\t20";
		cout << line << endl;;
		smatch sm;
		if (regex_match(line, sm, regex("(\\.text|\\.data|\\.bss|\\.rodata)\t*([0-9]+)\t*([0-9]+)\t*([0-9]+)"))) {
			string name = sm[1].str();
			int start = stoi(sm[2].str());
			int end = stoi(sm[3].str());
			int length = stoi(sm[4].str());

			cout << name << " " << start << " " << end << " " << length << " " << endl;
		}
	}
	return 0;
}

