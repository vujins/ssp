#include "Emulator.h"

int main(int argc, char *argv[]) {

	if (1) {
		Emulator emulator(argc, argv);

		emulator.read();
		emulator.resolve_conflict();
		emulator.execute();

		emulator.output();

		cout << "Emulation completed!" << endl;
	}
	else {
		uint8_t om[2]{ 0x0F, 0x0F};
		int bytes = 2;
		int address = 0;

		stringstream ss;
		for (int i = bytes - 1; i >= 0; i--) {
			if (om[address + i] < 0x10) ss << '0';
			ss << hex << (int16_t)om[address + i];
		}
		cout << ss.str() << endl;

		cout << hex_to_decimal(ss.str()) << endl;

	}

	return 0;
}