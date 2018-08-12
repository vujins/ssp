#include "Emulator.h"

int main(int argc, char *argv[]) {

	if (1) {
		Emulator emulator(argc, argv);
		emulator.read();
		emulator.resolve_conflict();
		emulator.output();

		cout << "Emulation completed!" << endl;
	}
	else {
		int b = 3, addr = 1100;
		int data = stoi("50A73B", nullptr, 16);
		cout << data << endl;
		string hex = decimal_to_hex(data);
		for (int i = b; i > 0; i--) {
			int n = hex_to_decimal(hex.substr(2 * (i - 1), 2));
			cout << addr + b - i << " " << n << endl;
		}
	}

	return 0;
}