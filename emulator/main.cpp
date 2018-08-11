#include "Emulator.h"

int main(int argc, char *argv[]) {

	Emulator emulator(argc, argv);
	emulator.read();
	emulator.output();

	cout << "Emulation completed!" << endl;

	return 0;
}