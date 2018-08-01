#include "OpCode.h"

regex OpCode::regex_char(".char");
regex OpCode::regex_word(".word");
regex OpCode::regex_long(".long");
regex OpCode::regex_skip(".skip");
regex OpCode::regex_align(".align");


OpCode::OpCode(string name_, string opcode_):
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
	if (regex_search(line, regex_char)) return 1;
	if (regex_search(line, regex_word)) return 2;
	if (regex_search(line, regex_long)) return 4;
	if (regex_search(line, regex_skip)) {
		int position = line.rfind(" ");
		return stoi(line.substr(position));
	}
	if (regex_search(line, regex_align)) {
		int position = line.rfind(" ");
		int align = stoi(line.substr(position));
		return (align - lc % align) % align;
	}

	return 0;
}

OpCodeTable::OpCodeTable() {
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
}

OpCodeTable::~OpCodeTable() {
	for (auto const &opcode : table)
		delete opcode.second;
}
