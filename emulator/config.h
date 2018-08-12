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

#endif