#include "Emulator.h"

Emulator::Emulator(int argc, char* argv[]) {
	for (int i = 1; i < argc; i++) files.push_back(argv[i]);

	for (int i = 1; i < argc; i++)
		table_section.push_back(new SectionTable());
}


Emulator::~Emulator() {
	for (auto it : table_section) delete it;
}

void Emulator::read() {
	for (int i = 0; i < files.size(); i++) {
		ifstream input_filestream;
		input_filestream.open(files[i]);
		assert(input_filestream);

		map<int, int> rel_index; //map[old] = new;

		while (!input_filestream.eof()) {
			string line;
			smatch sm;
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
						string visibility = sm[4].str();
						if (visibility == "local") continue;
						string name = sm[1].str();
						string section = sm[2].str();
						int value = stoi(sm[3].str());
						int index = stoi(sm[5].str());
						Simbol *simbol = table_simbol.get(name);
						if (simbol) {
							assert(!(simbol->get_section() != "UND" && section != "UND"));
							if (simbol->get_section() != "UND") continue;
							simbol->set_value(value);
							simbol->set_setcion(section);
						}
						else {
							//pravilo je da ne moze da se koristi simbol pre nego sto se definise
							//ako je uvezen iz drugog fajla, drugi fajl treba prvi da se ucita
							//assert(section != "UND");
							Simbol *new_simbol = new Simbol(name, section, value, visibility);
							table_simbol.put(name, new_simbol);
							if (section == "UND") {
								rel_index[index] = new_simbol->get_index();
							}
						}
					}
				}
			}
			else if (regex_search(line, regex("^#Reallocation table for"))) {
				while (!line.empty()) {
					getline(input_filestream, line);
					if (regex_match(line, regex("^#.*"))) continue;

					smatch sm;
					if (regex_match(line, sm, regex("([a-z|A-Z|0-9]+)\t*(R_386_PC32|R_386_32)\t*([0-9]+)"))) {
						string address = sm[1].str();
						string type = sm[2].str();
						int index_old = stoi(sm[3].str());
						int index = rel_index[index_old];
						if (!index) continue;
						table_reallocation.put(new Reallocation(address, type, index));
					}
				}

			}
			else if (regex_match(line, sm, regex("#(\\.text|\\.data|\\.bss|\\.rodata) code"))) {
				int address = table_section[i]->get(sm[1].str())->get_start_address();
				getline(input_filestream, line);
				smatch sm_code;
				while (regex_search(line, sm_code, regex("[a-z|A-Z|0-9]{2}"))) {
					uint8_t data = hex_to_decimal(sm_code[0].str());
					write(address++, data);
					line = sm_code.suffix().str();
				}
			}
		}
		input_filestream.close();
	}
}

void Emulator::resolve_conflict() {
	vector<Reallocation*> table = table_reallocation.get_table();
	for (Reallocation *reallocation : table) {
		uint16_t address = hex_to_decimal(reallocation->get_address());
		string type = reallocation->get_type();
		int index = reallocation->get_index();
		Simbol *simbol = table_simbol.get_index(index);
		assert(simbol);

		int value;
		if (type == "R_386_PC32") {
			value = simbol->get_value() + read(address, 2);
		}
		else {
			value = simbol->get_value();
		}

		write(address, value, 2);
	}
}

void Emulator::output() {
	ofstream output_filestream;
	output_filestream.open("C:\\Users\\vana\\Documents\\ssp\\tests\\emulator_output.txt");
	
	for (string file : files) output_filestream << "#" << file << endl;
	for (auto it : table_section) it->write(output_filestream);
	table_simbol.write(output_filestream);
	output_filestream << "#Reallocation table" << endl;
	table_reallocation.write(output_filestream);
	
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

void Emulator::write(uint16_t addr, uint8_t data) {
	//ovo su rezervisana mesta
	assert(addr >= 16 && addr < (OM_SIZE - 128));
	om[addr] = data;
	if (addr_cout == addr) {
		if (data == 0x10) cout << '\n';
		else  cout << data;
	}
}

void Emulator::write(uint16_t addr, int data, int bytes) {
	string hex = decimal_to_hex(data);
	hex = little_endian_from_hex(hex, bytes);
	for (int i = 0; i < bytes; i++) {
		int n = hex_to_decimal(hex.substr(2*i, 2));
		write(addr + i, n);
	}
}

int Emulator::read(uint16_t address, int bytes) {
	stringstream ss;
	for (int i = bytes - 1; i >= 0; i--) {
		ss << hex << (int)om[address + i];
	}

	if (bytes == 1) return (int8_t)hex_to_decimal(ss.str());
	if (bytes == 2) return (int16_t)hex_to_decimal(ss.str());
	return hex_to_decimal(ss.str());
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
