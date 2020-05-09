#ifndef CHIP8_CONTROL
#define CHIP8_CONTROL

#include "glb.h"
#include "io.h"
#include "opc.h"

////	Control

void *counters_decrement(void *args)
{
	struct timespec ts;
	ts.tv_sec = 16.6666/1000;
	ts.tv_nsec = 16.6666 * 1000000;

	while(1)
	{
		if(machine.DT > 0) machine.DT--;
		if(machine.ST > 0) machine.ST--;

		nanosleep(&ts, &ts);
	}
}

void *update_screen(void *args)
{
	struct timespec ts;
	ts.tv_sec = 1/1000;
	ts.tv_nsec = 1*1000000;

	while(1)
	{
		render();

		nanosleep(&ts, &ts);
	}
}

int load_rom(char *path)
{
	FILE *f = fopen(path, "rb");

	if(f == NULL) 
	{
		perror("Error: ");
		return 0;
	}

	uint8_t c;
	int j = 0;
	for(int i = 512; fread(&c, 1, 1, f) == 1; i++, j++)
	{
		machine.mem[i] = c;
	}

	machine.rom_size = j;

	return 1;
}

void init(enum mode m)
{
	machine.PC = 512;
	machine.SP = 15;

    for(int i = 0; i < 80; i++)
        machine.mem[i] = font[i];

	if(m == execute)
	{
		time_t t;
		pthread_t decrementers, screen_refresh;

		srand((unsigned) time(&t));

		pthread_create(&decrementers, NULL, counters_decrement, NULL);
		#ifndef CHIP8_DEBUG
		pthread_create(&screen_refresh, NULL, update_screen, NULL);
		#endif
	}
}

void fde_cycle()
{
	//  fetch instruction
	uint8_t m, x, y, l, kk;
	uint16_t nstr, nnn;
	m = machine.mem[machine.PC] >> 4;
	x = machine.mem[machine.PC] & 0x0f;
	y = machine.mem[machine.PC + 1] >> 4;
	l = machine.mem[machine.PC + 1] & 0x0f;
	kk = machine.mem[machine.PC + 1];
	nnn = x << 8 | kk;

	#ifdef CHIP8_DEBUG
	struct timespec ts;
	ts.tv_sec = 0.2;
	ts.tv_nsec = 200000000;
	
	printf("%x\t\t%x%x%x%x\t%x\n",machine.PC,m,x,y,l,machine.I);

	nanosleep(&ts, &ts);
	#endif

	//  decode and execute
	switch(m)
	{
		case 0x0:
			switch(l)
			{
				case 0x0:
					display_clear();
					break;
				case 0xe:
					flow_return_subroutine();
					break;
				default:
					nop();
					break;
			}
			break;
		case 0x1:
			flow_jmp(no_offset, nnn);
			break;
		case 0x2:
			call(nnn);
			break;
		case 0x3:
			flow_skip_eq(x, kk, regval_const);
			break;
		case 0x4:
			flow_skip_neq(x, kk, regval_const);
			break;
		case 0x5:
			flow_skip_eq(x, y, regval_regval);
			break;
		case 0x6:
			data_move(x, kk, regadr_const);
			break;
		case 0x7:
			math_add(x, kk, regadr_const);
			break;
		case 0x8:
			switch(l)
			{
				case 0x0:
					data_move(x, y, regadr_regval);
					break;
				case 0x1:
					bitwise_or(x, y);
					break;
				case 0x2:
					bitwise_and(x, y);
					break;
				case 0x3:
					bitwise_xor(x, y);
					break;
				case 0x4:
					math_add(x, y, regadr_regval);
					break;
				case 0x5:
					math_sub(x, y, right);
					break;
				case 0x6:
					bitwise_shift_right(x);
					break;
				case 0x7:
					math_sub(x, y, left);
					break;
				case 0xe:
					bitwise_shift_left(x);
					break;
			}
			break;
		case 0x9:
			flow_skip_neq(x, y, regval_regval);
			break;
		case 0xa:
			data_move(I_REG, nnn, iadr_const);
			break;
		case 0xb:
			flow_jmp(offset, nnn);
			break;
		case 0xc:
			math_rand(x, kk);
			break;
		case 0xd:
			display_draw(x, y, l);
			break;
		case 0xe:
			switch(kk)
			{
				case 0x9e:
					flow_skip_keyp(x);
					break;
				case 0xa1:
					flow_skip_keynp(x);
					break;
			}
			break;
		case 0xf:
			switch(kk)
			{
				case 0x07:
					data_move(x, DT_REG, regadr_dtval);
					break;
				case 0x0a:
					data_move(x, KEY, regadr_kval);
					break;
				case 0x15:
					data_move(DT_REG, x, dtadr_regval);
					break;
				case 0x18:
					data_move(ST_REG, x, stadr_regval);
					break;
				case 0x1e:
					math_add(I_REG, x, iadr_regval);
					break;
				case 0x29:
					data_move(I_REG, x, iadr_regval);
					break;
				case 0x33:
					data_move(I_REG, x, iarr_regval);
					break;
				case 0x55:
					data_move(I_REG, x, iarr_regadr);
					break;
				case 0x65:
					data_move(x, I_REG, regarr_iadr);
					break;
			}
			break;
	}

	//  increment PC
	machine.PC += 2;
}

#endif