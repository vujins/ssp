#include <iostream>
#include <exception>

#include "Assembler.h"

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
		cout << "Assembly completed!" << endl;
	}
	else {
		regex instruction("((?:eq|ne|gt|al)[a-z]{2,4}) ?([^,]*),? ?([^,]*)");
		string op = "eqmov r1, r2";
		smatch smatch;
		if (regex_match(op, smatch, instruction)) {
			for (auto it : smatch)
			cout << it.str() << endl;
		}
		else {
			cout << "regex failed" << endl;
		}

		string ins = "neret";
		if (regex_match(ins, regex("^(eq|ne|gt|al)ret"))) {
			cout << "dobar regex" << endl;
		}

	}


	return 0;
}

