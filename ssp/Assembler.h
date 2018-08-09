#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <exception>

#include "Section.h"
#include "Simbol.h"
#include "OpCode.h"
#include "Reallocation.h"

//TODO promeni define kad budes radio javni test
#define OUTPUT_FILE "C:\\Users\\vana\\Documents\\ssp\\tests\\output.txt"

class Assembler {
public:
	Assembler(const char *file, int start_address_);
	~Assembler();

	void first_pass();
	void second_pass();

	void output();

protected:
	void reset();
	bool is_comment(string line);
	string is_simbol(string operand, string &reallocation_type);
	bool increase_location_counter(string line);
	bool add_reallocation(Simbol *simbol, int offset, string type);

	string little_endian_from_hex(string hex, int multiplier);
	string bin_to_hex(string bin);
	string get_instruction_code(string line);
	string get_operand_code(string operand, string &result);
	string get_directive_code(string line);

private:
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