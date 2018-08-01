#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <exception>

#include "Section.h"
#include "Simbol.h"
#include "OpCode.h"

#define OUTPUT_FILE "C:\\Users\\vana\\Desktop\\ss\\output.txt"

class Assembler {
public:
	Assembler(const char *file, int start_address);
	~Assembler();

	void first_pass();

	void output();

private:
	OpCodeTable table_opcode;
	SectionTable table_section;
	SimbolTable table_simbol;

	ifstream input_filestream;
	ofstream output_filestream;

	regex end;
};

#endif