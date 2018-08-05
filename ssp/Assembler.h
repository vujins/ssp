#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <exception>

#include "Section.h"
#include "Simbol.h"
#include "OpCode.h"

//TODO promeni define kad budes radio javni test
#define OUTPUT_FILE "C:\\Users\\vana\\Documents\\ssp\\tests\\output.txt"

class Assembler {
public:
	Assembler(const char *file, int start_address_);
	~Assembler();

	void first_pass();
	void second_pass();

	void output();

private:
	void increase_location_counter(string line, int &location_counter, Section *current_section, int pass);

	int start_address;
	OpCodeTable table_opcode;
	SectionTable table_section;
	SimbolTable table_simbol;

	ifstream input_filestream;
	ofstream output_filestream;

	regex end;
};

#endif