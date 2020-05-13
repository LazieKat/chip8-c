//#define CHIP8_DEBUG

#include "chip8.h"


int main(int argc, char **argv)
{
	if(argc > 1 && (load_rom(argv[1]) == 0)) return 1;
	else if(argc == 1) return 0;


	if(argc == 3)
	{
		init(disassemble);

		while(machine.rom_size > 0)
		{
			fde_disassemble();
		}

		return 0;
	}
	else
//	load_rom("../games/z.ch8");
	{
        init(execute);

        struct timespec ts;
        ts.tv_sec = 1/1000;
        ts.tv_nsec = 1000000;

		while(1)
		{
            nanosleep(&ts, &ts);
			fde_cycle();
		}
	}
	
	return 0;
}