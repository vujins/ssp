#ifndef EMULATOR_H
#define EMULATOR_H

#include <string>
#include <fstream>
#include <cassert>
#include <iostream>
#include <regex>

#include "config.h"
#include "OpCode.h"
#include "Section.h"
#include "Simbol.h"
#include "functions.h"

using namespace std;

#define OUTPUT_FILE "C:\\Users\\vana\\Documents\\ssp\\tests\\ins\\emulator.txt"

class Emulator {
public:
	Emulator(int argc, char* argv[]);
	~Emulator();

	void read();
	void resolve_conflict();
	void execute();

	void output();

protected:
	int16_t get_operand(uint8_t op);
	void store_result(uint8_t op, int16_t result);

	//setters
	void sti();
	void stz();
	void sto();
	void stc();
	void stn();
	void stperiodic();
	//clear
	void cli();
	void clz();
	void clo();
	void clc();
	void cln();
	void clperiodic();
	//getters
	bool get_i();
	bool get_z();
	bool get_o();
	bool get_c();
	bool get_n();
	bool get_periodic();

	void check_address(uint16_t addr);
	void write(uint16_t addr, uint8_t data);
	void write(uint16_t addr, int data, int bytes);
	//little-endian
	int read(uint16_t address);
	uint16_t read_ins(uint16_t &address);

	void push(uint8_t data);
	uint8_t pop();


private:
	vector<string> files;

	uint8_t om[OM_SIZE];

	uint16_t r[8];
	uint16_t PSW;
	uint16_t end_address;

	OpCodeTable table_opcode;
	vector<SectionTable*> table_section;
	SimbolTable table_simbol;
	ReallocationTable table_reallocation;
};

#endif