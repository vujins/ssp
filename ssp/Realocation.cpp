#include "Realocation.h"



Realocation::Realocation(string address_, string type_, int index_):
address(address_), type(type_), index(index_) {}


Realocation::~Realocation(){

}

string Realocation::get_address() {
	return address;
}

string Realocation::get_type() {
	return type;
}

int Realocation::get_index() {
	return index;
}

RealocationTable::RealocationTable() {}

RealocationTable::~RealocationTable() {
	for (Realocation *rel : table)
		delete rel;
}

void RealocationTable::put(Realocation * rel) {
	table.push_back(rel);
}

void RealocationTable::write(ofstream & filestream) {
	filestream << "#address" << "\t\t" << "type" << "\t\t" << "index" << endl;
	for (Realocation *rel : table) {
		filestream << rel->get_address() << "\t\t" << rel->get_type() <<
			"\t\t" << rel->get_index() << endl;
	}
	filestream << endl;
}
