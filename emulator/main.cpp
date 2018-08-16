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
		emulator.interrupt(i_periodic);

		this_thread::sleep_for(chrono::seconds(PERIOD));
	}

}

int main(int argc, char *argv[]) {

	Emulator emulator(argc, argv);
	thread emulator_thread(&Emulator::run, &emulator);
	thread periodic_thread(periodic_job, ref(emulator));

	while (true) {
		while (!keyboard_hit());
		char c = get_char();
		if (c == 'q') break;
		emulator.input_char(c);
		emulator.interrupt(i_input);
	}

	periodic_thread.join();
	emulator_thread.join();

	cout << "Emulation completed!" << endl;

	return 0;
}