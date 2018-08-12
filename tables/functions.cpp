#include "functions.h"

string decimal_to_hex(int n) {
	stringstream code;
	code << uppercase << hex << n;

	return code.str();
}

int hex_to_decimal(string hex) {
	int x;
	std::stringstream ss;
	ss << std::hex << hex;
	ss >> x;
	return x;
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

	code << uppercase << hex << stoll(bin, nullptr, 2);
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