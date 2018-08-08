#include "Assembler.h"

#include <iostream> //TODO obrisi

Assembler::Assembler(const char *file, int start_address_) :
	start_address(start_address_), location_counter(0), table_section(start_address_), 
	end("^\\.end"), comment("^#.*"), current_section(nullptr) {

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
	while (!input_filestream.eof()) {
		string line;
		getline(input_filestream, line);

		//prazan red
		if (line.empty()) continue;
		if (regex_search(line, end)) {
			if (current_section)
				table_section.put(current_section);
			break;
		}
		//komentar sa #
		if (is_comment(line)) continue;
		//tabela sekcija
		if (Section::is_section(line)) {
			if (current_section)
				table_section.put(current_section);
			current_section = new Section(line);
		}
		//tabela simbola
		if (Simbol::is_label(line) || Section::is_section(line)) {
			if (current_section == nullptr)
				throw invalid_argument("Simbol needs to be in a section! Line: " + line);

			string name;
			int value;
			if (Section::is_section(line)) {
				value = 0;
				name = line;
			}
			else {
				value = start_address + location_counter;
				name = Simbol::get_label_name(line);
				line = Simbol::cut_label_from_line(line); //a: mov r1, r2 -> mov r1, r2
			}

			if (!table_simbol.put(name, new Simbol(name, 
				current_section->get_name(), value, "local")))
				throw invalid_argument("Two simbols with the same name are not allowed! Line: " + line);
		}

		increase_location_counter(line);
	}
	reset(); //restartovanje trenutne sekcije, lc, fajl
}

void Assembler::second_pass() {

	while (!input_filestream.eof()) {
		string line;
		getline(input_filestream, line);

		if (line.empty()) continue;
		if (regex_search(line, end)) break;
		if (is_comment(line)) continue;
		if (Section::is_section(line)) {
			current_section = table_section.get(line);
			if (!current_section) throw invalid_argument("Section name invalid! Line: " + line);
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
			if (code.empty()) throw invalid_argument("Skip directive missing an argument! Line: " + line);
			current_section->append_code(code);
		}
		if (OpCode::is_align(line)) {
			string code = OpCode::get_align_code(line, current_section->get_location_counter());
			if (code.empty()) throw invalid_argument("Align directive missing an argument! Line: " + line);
			current_section->append_code(code);
		}
		if (OpCode::is_directive(line)) {
			string code = get_directive_code(line);
			if (code.empty()) throw invalid_argument("Direktiva .char .word ili .long ima preveliku vrednost! Line: " + line);
			current_section->append_code(code);
		}
		if (OpCode::is_instruction(line)) {
			string bincode = get_instruction_code(line);
			if (bincode.empty()) throw invalid_argument("Instruction not valid! Line: " + line);
			string code = bin_to_hex(bincode);
			//ako postoje dodatna 2 bajta pretvaram ih u little endian
			if (code.size() == 8) {
				stringstream ss;
				ss << code.substr(0, 4) << little_endian_from_hex(code.substr(4), 2);
				code = ss.str();
			}
			current_section->append_code(code);
		}

		if (!increase_location_counter(line))
			throw invalid_argument("Skip or align directives missing an argument! Line: " + line);
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

bool Assembler::increase_location_counter(string line) {
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

string Assembler::little_endian_from_hex(string hex, int multiplier) {
	stringstream code;

	for (int i = 1; i <= multiplier; i++) {
		if (((int)hex.size() - 2 * i) >= 0) code << hex[hex.size() - 2 * i];
		else code << "0";
		if (((int)hex.size() - 2 * i + 1) >= 0) code << hex[hex.size() - 2 * i + 1];
		else code << "0";
	}

	return code.str();
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
				return "";
		}
		if (hex.size() > 2 * multiplier) return string(); //hex vrednost veca od dozvoljene

		code << little_endian_from_hex(hex, multiplier);

		line = result.suffix().str();
	}
	return code.str();
}


void Assembler::reset() {
	current_section = nullptr;
	location_counter = 0;
	for (auto it : table_section.get_table())
		it.second->reset_location_counter();

	input_filestream.clear();
	input_filestream.seekg(0, ios::beg);
}

bool Assembler::is_comment(string line) {
	if (regex_match(line, comment)) return true;

	return false;
}

string Assembler::bin_to_hex(string bin) {
	stringstream code;
	for (size_t i = 0; (bin.substr(i * 4, 4)).compare("0000") == 0; i++)
		code << "0";

	code << hex << stoll(bin, nullptr, 2);
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
		//pseudo instrukcija ret
		if (regex_match(ins, regex("^(eq|ne|gt|al)ret"))) {
			code << table_opcode.get_opcode(ins)->get_opcode();
			code << "0111100000";
			return code.str();
		}
		//pseudo instrukciju jmp
		smatch sm;
		if (regex_match(ins, sm, regex("^(eq|ne|gt|al)jmp"))) {
			if (!op2.empty()) return ""; //jmp ima samo jedan operand
			if (regex_match(op1, OpCode::regex_pc_rel)) {
				//jmp $lab -> add pc, &lab
				code << table_opcode.get_opcode(sm[1].str() + "add")->get_opcode();
				code << "0111100000"; 
				get_operand_code(op1, first_operand);
			}
			else {
				code << table_opcode.get_opcode(sm[1].str() + "mov")->get_opcode();
				code << "01111";
				code << get_operand_code(op1, first_operand);
			}
			if (!first_operand.empty())
				code << first_operand;
			return code.str();
		}

		//ako je pogresno napisana instrukcija vraca gresku
		if (!table_opcode.get_opcode(ins)) return "";
		code << table_opcode.get_opcode(ins)->get_opcode();
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
	smatch sm;

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
	else if (regex_match(operand, sm, OpCode::regex_reg_ind)) {
		//reg ind: r6[30] ili r6[labela]
		cout << "reg ind: " << operand << endl;
		code << "11" << OpCode::dec_to_bin(stoi(sm[1].str()), 3);
		if (regex_match(sm[2].str(), regex("[0-9]+"))) {
			//neposredna vrednost
			result = OpCode::dec_to_bin(stoi(sm[2].str()), 16);
		}
		else {
			//labela
			int value = table_simbol.get(sm[2].str())->get_value();
			result = OpCode::dec_to_bin(value, 16);
		}
	}
	else if (regex_match(operand, sm, OpCode::regex_pc_rel)) {
		//pc relativno: $labela
		cout << "pc rel: " << operand << endl;
		code << "11111";
		int value;
		if (regex_match(operand.substr(1), OpCode::regex_immediate))
			value = stoi(operand.substr(1)); //$128
		else
			value = table_simbol.get((sm[1].str()))->get_value(); //$a
		result = OpCode::dec_to_bin(value, 16);
	}
	else if (regex_match(operand, sm, OpCode::regex_simbol_value)) {
		//vrednost simbola kao neposredna vrednost: &labela
		cout << "vrednost simbola: " << operand << endl;
		code << "00000";
		int value = table_simbol.get((sm[1].str()))->get_value();
		result = OpCode::dec_to_bin(value, 16);
	}
	else if (table_simbol.get(operand)) {
		//memorisko direktno (samo labela): labela
		cout << "mem dir: " << operand << endl;
		code << "10000";
		int value = table_simbol.get(operand)->get_value();
		result = OpCode::dec_to_bin(value, 16);
	}
	else {
		table_simbol.erase(operand);
	}

	return code.str();
}
