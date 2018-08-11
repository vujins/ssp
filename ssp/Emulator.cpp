#include "Emulator.h"

Emulator::Emulator(int argc, char* argv[]) {
	for (int i = 1; i < argc; i++) files.push_back(argv[i]);

	for (int i = 1; i < argc; i++)
		table_section.push_back(new SectionTable());

	//TODO nemoj da zaboravis da setujes start_address za tabelju sekcija
}


Emulator::~Emulator() {
	for (auto it : table_section) delete it;
}

void Emulator::read() {
	for (int i = 0; i < files.size(); i++) {
		ifstream input_filestream;
		input_filestream.open(files[i]);
		assert(input_filestream);

		while (!input_filestream.eof()) {
			string line;
			getline(input_filestream, line);

			if (line.empty()) continue;

			if (line == "#Section table") {
				while (!line.empty()) {
					getline(input_filestream, line);
					if (regex_match(line, regex("^#.*"))) continue;

					smatch sm;
					if (regex_match(line, sm, regex("(\\.text|\\.data|\\.bss|\\.rodata)\t*([0-9]+)\t*([0-9]+)\t*([0-9]+)"))) {
						string name = sm[1].str();
						int start = stoi(sm[2].str());
						int end = stoi(sm[3].str());
						int length = stoi(sm[4].str());
						table_section[i]->add(new Section(name, start, end, length));
					}
				}
			}
			else if (line == "#Simbol table") {
				while (!line.empty()) {
					getline(input_filestream, line);
					if (regex_match(line, regex("^#.*"))) continue;

					smatch sm;
					if (regex_match(line, sm, regex("(\\.?[a-z|A-Z|0-9]+)\t*(\\.?[a-z|A-Z|0-9]+)\t*([0-9]+)\t*(local|global)\t*([0-9]+)"))) {
						string name = sm[1].str();
						string section = sm[2].str();
						int value = stoi(sm[3].str());
						string visibility = sm[4].str();
						int index = stoi(sm[5].str());
						if (table_simbol.get(name)) {
							if (table_simbol.get(name)->get_value()) continue;
							else table_simbol.get(name)->set_value(value);
						}
						else {
							table_simbol.put(name, new Simbol(name, section, value, visibility));
						}
					}
				}
			}
		}
		input_filestream.close();
	}
}

void Emulator::write() {
	ofstream output_filestream;
	output_filestream.open("C:\\Users\\vana\\Documents\\ssp\\tests\\emulator_output.txt");
	
	for (auto it : table_section) it->write(output_filestream);
	table_simbol.write(output_filestream);
	
	output_filestream.close();
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

bool Emulator::get_i() {
	return PSW & FLAG_I;
}

bool Emulator::get_z() {
	return PSW & FLAG_Z;
}

bool Emulator::get_o() {
	return PSW & FLAG_O;
}

bool Emulator::get_c() {
	return PSW & FLAG_C;
}

bool Emulator::get_n() {
	return PSW & FLAG_N;
}

bool Emulator::get_periodic() {
	return PSW & FLAG_periodic;
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
