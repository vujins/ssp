#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <sstream>

using namespace std;

string decimal_to_hex(int br);
int hex_to_decimal(string hex);
string dec_to_bin(int n, size_t bits);
string bin_to_hex(string bin);

string little_endian_from_hex(string hex, int multiplier);


#endif