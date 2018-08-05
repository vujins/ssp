#ifndef SECTION_H
#define SECTION_H

#include <string>
#include <regex>
#include <map>
#include <fstream>

#include "Realocation.h"

using namespace std;

class Section {
public:
	Section(string name_);

	string get_name();
	int get_start_address();
	int get_end_address();
	int get_location_counter();

	void increment_lc(int increment);

	static bool is_section(string line);

	void write_rel_table(ofstream &filestream);
	void write_code(ofstream &filestream);

	void append_code(string s);
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

	RealocationTable rel_table;
	vector<string> code;
};

class SectionTable {
public:
	SectionTable(int start_address_);
	~SectionTable();

	bool put(Section *section);
	Section* get(string name);
	const map<string, Section*> get_table() const;

	void write(ofstream &filestream);

private:
	int start_address;
	map<string, Section*> table;
};

#endif