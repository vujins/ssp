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
			current_section->append_code(code);
		}
		if (OpCode::is_align(line)) {
			string code = OpCode::get_align_code(line, current_section->get_location_counter());
			current_section->append_code(code);
		}
		if (OpCode::is_directive(line)) {
			string code = OpCode::get_directive_code(line);
			if (code.empty()) throw invalid_argument("Direktiva .char .word ili .long ima preveliku vrednost!");
			current_section->append_code(code);
		}

		increase_location_counter(line, location_counter, current_section);
	}
}

void Assembler::output() {
	output_filestream.open(OUTPUT_FILE);
	if (!output_filestream.is_open())
		throw invalid_argument("Output file wrong!");

	table_section.write(output_filestream);
	table_simbol.write(output_filestream);

	for (auto it : table_section.get_table()) {
		it.second->write_rel_table(output_filestream);
	}
	for (auto it : table_section.get_table()) {
		it.second->write_code(output_filestream);
	}

	output_filestream.close();
}

void Assembler::increase_location_counter(string line, int & location_counter, Section * current_section) {
	//ako je jedna od direktiva .char .word .long .align .skip
	//lc se prenosi u funkciju zbog izracunavanja align
	int increment = OpCode::length_of_directive(line, current_section->get_location_counter());
	if (increment) {
		location_counter += increment;
		current_section->increment_lc(increment);
		return;
	}

	increment = OpCode::length_of_operation(line);
	if (increment) {
		location_counter += increment;
		current_section->increment_lc(increment);
		return;
	}

}
