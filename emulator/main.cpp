#include "Emulator.h"
#include "input.h"

#include <thread>
#include <chrono>
#include <iostream>

#define PERIOD 1

using namespace std;
using namespace input;

void periodic_job(Emulator &emulator) {
	cout << "Periodic thread started!" << endl;

	while (!emulator.is_finished()) {
		if (!emulator.get_periodic()) continue;
		cout << "Periodic job" << endl;

		this_thread::sleep_for(chrono::seconds(PERIOD));
	}
	
}

int main(int argc, char *argv[]) {

	if (1) {
		Emulator emulator(argc, argv);
		thread emulator_thread(&Emulator::run, &emulator);
		thread periodic_thread(periodic_job, ref(emulator));

		while (true) {
			// ReSharper disable once CppPossiblyErroneousEmptyStatements
			while (!keyboard_hit());
			char c = get_char();
			if (c == 'q') break;
			std::cout << "Input character: " << c << std::endl;
		}

		emulator_thread.join();
		periodic_thread.join();

		cout << "Emulation completed!" << endl;
	}
	else {

		while (true) {
			// ReSharper disable once CppPossiblyErroneousEmptyStatements
			while (!keyboard_hit());
			char c = get_char();
			if (c == 'q') return 0;
			std::cout << "Input character: " << c << std::endl;
		}

	}

	return 0;
}