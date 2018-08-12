#include "Simbol.h"
#include <set>

regex Simbol::regex_simbol("^[a-z0-9]+:");
int Simbol::sindex = 0;

Simbol::Simbol(string name_, string section_, int value_, string visibility_) :
	name(name_), section(section_), value(value_), visibility(visibility_), index(sindex++) {}

Simbol::~Simbol() {
}

string Simbol::get_name() {
	return name;
}

string Simbol::get_section() {
	return section;
}

void Simbol::set_setcion(string s) {
	section = s;
}

int Simbol::get_value() {
	return value;
}

void Simbol::set_value(int v) {
	value = v;
}

string Simbol::get_visibility() {
	return visibility;
}

void Simbol::set_viisibility(string visibility_) {
	visibility = visibility_;
}

int Simbol::get_index() {
	return index;
}

bool Simbol::is_label(string line) {
	if (regex_search(line, regex_simbol)) return true;

	return false;
}

string Simbol::get_label_name(string line) {
	int position = line.find(":");

	return line.substr(0, position);
}

string Simbol::cut_label_from_line(string line) {
	size_t position = line.find(":");
	if (position + 1 == line.size())
		return line.substr(position + 1);
	else
		return line.substr(position + 2);
}

SimbolTable::SimbolTable() {
	put("UND", new Simbol("UND", "UND", 0, "local"));
}

SimbolTable::~SimbolTable() {
	for (auto const &simbol : table)
		delete simbol.second;
}

bool SimbolTable::put(string key, Simbol * simbol) {
	if (table[key] != nullptr)
		return false;
	table[key] = simbol;
	return true;
}

Simbol * SimbolTable::get(string key) {
	return table[key];
}

Simbol * SimbolTable::get_index(int index) {
	Simbol *simbol = nullptr;
	for (auto s : table) 
		if (s.second->get_index() == index) {
			simbol = s.second;
		}
	return simbol;
}

void SimbolTable::erase(string key) {
	table.erase(key);
}

struct comp
{
	bool operator()(const pair<std::string, Simbol*>& l, const pair<std::string, Simbol*>& r) const
	{
		if (!l.second || !r.second) return false;
		if (l.second != r.second)
			return l.second->get_index() < r.second->get_index();

		return l.first < r.first;
	}
};

void SimbolTable::write(ofstream &filestream) {
	std::set<std::pair<std::string, Simbol*>, comp> set(table.begin(), table.end());

	filestream << "#Simbol table" << endl;
	filestream << "#name" << "\t\t" << "section" << "\t\t" << "value" << "\t\t" << "visibility" <<
		"\t" << "index" << endl;
	for (auto const &simbol : set) {
		filestream << simbol.second->get_name() << "\t\t" << simbol.second->get_section() <<
			"\t\t" << simbol.second->get_value() << "\t\t" << simbol.second->get_visibility() <<
			"\t\t" << simbol.second->get_index() << endl;
	}
	filestream << endl;
}

void SimbolTable::add_global_simbols(string line) {
	int position = line.find(" ");
	line = line.substr(position + 1);

	regex reg("[a-z|A-Z|0-9]+");
	smatch result;
	while (regex_search(line, result, reg)) {
		if (table[result[0]])
			table[result[0]]->set_viisibility("global");
		else
			table[result[0]] = new Simbol(result[0], "UND", 0, "global");

		line = result.suffix().str();
	}
}
