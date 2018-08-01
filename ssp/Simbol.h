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
	string get_visibility();
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

	static regex simbol_regex;
};

class SimbolTable {
public:
	SimbolTable();
	~SimbolTable();

	int put(string key, Simbol* simbol);
	//-1 ako vec postoji
	//0 ako je ok
	void write(ofstream &filestream);
	//TODO metoda za pisanje tabele simbola u fajl

private:
	map<string, Simbol*> table;
};

#endif