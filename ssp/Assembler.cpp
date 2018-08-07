#include "Assembler.h"

#include <iostream> //TODO obrisi ovo

Assembler::Assembler(const char *file, int start_address_) :
	start_address(start_address_), table_section(start_address_), end("^\\.end") {

	if (start_address == 0)
		throw invalid_argument("Start address is invalid!");
	input_filestream.open(file);
	if (!input_filestream.is_open())
		throw invalid_argument("File path is invalid!");
}


Assembler::~Assembler() {
	if (input_filestream.is_open())
		input_filestream.close();
}

void Assembler::first_pass() {
	int location_counter = 0;
	Section *current_section = nullptr;

	while (!input_filestream.eof()) {
		string line;
		getline(input_filestream, line);

		if (line.empty()) continue;
		if (regex_search(line, end)) {
			if (current_section)
				table_section.put(current_section);
			break;
		}
		//tabela sekcija
		if (Section::is_section(line)) {
			if (current_section)
				table_section.put(current_section);
			current_section = new Section(line);
		}
		//tabela simbola
		if (Simbol::is_label(line) || Section::is_section(line)) {
			string name;
			if (Section::is_section(line)) name = line;
			else {
				name = Simbol::get_label_name(line);
				line = Simbol::cut_label_from_line(line); //a: mov r1, r2 -> mov r1, r2
			}
			if (current_section == nullptr)
				throw invalid_argument("Simbol needs to be in a section!");

			if (!table_simbol.put(name, new Simbol(name, current_section->get_name(),
				start_address + location_counter, "local")))
				throw invalid_argument("Two simbols with the same name are not allowed!");
		}

		increase_location_counter(line, location_counter, current_section);
	}
	for (auto it : table_section.get_table())
		it.second->reset_location_counter();

	input_filestream.clear();
	input_filestream.seekg(0, ios::beg);
}

void Assembler::second_pass() {
	int location_counter = 0;
	Section *current_section = nullptr;

	while (!input_filestream.eof()) {
		string line;
		getline(input_filestream, line);

		if (line.empty()) continue;
		if (regex_search(line, end)) break;
		if (Section::is_section(line)) {
			current_section = table_section.get(line);
			if (!current_section) throw invalid_argument("Section name invalid!");
			continue;
		}
		if (Simbol::is_label(line)) {
			line = Simbol::cut_label_from_line(line); //a: mov r1, r2 -> mov r1, r2
			if (line.empty()) continue;
		}
		if (OpCode::is_global(line)) {
			table_simbol.add_global_simbols(line);
			continue;
		}
		if (OpCode::is_skip(line)) {
			string code = OpCode::get_skip_code(line);
			if (code.empty()) throw invalid_argument("Skip directive missing an argument!");
			current_section->append_code(code);
		}
		if (OpCode::is_align(line)) {
			string code = OpCode::get_align_code(line, current_section->get_location_counter());
			if (code.empty()) throw invalid_argument("Align directive missing an argument!");
			current_section->append_code(code);
		}
		if (OpCode::is_directive(line)) {
			string code = get_directive_code(line);
			if (code.empty()) throw invalid_argument("Direktiva .char .word ili .long ima preveliku vrednost!");
			current_section->append_code(code);
		}
		if (OpCode::is_instruction(line)) {
			string bincode = get_instruction_code(line);
			if (bincode.empty()) throw invalid_argument("Instruction not valid!");
			stringstream code;
			code << hex << stoll(bincode, nullptr, 2);
			current_section->append_code(code.str());
		}

		if (!increase_location_counter(line, location_counter, current_section))
			throw invalid_argument("Skip or align directives missing an argument!");
	}
}

void Assembler::output() {
	output_filestream.open(OUTPUT_FILE);
	if (!output_filestream.is_open())
		throw invalid_argument("Output file path does not exist!");

	table_section.write(output_filestream);
	table_simbol.write(output_filestream);

	for (auto it : table_section.get_table()) {
		if (!(it.first).compare(".bss")) continue;
		it.second->write_rel_table(output_filestream);
	}
	for (auto it : table_section.get_table()) {
		if (!(it.first).compare(".bss")) continue;
		it.second->write_code(output_filestream);
	}

	output_filestream.close();
}

bool Assembler::increase_location_counter(string line, int & location_counter, Section * current_section) {
	//ako je jedna od direktiva .char .word .long .align .skip
	//lc se prenosi u funkciju zbog izracunavanja align
	int increment = OpCode::length_of_directive(line, current_section->get_location_counter());
	if (increment == -1) return false;
	if (increment) {
		location_counter += increment;
		current_section->increment_lc(increment);
		return true;
	}

	increment = OpCode::length_of_operation(line);
	if (increment == -1) return false;
	if (increment) {
		location_counter += increment;
		current_section->increment_lc(increment);
		return true;
	}

	return true;
}

