#include "functions.h"

string decimal_to_hex(int n) {
	string hexbr;
	int rem;
	if (n == 0) return "0";
	while (n > 0) {
		rem = n % 16;
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
		n /= 16;
	}
	return hexbr;
}

string dec_to_bin(int n, size_t bits) {
	string result;
	for (int i = 0; i < bits; i++) {
		result = (char)((n & 1) + '0') + result;
		n >>= 1;
	}
	return result;
}

string bin_to_hex(string bin) {
	stringstream code;
	for (size_t i = 0; (bin.substr(i * 4, 4)).compare("0000") == 0; i++)
		code << "0";

	code << hex << stoll(bin, nullptr, 2);
	return code.str();
}


string little_endian_from_hex(string hex, int multiplier) {
	stringstream code;

	for (int i = 1; i <= multiplier; i++) {
		if (((int)hex.size() - 2 * i) >= 0) code << hex[hex.size() - 2 * i];
		else code << "0";
		if (((int)hex.size() - 2 * i + 1) >= 0) code << hex[hex.size() - 2 * i + 1];
		else code << "0";
	}

	return code.str();
}