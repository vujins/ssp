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

using namespace std;

class Emulator {
public:
	Emulator(int argc, char* argv[]);
	~Emulator();

	void read();
	void write();

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

	void write(uint16_t addr, int8_t data);

private:
	vector<string> files;
	//od adrese 0 nalazi se IVT sa 8 ulaza
	//poslednjih 128 bajtova rezervisano za periferije
	int8_t om[OM_SIZE];
	uint16_t r[8];
	uint16_t PSW;

	OpCodeTable table_opcode;
	vector<SectionTable*> table_section;
	SimbolTable table_simbol;
};

#endif