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

class Emulator {
public:
	Emulator(int argc, char* argv[]);
	~Emulator();

	void read();
	void resolve_conflict();
	void output();

protected:
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

	void write(uint16_t addr, uint8_t data);
	void write(uint16_t addr, int data, int bytes);
	int read(uint16_t address, int bytes);

private:
	vector<string> files;
	//od adrese 0 nalazi se IVT sa 8 ulaza od po 2 bajta
	//poslednjih 128 bajtova rezervisano za periferije
	uint8_t om[OM_SIZE];
	uint16_t r[8];
	uint16_t PSW;

	OpCodeTable table_opcode;
	vector<SectionTable*> table_section;
	SimbolTable table_simbol;
	ReallocationTable table_reallocation;
};

#endif