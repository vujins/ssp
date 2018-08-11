#include "Reallocation.h"



Reallocation::Reallocation(string address_, string type_, int index_, string name_):
address(address_), type(type_), index(index_), name(name_) {}


Reallocation::~Reallocation(){

}

string Reallocation::get_address() {
	return address;
}

string Reallocation::get_type() {
	return type;
}

int Reallocation::get_index() {
	return index;
}

string Reallocation::get_name() {
	return name;
}

ReallocationTable::ReallocationTable() {}

ReallocationTable::~ReallocationTable() {
	for (Reallocation *rel : table)
		delete rel;
}

void ReallocationTable::put(Reallocation * rel) {
	table.push_back(rel);
}

void ReallocationTable::write(ofstream & filestream) {
	filestream << "#address" << "\t\t" << "type" << "\t\t" << "name" << endl;
	for (Reallocation *rel : table) {
		filestream << rel->get_address() << "\t\t" << rel->get_type() <<
			"\t\t" << rel->get_name() << endl;
	}
	filestream << endl;
}
