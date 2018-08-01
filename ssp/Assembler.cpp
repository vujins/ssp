#include "Assembler.h"

Assembler::Assembler(const char *file, int start_address): 
	table_section(start_address), end(".end") {

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
		if (Section::is_section(line)) {
			if (current_section)
				table_section.put(current_section);
			current_section = new Section(line);
		}
		if (Simbol::is_label(line) || Section::is_section(line)) {
			string name;
			if (Section::is_section(line)) name = line;
			else {
				name = Simbol::get_label_name(line);
				//line = Simbol::cut_label_from_line(line);
			}
			if (current_section == nullptr)
				throw invalid_argument("Simbol needs to be in a section!");

			table_simbol.put(name, new Simbol(name, current_section->get_name(),
				current_section->get_length(), "local"));
		}
		int increment = OpCode::length_of_directive(line, current_section->get_length());
		if (increment) {
			current_section->increment_lc(increment);
			continue;
		}

		//TODO ocitaj duzinu instrukcije
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
