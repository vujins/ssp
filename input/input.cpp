#include "input.h"


#ifdef _MSC_VER

#include <conio.h>

#else

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>

#endif

namespace input {

#ifndef _MSC_VER

	struct termios orig_termios;
	bool initialized = false;

	void reset_terminal_mode()
	{
		tcsetattr(0, TCSANOW, &orig_termios);
	}

	void init() {
		initialized = true;

		struct termios new_termios;

		/* take two copies - one for now, one for later */
		tcgetattr(0, &orig_termios);
		memcpy(&new_termios, &orig_termios, sizeof(new_termios));

		/* register cleanup handler, and set the new terminal mode */
		atexit(reset_terminal_mode);
		cfmakeraw(&new_termios);
		tcsetattr(0, TCSANOW, &new_termios);
	}

#endif

	bool input::keyboard_hit() {

#ifdef _MSC_VER

		return _kbhit();

#else
		if (!initialized) init();
		struct timeval tv = { 0L, 0L };
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(0, &fds);
		return select(1, &fds, NULL, NULL, &tv);

#endif
	}

	int input::get_char() {

#ifdef _MSC_VER

		return _getch();

#else 

		if (!initialized) init();
		int r;
		unsigned char c;
		if ((r = read(0, &c, sizeof(c))) < 0) {
			return r;
		}
		else {
			return c;
		}

#endif
	}

}