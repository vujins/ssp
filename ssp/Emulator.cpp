#include "Emulator.h"

Emulator::Emulator(string input) {
	input_filestream.open(input);
	assert(input_filestream);

	//TODO nemoj da zaboravis da setujes start_address za tabelju sekcija
}


Emulator::~Emulator() {
	input_filestream.close();
}

void Emulator::cli() {
	PSW &= ~FLAG_I;
}

void Emulator::clz() {
	PSW &= ~FLAG_Z;
}

void Emulator::clo() {
	PSW &= ~FLAG_O;
}

void Emulator::clc() {
	PSW &= ~FLAG_C;
}

void Emulator::cln() {
	PSW &= ~FLAG_N;
}

void Emulator::clperiodic() {
	PSW &= ~FLAG_periodic;
}

void Emulator::write(uint16_t addr, int8_t data) {
	om[addr] = data;
	if (addr_cout == addr) {
		if (data == 0x10) cout << '\n';
		else  cout << data;
	}
}

void Emulator::sti() {
	PSW |= FLAG_I;
}

void Emulator::stz() {
	PSW |= FLAG_Z;
}

void Emulator::sto() {
	PSW |= FLAG_O;
}

void Emulator::stc() {
	PSW |= FLAG_C;
}

void Emulator::stn() {
	PSW |= FLAG_N;
}

void Emulator::stperiodic() {
	PSW |= FLAG_periodic;
}
