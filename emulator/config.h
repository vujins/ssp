#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define PC 7
#define SP 6
//PSW flagovi
#define FLAG_Z 0x1 //0001
#define FLAG_O 0x2 //0010
#define FLAG_C 0x4 //0100
#define FLAG_N 0x8 //1000
#define FLAG_I 0x8000 //1000 0000 0000 0000
#define FLAG_periodic 0x1000

//upisom na ovu adresu ispisuje se vrednost na ekranu
#define addr_cout 0xFFFEU
//pritiskom na taster poziva se prekidna rutina na ulazu 3 koja upisuje podatak na ovu adresu
#define addr_data 0xFFFCU

#define OM_SIZE 0x10000 //2^16

#define IVT_SIZE 16
#define IVT_START 0
#define IVT_END 0xF

#define STACK_SIZE 0x1000 //2^12
#define STACK_START 0xEF80 //2^16 - 2^12 - 128
#define STACK_END 0xFF7F //2^16 - 128 - 1

#define IO_START 0xFF80 //2^16 - 128
#define IO_END 0xFFFF //2^16 - 1
#define IO_SIZE 128

//interrupt
enum {
	i_start,
	i_periodic,
	i_illegal_ins,
	i_input
};

#endif