#ifndef SECTION_H
#define SECTION_H

#include <string>
#include <regex>
#include <vector>
#include <fstream>

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
};

class SectionTable {
public:
	SectionTable(int start_address_);
	~SectionTable();

	bool put(Section *section);
	void write(ofstream &filestream);

private:
	int start_address;
	vector<Section*> table;
};

#endif