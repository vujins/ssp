#include "Section.h"

regex Section::section_regex = regex("^.data|.text|.bss|.rodata");

Section::Section(string name_):
	name(name_), start_address(0), end_address(0), length(0) {}

string Section::get_name() {
	return name;
}

int Section::get_start_address() {
	return start_address;
}

int Section::get_end_address() {
	return end_address;
}

int Section::get_length() {
	return length;
}

void Section::increment_lc(int increment) {
	length += increment;
}

bool Section::is_section(string line) {
	if (regex_match(line, section_regex))
		return true;

	return false;
}

void Section::set_start_address(int start_address_) {
	start_address = start_address_;
}

void Section::set_end_address() {
	end_address = start_address + length;
}

SectionTable::SectionTable(int start_address_):
	start_address(start_address_) {}

SectionTable::~SectionTable() {
	for (auto it = table.begin(); it != table.end(); it++)
		delete *it;
}

void SectionTable::put(Section *section) {
	section->set_start_address(start_address);
	section->set_end_address();
	start_address += section->get_length();

	table.push_back(section);
}

void SectionTable::write(ofstream &filestream) {
	filestream << "Section table" << endl;
	filestream << "name" << "\t\t" << "start_address" << "\t" << "end_address" <<
		"\t" << "length" << endl;
	for (auto it = table.begin(); it != table.end(); it++) {
		filestream << (*it)->get_name() << "\t\t" << (*it)->get_start_address() << "\t\t" <<
			(*it)->get_end_address() << "\t\t" << (*it)->get_length() << endl;
	}
	filestream << endl;
}
