#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdint.h>
#include <string>
#include <fstream>
#include <cassert>
#include <iostream>

#include "OpCode.h"
#include "Section.h"
#include "Simbol.h"

using namespace std;

typedef uint16_t address;

#define PC 7
#define SP 6
//PSW flagovi
#define FLAG_Z 0x1 //0001
#define FLAG_O 0x2 //0010
#define FLAG_C 0x4 //0100
#define FLAG_N 0x8 //1000
#define FLAG_I 0x8000 //1000 0000 0000 0000
#define FLAG_periodic 0x1000

//upisom na ovu adresu ispisuje se vrednost na ekranu
#define addr_cout 0xFFFEU
//pritiskom na taster poziva se prekidna rutina na ulazu 3 koja upisuje podatak na ovu adresu
#define addr_data 0xFFFCU

class Emulator {
public:
	Emulator(string input);
	~Emulator();

//protected:
	void sti();
	void stz();
	void sto();
	void stc();
	void stn();
	void stperiodic();

	void cli();
	void clz();
	void clo();
	void clc();
	void cln();
	void clperiodic();

	void write(uint16_t addr, int8_t data);


private:
	ifstream input_filestream;

	//od adrese 0 nalazi se IVT sa 8 ulaza
	//poslednjih 128 bajtova rezervisano za periferije
	int8_t om[0x10000]; //2^16
	uint16_t r[8];
	uint16_t PSW;

	Section *current_section;
	OpCodeTable table_opcode;
	SectionTable table_section;
	SimbolTable table_simbol;
};

#endif