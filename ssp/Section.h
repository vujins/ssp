#ifndef SECTION_H
#define SECTION_H

#include <string>
#include <regex>
#include <map>
#include <fstream>

#include "Reallocation.h"

using namespace std;

class Section {
public:
	Section(string name_);
	Section(string name_, int start, int end, int length_);

	string get_name();
	int get_start_address();
	int get_end_address();
	int get_location_counter();

	void increment_lc(int increment);
	void reset_location_counter();

	static bool is_section(string line);

	void write_rel_table(ofstream &filestream);
	void write_code(ofstream &filestream);

	void append_code(string s);
	void add_realocation(Reallocation *rel);
protected:
	void set_start_address(int start_address_);
	void set_end_address();

private:
	friend class SectionTable;
	static regex section_regex;

	string name;
	int start_address;
	int end_address;
	int length;

	ReallocationTable table_rel;
	vector<string> code;
};

class SectionTable {
public:
	SectionTable();
	SectionTable(int start_address_);
	~SectionTable();

	void set_start_address(int start_address_);
	bool put(Section *section);
	bool add(Section *section);
	Section* get(string name);
	const map<string, Section*> get_table() const;

	void write(ofstream &filestream);

private:
	int start_address;
	map<string, Section*> table;
};

#endif