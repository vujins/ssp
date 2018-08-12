#ifndef REALOCATION_H
#define REALOCATION_H

#include <string>
#include <vector>
#include <fstream>

using namespace std;

class Reallocation {
public:
	Reallocation(string address_, string type_, int index_);
	~Reallocation();

	string get_address();
	string get_type();
	int get_index();
	string get_name();

private:
	string address;
	string type;
	int index;
	string name;
};

class ReallocationTable {
public:
	ReallocationTable();
	~ReallocationTable();

	void put(Reallocation *rel);
	void write(ofstream &filestream);
	vector<Reallocation*> get_table();

private:
	vector<Reallocation*> table;
};

#endif