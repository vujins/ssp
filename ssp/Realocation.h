#ifndef REALOCATION_H
#define REALOCATION_H

#include <string>
#include <vector>
#include <fstream>

using namespace std;

class Realocation {
public:
	Realocation(string address_, string type_, int index_);
	~Realocation();

	string get_address();
	string get_type();
	int get_index();

private:
	string address;
	string type;
	int index;
};

class RealocationTable {
public:
	RealocationTable();
	~RealocationTable();

	void put(Realocation *rel);
	void write(ofstream &filestream);

private:
	vector<Realocation*> table;
};

#endif