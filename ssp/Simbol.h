#ifndef SIMBOL_H
#define SIMBOL_H

#include <string>
#include <regex>
#include <map>
#include <fstream>

using namespace std;

class Simbol {
public:
	Simbol(string name_, string section_, int value_, string visibility_);
	~Simbol();

	string get_name();
	string get_section();
	int get_value();
	void set_value(int v);
	string get_visibility();
	void set_viisibility(string visibility_);
	int get_index();

	static bool is_label(string line);
	static string get_label_name(string line);
	static string cut_label_from_line(string line);

private:
	string name;
	string section;
	int value;
	string visibility;
	static int sindex;
	int index;

	static regex regex_simbol;
};

class SimbolTable {
public:
	SimbolTable();
	~SimbolTable();

	bool put(string key, Simbol* simbol);
	Simbol* get(string key);
	void erase(string key);
	void write(ofstream &filestream);

	void add_global_simbols(string line);

private:
	map<string, Simbol*> table;
};

#endif