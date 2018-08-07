#ifndef OPCODE_H
#define OPCODE_H

#include <string>
#include <unordered_map>
#include <regex>
#include <sstream>

using namespace std;

class OpCode {
public:
	OpCode(string name_, string opcode_);
	~OpCode();

	const string get_name() const;
	const string get_opcode() const;

	static int length_of_directive(string line, int lc);
	static int length_of_operation(string line);

	static bool is_global(string line);
	static bool is_skip(string line);
	static bool is_align(string line);
	static bool is_directive(string line);
	static bool is_instruction(string line);

	static string get_skip_code(string line);
	static string get_align_code(string line, int lc);
	//static string get_directive_code(string line);

	static string decimal_to_hex(int br);
	static string dec_to_bin(int n, size_t bits);

private:
	friend class OpCodeTable;
	friend class Assembler;

	const string name;
	const string opcode;

	static regex regex_char, regex_word, regex_long, regex_skip, regex_align, regex_directive, regex_global;
	static regex regex_comma, regex_registers;
	static regex regex_operation, regex_no_operands;
	//adresiranja
	static regex regex_register, regex_immediate, regex_mem_dir, regex_reg_ind;
};

class OpCodeTable {
public:
	OpCodeTable();
	~OpCodeTable();

	OpCode* get_opcode(string key);

private:
	unordered_map<string, OpCode*> table;

	regex regex_alfanum;
};

#endif