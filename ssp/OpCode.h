#ifndef OPCODE_H
#define OPCODE_H

#include <string>
#include <unordered_map>
#include <regex>

using namespace std;

class OpCode {
public:
	OpCode(string name_, string opcode_);
	~OpCode();

	const string get_name() const;
	const string get_opcode() const;

	static int length_of_directive(string line, int lc);
	static int length_of_operation(string line);

private:
	const string name;
	const string opcode;

	static regex regex_char, regex_word, regex_long, regex_skip, regex_align;
	static regex regex_comma, regex_registers, regex_register;
	static regex regex_operation, regex_no_operands;
};

class OpCodeTable {
public:
	OpCodeTable();
	~OpCodeTable();

private:
	unordered_map<string, OpCode*> table;

};

#endif