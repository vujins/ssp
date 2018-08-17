#include "Emulator.h"

Emulator* Emulator::global_emulator = nullptr;

Emulator::Emulator(int argc, char* argv[]) : finished(false) {
	table_simbol.erase("UND");
	for (int i = 1; i < argc; i++) files.push_back(argv[i]);

	for (int i = 1; i < argc; i++)
		table_section.push_back(new SectionTable());

	write(0, reinterpret_cast<uint32_t> (interrupt_start), 2);
	write(2, reinterpret_cast<uint32_t> (interrupt_periodic), 2);
	write(4, reinterpret_cast<uint32_t> (interrupt_illegal_ins), 2);
	write(6, reinterpret_cast<uint32_t> (interrupt_input), 2);

	interrupt(i_start);

	if (!global_emulator) global_emulator = this;
}


Emulator::~Emulator() {
	for (auto it : table_section) delete it;
}

void Emulator::run() {
	read();
	resolve_conflict();
	execute();
	output();

	this_thread::sleep_for(chrono::seconds(5));


	finished = true;
}

void Emulator::interrupt(int i) {
	uint32_t segment = (uint32_t)interrupt_start & 0xffff0000;
	uint16_t offset = read(2 * i, 2);
	void (*ptr)() = (void (*)())(segment + offset);
	ptr();
}

void Emulator::input_char(char c) {
	write(0xFFFC, c);
}

