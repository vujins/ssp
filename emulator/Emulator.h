#ifndef EMULATOR_H
#define EMULATOR_H

#include <string>
#include <fstream>
#include <cassert>
#include <iostream>
#include <regex>

//TODO obrisi
#include <thread>
#include <chrono>

#include "config.h"
#include "OpCode.h"
#include "Section.h"
#include "Simbol.h"
#include "functions.h"

using namespace std;

//TODO promeni define kad budes radio javni test
#define OUTPUT_FILE "C:\\Users\\vana\\Desktop\\emulator.txt"

class Emulator {
public:
	Emulator(int argc, char* argv[]);
	~Emulator();

	void run();

	void interrupt(int i);
	void input_char(char c);

	bool get_periodic();
	bool is_finished();

protected:
	static void interrupt_start();
	static void interrupt_periodic();
	static void interrupt_illegal_ins();
	static void interrupt_input();


	void read();
	void resolve_conflict();
	void execute();

	void output();

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


	void check_address(uint16_t addr);
	void write(uint16_t addr, int data, int bytes);
	void write(uint16_t addr, uint8_t data);
	//little-endian
	int read(uint16_t address, int bytes);
	uint16_t read_ins();

	void push(uint8_t data);
	void pushw(uint16_t data);
	uint8_t pop();
	uint16_t popw();

private:
	static Emulator *global_emulator;

	bool finished;
	vector<string> files;

	uint8_t om[OM_SIZE];

	uint16_t r[8];
	uint16_t PSW;
	uint16_t end_address;

	vector<SectionTable*> table_section;
	SimbolTable table_simbol;
	ReallocationTable table_reallocation;
};

#endif