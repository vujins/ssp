#include <iostream>
#include <exception>
#include <sstream>

#include "Assembler.h"

using namespace std;

string decimal_to_hex(int br) { //provereno
	string hexbr;
	int rem;
	if (br == 0) return "0";
	while (br> 0) {
		rem = br % 16;
		switch (rem)
		{
		case 10: hexbr = 'A' + hexbr; break;
		case 11: hexbr = 'B' + hexbr; break;
		case 12: hexbr = "C" + hexbr; break;
		case 13: hexbr = "D" + hexbr; break;
		case 14: hexbr = "E" + hexbr; break;
		case 15: hexbr = "F" + hexbr; break;
		default: hexbr = to_string(rem) + hexbr;
		}
		br /= 16;
	}
	return hexbr;
}

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
		string hex = decimal_to_hex(1675);
		cout << "hex = " << hex << endl;
		stringstream le;
		int multiplier = 4;
		if (hex.size() > 2 * multiplier) cout << "error" << endl;
		for (int i = 0; i < 2*multiplier; i++) {
			if (i < hex.size()) le << hex[hex.size() - i - 1];
			else le << "0";
		}
		string result;
		le >> result;
		cout << result << endl;
	}


	return 0;
}

