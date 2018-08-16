#include "Emulator.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <windows.h>

#define PERIOD 1

void periodic_job(Emulator &emulator) {
	cout << "Periodic thread started!" << endl;
	
	while (!emulator.is_finished()) {
		cout << "Periodic job" << endl;

		this_thread::sleep_for(chrono::seconds(PERIOD));
	}
	
}


int main(int argc, char *argv[]) {

	if (0) {
		Emulator emulator(argc, argv);
		thread emulator_thread(&Emulator::run, &emulator);
		thread periodic_thread(periodic_job, ref(emulator));
		


		emulator_thread.join();
		periodic_thread.join();

		cout << "Emulation completed!" << endl;
	}
	else {
		//HANDLE hstdin;
		//DWORD  mode;

		//hstdin = GetStdHandle(STD_INPUT_HANDLE);
		//GetConsoleMode(hstdin, &mode);
		//SetConsoleMode(hstdin, ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);  // see note below

		//char c;
		//int i = 0;
		//while (i++ != 4) {
		//	c = cin.get();
		//	if (c == 27) cout << "you pressed ESC" << endl;
		//	else cout << "you didnt press ESC" << endl;
		//}

		//SetConsoleMode(hstdin, mode);

		cout << hex << &periodic_job << endl;

		//Emulator emulator(argc, argv);
		cout << hex << &Emulator::run << endl;
		cout << hex << &Emulator::get_periodic << endl;
		cout << hex << &Emulator::inter << endl;

	}

	return 0;
}