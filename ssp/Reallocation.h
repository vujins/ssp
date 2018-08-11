#ifndef REALOCATION_H
#define REALOCATION_H

#include <string>
#include <vector>
#include <fstream>

using namespace std;

class Reallocation {
public:
	Reallocation(string address_, string type_, string index_);
	~Reallocation();

	string get_address();
	string get_type();
	int get_index();

private:
	string address;
	string type;
	string index;
};

class ReallocationTable {
public:
	ReallocationTable();
	~ReallocationTable();

	void put(Reallocation *rel);
	void write(ofstream &filestream);

private:
	vector<Reallocation*> table;
};

#endif