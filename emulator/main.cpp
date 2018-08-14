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

	}

	return 0;
}