#include "Emulator.h"
#include <thread>

int main(int argc, char *argv[]) {

	if (1) {
		Emulator emulator(argc, argv);
		thread thread(&Emulator::run, &emulator);
		



		thread.join();

		cout << "Emulation completed!" << endl;
	}
	else {

	}

	return 0;
}