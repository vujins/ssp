#include "Section.h"
#include <set>

regex Section::section_regex = regex("^\\.data|^\\.text|^\\.bss|^\\.rodata");

Section::Section(string name_):
	name(name_), start_address(0), end_address(0), length(0) {}

Section::Section(string name_, int start, int end, int length_):
	name(name_), start_address(start), end_address(end), length(length_) {}

string Section::get_name() {
	return name;
}

int Section::get_start_address() {
	return start_address;
}

int Section::get_end_address() {
	return end_address;
}

int Section::get_location_counter() {
	return length;
}

void Section::increment_lc(int increment) {
	length += increment;
}

void Section::reset_location_counter() {
	length = 0;
}

bool Section::is_section(string line) {
	if (regex_match(line, section_regex))
		return true;

	return false;
}

void Section::write_rel_table(ofstream & filestream) {
	filestream << "#Reallocation table for " << name << endl;
	table_rel.write(filestream);
}

void Section::write_code(ofstream & filestream) {
	filestream << "#" << name << " code" << endl;
	for (size_t i = 0, k = 0; i < code.size(); i++) {
		//filestream << code[i];
		for (size_t j = 0; j < code[i].size(); j++) {
			if (!(k % 4) && (k != 0)) {
				filestream << " ";
			}
			filestream << code[i][j];
			k++;
		}
	}
	filestream << endl;
}

void Section::append_code(string s) {
	code.push_back(s);
}

void Section::add_realocation(Reallocation * rel) {
	table_rel.put(rel);
}

void Section::set_start_address(int start_address_) {
	start_address = start_address_;
}

void Section::set_end_address() {
	end_address = start_address + length;
}

SectionTable::SectionTable() {
}

SectionTable::SectionTable(int start_address_):
	start_address(start_address_) {}

SectionTable::~SectionTable() {
	for (auto it : table)
		delete it.second;
}

void SectionTable::set_start_address(int start_address_) {
	start_address = start_address_;
}

bool SectionTable::put(Section *section) {
	//ovde bi eventualno mogla da dodje provera da li selcija vec postoji
	//ali ta provera vec postoji za simbol i pre ce se izvrsiti nego za sekciju
	if (table[section->get_name()] != nullptr) return false;

	section->set_start_address(start_address);
	section->set_end_address();
	start_address += section->get_location_counter();

	table[section->get_name()] = section;

	return true;
}

bool SectionTable::add(Section * section) {
	if (table[section->get_name()]) return false;
	else table[section->get_name()] = section;

	return true;
}

Section* SectionTable::get(string name) {
	return table[name];
}

const map<string, Section*> SectionTable::get_table() const {
	return table;
}

struct comp
{
	bool operator()(const pair<std::string, Section*>& l, const pair<std::string, Section*>& r) const
	{
		if (l.second != r.second)
			return l.second->get_start_address() < r.second->get_start_address();

		return l.first < r.first;
	}
};

void SectionTable::write(ofstream &filestream) {
	std::set<std::pair<std::string, Section*>, comp> set(table.begin(), table.end());

	filestream << "#Section table" << endl;
	filestream << "#name" << "\t\t" << "start" << "\t\t" << "end" <<
		"\t\t" << "length" << endl;
	for (auto const &section : set) {
		filestream << section.second->get_name() << "\t\t" << 
			section.second->get_start_address() << "\t\t" <<
			section.second->get_end_address() << "\t\t" << 
			section.second->get_location_counter() << endl;
	}
	filestream << endl;
}
