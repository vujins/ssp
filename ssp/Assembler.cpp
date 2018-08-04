#include "Assembler.h"

Assembler::Assembler(const char *file, int start_address_): 
	start_address(start_address_), table_section(start_address_), end(".end") {

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

		if (line.length() == 0) continue;
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
				//line = Simbol::cut_label_from_line(line); //a: mov r1, r2 -> mov r1, r2
			}
			if (current_section == nullptr)
				throw invalid_argument("Simbol needs to be in a section!");

			if (!table_simbol.put(name, new Simbol(name, current_section->get_name(),
				start_address + location_counter, "local")))
				throw invalid_argument("Two simbols with the same name are not allowed!");
		}
		//ako je jedna od direktiva .char .word .long .align .skip
		//lc se prenosi u funkciju zbog izracunavanja align
		int increment = OpCode::length_of_directive(line, current_section->get_location_counter());
		if (increment) {
			location_counter += increment;
			current_section->increment_lc(increment);
			continue;
		}

		increment = OpCode::length_of_operation(line);
		if (increment) {
			location_counter += increment;
			current_section->increment_lc(increment);
			continue;
		}
	}
}

void Assembler::output() {
	output_filestream.open(OUTPUT_FILE);
	if (!output_filestream.is_open())
		throw invalid_argument("Output file wrong!");

	table_section.write(output_filestream);
	table_simbol.write(output_filestream);

	output_filestream.close();
}
