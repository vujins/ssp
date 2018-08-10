#include <iostream>
#include <exception>
#include <cassert>

#include "Assembler.h"
#include "Emulator.h"

using namespace std;

//argc[1] = putanja do fajla
//argc[2] = pocetna adresa
int main(int argc, char* argv[]) {

	const char *file = argv[1];
	int start_address = atoi(argv[2]);

	if (0) {
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
		Emulator emulator(OUTPUT_FILE);
		emulator.write(addr_cout, 65);
		emulator.write(addr_cout, 0x10);
	}


	return 0;
}

