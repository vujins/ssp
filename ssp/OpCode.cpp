#include "OpCode.h"
#include <algorithm>

regex OpCode::regex_directive("^\\.char|^\\.word|^\\.long");
regex OpCode::regex_char("^\\.char");
regex OpCode::regex_word("^\\.word");
regex OpCode::regex_long("^\\.long");
regex OpCode::regex_skip("^\\.skip");
regex OpCode::regex_align("^\\.align");
regex OpCode::regex_global("^\\.global");

regex OpCode::regex_comma(",");
regex OpCode::regex_registers("(r[0-7],r[0-7])$");

regex OpCode::regex_operation("^eq[a-z]{2,4}|^ne[a-z]{2,4}|^gt[a-z]{2,4}|^al[a-z]{2,4}");
regex OpCode::regex_no_operands("(eq[a-z]{2,4})$|(ne[a-z]{2,4})$|(gt[a-z]{2,4})$|(al[a-z]{2,4})$");

//adresiranja
regex OpCode::regex_register("(r[0-7])$");
regex OpCode::regex_mem_dir("^(\\*[0-9]+)");
regex OpCode::regex_immediate("^[0-9]+");
regex OpCode::regex_reg_ind("^r([0-7])\\[([0-9|a-z|A-Z]+)\\]");
regex OpCode::regex_pc_rel("^\\$([a-z|A-Z|0-9]+)");
regex OpCode::regex_simbol_value("^&([a-z|A-Z|0-9]+)");

OpCode::OpCode(string name_, string opcode_) :
	name(name_), opcode(opcode_) {}


OpCode::~OpCode() {

}

const string OpCode::get_name() const {
	return name;
}

const string OpCode::get_opcode() const {
	return opcode;
}

int OpCode::length_of_directive(string line, int lc) {
	int count = std::count(line.begin(), line.end(), ',');
	if (regex_search(line, regex_char)) return 1 + count;
	if (regex_search(line, regex_word)) return 2 + 2 * count;
	if (regex_search(line, regex_long)) return 4 + 4 * count;
	if (regex_search(line, regex_skip)) {
		int position = line.rfind(" ");
		if (position == -1) return -1;
		return stoi(line.substr(position));
	}
	if (regex_search(line, regex_align)) {
		int position = line.rfind(" ");
		if (position == -1) return -1;
		int align = stoi(line.substr(position));
		return (align - lc % align) % align;
	}

	return 0;
}

int OpCode::length_of_operation(string line) {
	if (!regex_search(line, regex_operation)) return 0; //nije operacija

	if (regex_search(line, regex_no_operands)) {
		//nema operanda
		return 2;
	}
	if (regex_search(line, regex_comma)) {
		//dva operanda
		if (regex_search(line, regex_registers)) {
			return 2;
		}
		else {
			return 4;
		}
	}
	else {
		//jedan operand

		if (regex_search(line, regex_register)) {
			return 2;
		}
		else {
			return 4;
		}
	}
}

bool OpCode::is_global(string line) {
	if (regex_search(line, regex_global)) return true;

	return false;
}

bool OpCode::is_skip(string line) {
	if (regex_search(line, regex_skip)) return true;

	return false;
}

bool OpCode::is_align(string line) {
	if (regex_search(line, regex_align)) return true;

	return false;
}

bool OpCode::is_directive(string line) {
	if (regex_search(line, regex_directive)) return true;

	return false;
}

bool OpCode::is_instruction(string line) {
	if (regex_search(line, regex_operation)) return true;

	return false;
}

string OpCode::get_skip_code(string line) {
	string code;
	int position = line.find(" ");
	if (position == -1) return code;
	line = line.substr(position);
	int skip = stoi(line);
	for (int i = 0; i < skip; i++) {
		code.append("00");
	}

	return code;
}

string OpCode::get_align_code(string line, int lc) {
	string code;
	int position = line.find(" ");
	if (position == -1) return code;
	line = line.substr(position);
	int align = stoi(line);
	int fill = (align - lc % align) % align;

	for (int i = 0; i < fill; i++)
		code.append("00");

	return code;
}

string OpCode::decimal_to_hex(int n) {
	string hexbr;
	int rem;
	if (n == 0) return "0";
	while (n > 0) {
		rem = n % 16;
		switch (rem)
		{
		case 10: hexbr = 'A' + hexbr; break;
		case 11: hexbr = 'B' + hexbr; break;
		case 12: hexbr = "C" + hexbr; break;
		case 13: hexbr = "D" + hexbr; break;
		case 14: hexbr = "E" + hexbr; break;
		case 15: hexbr = "F" + hexbr; break;
		default: hexbr = to_string(rem) + hexbr;
		}
		n /= 16;
	}
	return hexbr;
}

string OpCode::dec_to_bin(int n, size_t bits) {
	string result;
	for (int i = 0; i < bits; i++) {
		result = (char)((n & 1) + '0') + result;
		n >>= 1;
	}
	return result;
}