string Assembler::get_directive_code(string line) {
	stringstream code;
	size_t multiplier;
	smatch directive;
	if (regex_search(line, directive, OpCode::regex_char)) multiplier = 1;
	if (regex_search(line, directive, OpCode::regex_word)) multiplier = 2;
	if (regex_search(line, directive, OpCode::regex_long)) multiplier = 4;
	line = directive.suffix().str();

	regex alfanum("[0-9|a-z|A-Z]+");
	smatch result;
	while (regex_search(line, result, alfanum)) {
		string hex;
		if (regex_match(result[0].str(), regex("[0-9]+"))) {
			//neposredna vrednost
			hex = OpCode::decimal_to_hex(stoi(result[0]));
		}
		else {
			//labela
			Simbol *simbol = table_simbol.get(result[0].str());
			if (simbol)
				hex = OpCode::decimal_to_hex(simbol->get_value());
			else
				return string();
		}
		if (hex.size() > 2 * multiplier) return string(); //hex vrednost veca od dozvoljene

		for (int i = 1; i <= multiplier; i++) {
			if (((int)hex.size() - 2 * i) >= 0) code << hex[hex.size() - 2 * i];
			else code << "0";
			if (((int)hex.size() - 2 * i + 1) >= 0) code << hex[hex.size() - 2 * i + 1];
			else code << "0";
		}
		line = result.suffix().str();
	}
	return code.str();
}


string Assembler::get_instruction_code(string line) {
	stringstream code;
	regex instruction("((?:eq|ne|gt|al)[a-z]{2,4}) ?([^,]*),? ?([^,]*)");
	smatch result;
	string ins, op1, op2, first_operand, second_operand;

	if (regex_match(line, result, instruction)) {
		ins = result[1]; //instrukcija
		op1 = result[2]; //prvi operand
		op2 = result[3]; //drugi operand
	}
	else {
		return "";
	}

	if (!ins.empty()) {
		//ako je pogresno napisana instrukcija vraca gresku
		if (!table_opcode.get_opcode(ins)) return "";
		code << table_opcode.get_opcode(ins)->get_opcode();

		//pseudo instrukcija ret
		if (regex_match(ins, regex("^(eq|ne|gt|al)ret"))) {
			code << "0111100000";
			return code.str();
		}
		//TODO dodaj pseudo instrukciju jmp
	}

	//prvi operand
	if (!op1.empty()) {
		code << get_operand_code(op1, first_operand);
	}
	else {
		code << "00000";
	}

	//drugi operand
	if (!op2.empty()) {
		code << get_operand_code(op2, second_operand);
	}
	else {
		code << "00000";
	}

	//ako oba operanda zahtevaju dodatna 2 bajta
	if (!first_operand.empty() && !second_operand.empty()) return "";
	if (!first_operand.empty()) code << first_operand;
	if (!second_operand.empty()) code << second_operand;

	return code.str();
}

//00 neposredno ili PSW
//01 za registarsko direktno
//10 memorijsko
//11 reg indirektno sa pomerajem
string Assembler::get_operand_code(string operand, string &result) {
	stringstream code;
	smatch smatch;

	if (regex_match(operand, OpCode::regex_register)) {
		//registarsko direktno: r[0-7]
		cout << "registarsko: " << operand << endl;
		code << "01" << OpCode::dec_to_bin(stoi(operand.substr(1)), 3);
	}
	else if (regex_match(operand, OpCode::regex_immediate)) {
		//neposredno: 20
		cout << "neposredno: " << operand << endl;
		result = OpCode::dec_to_bin(stoi(operand), 16);
		code << "00000";
	}
	else if (regex_match(operand, OpCode::regex_mem_dir)) {
		//mem dir: *20
		cout << "mem dir: " << operand << endl;
		result = OpCode::dec_to_bin(stoi(operand.substr(1)), 16);
		code << "10000";
	}
	else if (regex_match(operand, smatch, OpCode::regex_reg_ind)) {
		//reg ind: r6[30]
		cout << "reg ind: " << operand << endl;
		code << "11" << OpCode::dec_to_bin(stoi(smatch[1].str()), 3);
		if (regex_match(smatch[2].str(), regex("[0-9]+"))) {
			result = OpCode::dec_to_bin(stoi(smatch[2].str()), 16);
		}
		else {
			int value = table_simbol.get(smatch[2])->get_value();
			result = OpCode::dec_to_bin(value, 16);
		}
	}


	return code.str();
}
