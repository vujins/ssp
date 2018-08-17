#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <exception>

#include "Section.h"
#include "Simbol.h"
#include "OpCode.h"
#include "Reallocation.h"
#include "functions.h"

class Assembler {
public:
	Assembler(const char *file, int start_address_);
	~Assembler();

	void first_pass();
	void second_pass();

	void output();

protected:
	void remove_line_ending(string &line);
	void reset();
	bool is_comment(string line);
	string is_simbol(string operand, string &reallocation_type);
	bool increase_location_counter(string line);
	bool add_reallocation(Simbol *simbol, int offset, string type);

	string get_instruction_code(string line);
	string get_operand_code(string operand, string &result);
	string get_directive_code(string line);



private:
	string file_name;
	int start_address, location_counter;
	Section *current_section;
	OpCodeTable table_opcode;
	SectionTable table_section;
	SimbolTable table_simbol;

	ifstream input_filestream;
	ofstream output_filestream;

	regex end, comment;
};

#endif