OpCodeTable::OpCodeTable() : regex_alfanum("[a-z|A-Z|0-9|\\*|\\[|\\]]+") {
	//-----------------eq-------------------
	table["eqadd"] = new OpCode("eqadd", "000000");
	table["eqsub"] = new OpCode("eqsub", "000001");
	table["eqmul"] = new OpCode("eqmul", "000010");
	table["eqdiv"] = new OpCode("eqdiv", "000011");
	table["eqcmp"] = new OpCode("eqcmp", "000100");
	table["eqand"] = new OpCode("eqand", "000101");
	table["eqor"] = new OpCode("eqor", "000110");
	table["eqnot"] = new OpCode("eqnot", "000111");
	table["eqtest"] = new OpCode("eqtest", "001000");
	table["eqpush"] = new OpCode("eqpush", "001001");
	table["eqpop"] = new OpCode("eqpop", "001010");
	table["eqcall"] = new OpCode("eqcall", "001011");
	table["eqiret"] = new OpCode("eqiret", "001100");
	table["eqmov"] = new OpCode("eqmov", "001101");
	table["eqshl"] = new OpCode("eqshl", "001110");
	table["eqshr"] = new OpCode("eqshr", "001111");
	//pseudo
	table["eqret"] = new OpCode("eqret", "001010");

	//-----------------ne-------------------
	table["neadd"] = new OpCode("neadd", "010000");
	table["nesub"] = new OpCode("nesub", "010001");
	table["nemul"] = new OpCode("nemul", "010010");
	table["nediv"] = new OpCode("nediv", "010011");
	table["necmp"] = new OpCode("necmp", "010100");
	table["neand"] = new OpCode("neand", "010101");
	table["neor"] = new OpCode("neor", "010110");
	table["nenot"] = new OpCode("nenot", "010111");
	table["netest"] = new OpCode("netest", "011000");
	table["nepush"] = new OpCode("nepush", "011001");
	table["nepop"] = new OpCode("nepop", "011010");
	table["necall"] = new OpCode("necall", "011011");
	table["neiret"] = new OpCode("neiret", "011100");
	table["nemov"] = new OpCode("nemov", "011101");
	table["neshl"] = new OpCode("neshl", "011110");
	table["neshr"] = new OpCode("neshr", "011111");
	//pseudo
	table["neret"] = new OpCode("neret", "011010");

	//-----------------gt-------------------
	table["gtadd"] = new OpCode("gtadd", "100000");
	table["gtsub"] = new OpCode("gtsub", "100001");
	table["gtmul"] = new OpCode("gtmul", "100010");
	table["gtdiv"] = new OpCode("gtdiv", "100011");
	table["gtcmp"] = new OpCode("gtcmp", "100100");
	table["gtand"] = new OpCode("gtand", "100101");
	table["gtor"] = new OpCode("gtor", "100110");
	table["gtnot"] = new OpCode("gtnot", "100111");
	table["gttest"] = new OpCode("gttest", "101000");
	table["gtpush"] = new OpCode("gtpush", "101001");
	table["gtpop"] = new OpCode("gtpop", "101010");
	table["gtcall"] = new OpCode("gtcall", "101011");
	table["gtiret"] = new OpCode("gtiret", "101100");
	table["gtmov"] = new OpCode("gtmov", "101101");
	table["gtshl"] = new OpCode("gtshl", "101110");
	table["gtshr"] = new OpCode("gtshr", "101111");
	//pseudo
	table["gtret"] = new OpCode("gtret", "101010");

	//-----------------al-------------------
	table["aladd"] = new OpCode("aladd", "110000");
	table["alsub"] = new OpCode("alsub", "110001");
	table["almul"] = new OpCode("almul", "110010");
	table["aldiv"] = new OpCode("aldiv", "110011");
	table["alcmp"] = new OpCode("alcmp", "110100");
	table["aland"] = new OpCode("aland", "110101");
	table["alor"] = new OpCode("alor", "110110");
	table["alnot"] = new OpCode("alnot", "110111");
	table["altest"] = new OpCode("altest", "111000");
	table["alpush"] = new OpCode("alpush", "111001");
	table["alpop"] = new OpCode("alpop", "111010");
	table["alcall"] = new OpCode("alcall", "111011");
	table["aliret"] = new OpCode("aliret", "111100");
	table["almov"] = new OpCode("almov", "111101");
	table["alshl"] = new OpCode("alshl", "111110");
	table["alshr"] = new OpCode("alshr", "111111");
	//pseudo
	table["alret"] = new OpCode("alret", "111010");
}

OpCodeTable::~OpCodeTable() {
	for (auto const &opcode : table)
		delete opcode.second;
}

OpCode* OpCodeTable::get_opcode(string key) {
	return table[key];
}