void Emulator::read() {
	for (size_t i = 0; i < files.size(); i++) {
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

						for (auto it : table_section) {
							map<string, Section*> current_table = (it)->get_table();
							for (auto pair : current_table) {
								assert(!(pair.second->get_start_address() >= start && pair.second->get_start_address() <= end));
								assert(!(pair.second->get_end_address() >= start && pair.second->get_end_address() <= end));

							}
						}
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
						int index_new = 0;
						Simbol *simbol = table_simbol.get(name);
						if (simbol) {
							//dva globalna simbola sa istim imenom
							assert(!(simbol->get_section() != "UND" && section != "UND"));
							if (simbol->get_section() != "UND") {
								index_new = simbol->get_index();
							}
							else {
								simbol->set_value(value);
								simbol->set_setcion(section);
							}
						}
						else {
							//pravilo je da ne moze da se koristi simbol pre nego sto se definise
							//ako je uvezen iz drugog fajla, drugi fajl treba prvi da se ucita
							//assert(section != "UND");
							Simbol *simbol_new = new Simbol(name, section, value, visibility);
							table_simbol.put(name, simbol_new);
							index_new = simbol_new->get_index();
							if (name == ".start") {
								end_address = table_section[i]->get(section)->get_end_address();
							}
						}
						if (section == "UND") {
							if (index_new)
								rel_index[index] = index_new;
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

	bool is_start = false;
	map<string, Simbol*> simbol_table = table_simbol.get_simbol_table();
	for (auto it : simbol_table) {
		if (it.second->get_name() == ".start") is_start = true;
		assert(it.second->get_section() != "UND");
	}
	assert(is_start);
}

void Emulator::execute() {
	r[PC] = table_simbol.get(".start")->get_value();
	r[SP] = STACK_END + 1;

	do {
		uint16_t ins = read_ins();

		uint8_t opcode = ins >> 10;
		uint8_t condition = opcode >> 4;
		uint8_t instruction = opcode & 0xF;

		uint8_t op1 = (ins >> 5) & 0x1F;
		uint8_t op2 = ins & 0x1F;

		int16_t dst;
		int16_t src;
		int16_t result;

		bool execute = true;
		switch (condition) {
		case 0: //eq
			if (!get_z()) execute = false;
			break;
		case 1: //ne
			if (get_z()) execute = false;
			break;
		case 2: //gt
			if (get_n()) execute = false;
			break;
		case 3: //al
			break;
		default: //pogresan uslov
			interrupt(i_illegal_ins);
		}

		switch (instruction) {
		case 0: //add
		case 1: //sub 
		case 2: //mul
		case 3: //div
		{
			dst = get_operand(op1);
			src = get_operand(op2);
			if (!execute) break;
			if (instruction == 0) {
				result = dst + src;
				if ((src > 0) && (dst > INT16_MAX - src)) {
					sto();
					stc();
				}
				else if ((src < 0) && (dst < INT16_MIN - src)) {
					sto();
					stc();
				}
				else {
					clo();
					clc();
				}
			}
			else if (instruction == 1) {
				result = dst - src;
				if ((src < 0) && (dst > INT16_MAX + src)) {
					sto();
					stc();
				}
				else if ((src > 0) && (dst < INT16_MIN + src)) {
					sto();
					stc();
				}
				else {
					clo();
					stc();
				}

			}
			else if (instruction == 2) result = dst * src;
			else if (instruction == 3) result = dst / src;
			store_result(op1, result);

			if (result == 0) stz();
			else clz();
			if (result < 0) stn();
			else clz();

			break;
		}
		case 4: //cmp
		{
			dst = get_operand(op1);
			src = get_operand(op2);

			if (!execute) break;

			result = dst - src;
			if ((src < 0) && (dst > INT16_MAX + src)) {
				sto();
				stc();
			}
			else if ((src > 0) && (dst < INT16_MIN + src)) {
				sto();
				stc();
			}
			else {
				clo();
				clc();
			}

			if (result == 0) stz();
			else clz();
			if (result < 0) stn();
			else cln();

			break;
		}
		case 5: //and
		case 6: //or
		case 7: //not
		case 8: //test
		{
			dst = get_operand(op1);
			src = get_operand(op2);

			if (!execute) break;

			if (instruction == 5) result = dst & src;
			else if (instruction == 6) result = dst | src;
			else if (instruction == 7) result = ~src;
			else if (instruction == 8) result = dst & src;

			if (instruction != 8) store_result(op1, result);

			if (result == 0) stz();
			else clz();
			if (result < 0) stn();
			else cln();
			break;
		}
		case 9: //push
		{
			src = get_operand(op1);

			if (!execute) break;

			pushw(src);

			break;
		}
		case 10: //pop
		{
			if (!execute) break;

			result = popw();
			store_result(op1, result);
			break;
		}
		case 11: //call
		{
			src = get_operand(op1);

			if (!execute) break;

			pushw(r[PC]);
			r[PC] = src;
			break;
		}
		case 12: //iret
		{
			if (!execute) break;
			PSW = popw();
			r[PC] = popw();
			break;
		}
		case 13: //mov
		{
			src = get_operand(op2);

			if (!execute) break;

			store_result(op1, src);
			if (src == 0) stz();
			else clz();
			if (src < 0) stn();
			else cln();
			break;
		}
		case 14: //shl
		case 15: //shr
		{
			dst = get_operand(op1);
			src = get_operand(op2);

			if (!execute) break;

			if (instruction == 14) {
				result = dst << src;
				if (dst & 0x8000) stc();
				else clc();
			}
			if (instruction == 15) {
				result = dst >> src;
				if (dst & 1) stc();
				else clc();
			}
			store_result(op1, src);

			if (result == 0) stz();
			else clz();
			if (result < 0) stn();
			else cln();
			break;
		}
		default: //pogresna instrukcija
			interrupt(i_illegal_ins);
		}

	} while (r[PC] != end_address + 1);
}

void Emulator::output() {
	ofstream output_filestream;
	output_filestream.open(OUTPUT_FILE);
	assert(output_filestream);

	for (string file : files) output_filestream << "#" << file << endl;
	for (auto it : table_section) it->write(output_filestream);
	table_simbol.write(output_filestream);
	output_filestream << "#Reallocation table" << endl;
	table_reallocation.write(output_filestream);

	output_filestream << "1100: ";
	for (int i = table_simbol.get(".start")->get_value(); i <= end_address; i++)
		output_filestream << uppercase << hex << (int)om[i] << " ";
	//output_filestream << endl << "1200: ";
	//for (int i = 1200; i < 1230; i++)
	//	output_filestream << uppercase << hex << (int)om[i] << " ";
	output_filestream << endl;
	for (int i = 0; i < 8; i++) {
		output_filestream << "r[" << i << "] = " << r[i] << " ";
	}

	output_filestream.close();
}

int16_t Emulator::get_operand(uint8_t op) {
	int16_t operand;
	uint8_t op_type = op >> 3;
	uint8_t op_reg = op & 0x7;

	switch (op_type) {
	case 0: {//immediate or PSW
		if (op_reg == 7) operand = PSW;
		else {
			operand = read(r[PC], 2);
			r[PC] += 2;
		}
		break;
	}
	case 1: {//reg dir
		operand = r[op_reg];
		break;
	}
	case 2: {//mem dir
		uint16_t address = read(r[PC], 2);
		r[PC] += 2;
		//operand = om[address];
		operand = read(address, 1);
		break;
	}
	case 3: {//reg ind
		int16_t offset = read(r[PC], 2);
		r[PC] += 2;
		operand = read(r[op_reg] + offset, 1);
		break;
	}
	default: {//pogresna operacija
		interrupt(i_illegal_ins);
	}
	}

	return operand;
}

void Emulator::store_result(uint8_t op, int16_t result) {
	uint8_t op_type = op >> 3;
	uint8_t op_reg = op & 0x7;

	switch (op_type) {
	case 0: {//immediate or PSW
		if (op_reg == 7) PSW = result;
		else assert(false);
		break;
	}
	case 1: {//reg dir
		r[op_reg] = result;
		break;
	}
	case 2: {//mem dir
		uint16_t address = read(r[PC - 2], 2);
		write(address, result, 2);
		break;
	}
	case 3: {//reg ind
		int16_t offset = read(r[PC - 2], 2);
		uint16_t address = r[op_reg] + offset;
		//om[address] = result;
		write(address, result, 2);
		break;
	}
	default: {//pogresna operacija
		interrupt(i_illegal_ins);
	}
	}
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

bool Emulator::is_finished() {
	return finished;
}

void Emulator::interrupt_start() {
	cout << "Emulation started!" << endl;
}

void Emulator::interrupt_periodic() {
	cout << "Periodic job" << endl;
}

void Emulator::interrupt_illegal_ins() {
	cout << "Illegal instruction!" << endl;
	assert(false);
}

void Emulator::interrupt_input() {
	char c = global_emulator->read(0xFFFC, 1);
	cout << "Interrupt input: " << c << endl;
}

void Emulator::check_address(uint16_t addr) {
	assert(addr >= 16 && addr < STACK_START);
}

void Emulator::write(uint16_t addr, uint8_t data) {
	//check_address(addr);

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
		int n = hex_to_decimal(hex.substr(2 * i, 2));
		write(addr + i, n);
	}
}

int Emulator::read(uint16_t address, int bytes) {
	//check_address(address);

	stringstream ss;
	for (int i = bytes - 1; i >= 0; i--) {
		if (om[address + i] < 0x10) ss << '0';
		ss << hex << (int16_t)om[address + i];
	}

	if (bytes == 1) (int8_t)hex_to_decimal(ss.str());
	if (bytes == 2) (int16_t)hex_to_decimal(ss.str());
	return hex_to_decimal(ss.str());
}

uint16_t Emulator::read_ins() {
	check_address(r[PC]);

	uint16_t ins = om[r[PC]++];
	ins <<= 8;
	ins += om[r[PC]++];

	return ins;
}

void Emulator::push(uint8_t data) {
	assert(r[SP] > STACK_START);
	om[--r[SP]] = data;
}

void Emulator::pushw(uint16_t data) {
	push(data);
	push(data >> 8);
}

uint8_t Emulator::pop() {
	assert(r[SP] <= STACK_END);
	return om[r[SP]++];
}

uint16_t Emulator::popw() {
	uint16_t result;

	result = pop() << 8;
	result += pop();

	return result;
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
