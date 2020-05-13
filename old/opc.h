#ifndef CHIP8_OPCODES
#define CHIP8_OPCODES

#include "glb.h"
#include "io.h"

////	Instructions

void nop()
{
	//	0nnn	sys		$0xNNN
	return;
}

void call(uint16_t adr)
{
	//	2nnn	call	$0xNNN
	machine.stack[machine.SP] = machine.PC;
	machine.PC = adr-2;
	if(--machine.SP == -1) machine.SP = 31;
}

void display_clear()
{
	//	00e0	cls
	for(int i = 0; i < DISP_H; i++) 
		for(int j = 0; j < DISP_W; j++)
			machine.display[j][i] = 0;
}

void display_draw(uint8_t x, uint8_t y, uint8_t h) // 0 1 8
{
	//	dxyn	dsp		@VX, @VY, #0xN
	uint8_t sprite_row, pixel_xor, disp_pixel, sprite_pixel;
	machine.V[0xf] = 0;

	for(int dy = machine.V[y], sy = 0; sy < h; dy++, sy++)
	{
		if(dy >= 32) dy -= 32;	
		sprite_row = machine.mem[machine.I + sy];

		for(int dx = machine.V[x], sx = 7; sx >= 0; dx++, sx--)
		{
			if(dx >= 64) dx -= 64;

			disp_pixel = machine.display[dx][dy];
			sprite_pixel = (sprite_row >> sx) & 0x01;
			pixel_xor = sprite_pixel ^ disp_pixel;

			if(!machine.V[0xf] && disp_pixel && !pixel_xor) machine.V[0xf] = 1;

			machine.display[dx][dy] = pixel_xor;
		}
	}
}

void flow_skip_eq(uint8_t x, uint8_t y, enum mode m)
{
	//	3xkk	seq		@VX, #0xNN			regval_const
	//	5xy0	seq		@VX, @VY			regval_regval

	y = (m == regval_const) ? y : machine.V[y];
	if(machine.V[x] == y) machine.PC += 2;
}

void flow_skip_neq(uint8_t x, uint8_t y, enum mode m)
{
	//	4xkk	sneq	@VX, #0xNN			regval_const
	//	9xy0	sneq	@VX, @VY    		regval_regval

	y = (m == regval_const) ? y : machine.V[y];
	if(machine.V[x] != y) machine.PC += 2;
}

void flow_skip_keyp(uint8_t k)
{
	//	ex9e	skp		@VX
	if(check_key(k)) machine.PC += 2;
}

void flow_skip_keynp(uint8_t k)
{
	//	exa1	sknp	@VX
	if(!check_key(k)) machine.PC += 2;
}

void flow_jmp(enum mode m, uint16_t adr)
{
	//	1nnn	jmp 	$0xNNN				no_offset
	//	bnnn	jmp		@V0, 0xNNN			offset
	machine.PC = adr-2;
	if(m == offset) machine.PC += machine.V[0x0];
}

void flow_return_subroutine()
{
	//	00ee	rts
	if(++machine.SP == 32) machine.SP = 0;
	machine.PC = machine.stack[machine.SP]-2;
}

void bitwise_or(uint8_t x, uint8_t y)
{
	//	8xy1	or		@VX, @VY
	machine.V[x] |= machine.V[y];
}

void bitwise_and(uint8_t x, uint8_t y)
{
	//	8xy2	and		@VX, @VY
	machine.V[x] &= machine.V[y];
}

void bitwise_xor(uint8_t x, uint8_t y)
{
	//	8xy3	xor		@VX, @VY
	machine.V[x] ^= machine.V[y];
}

void bitwise_shift_right(uint8_t x)
{
	//	8xy6	shr		@VX
	machine.V[0xf] = (machine.V[x] & 0x80) >> 7;
	machine.V[x] >>= 1;
}

void bitwise_shift_left(uint8_t x)
{
	//	8xye	shl		@VX
	machine.V[0xf] = machine.V[x] & 0x1;
	machine.V[x] <<= 1;
}

void math_add(uint8_t x, uint8_t y, enum mode m)
{
	//	7xkk	add		$VX, #0xNN			regadr_const
	//	8xy4	add		$VX, @VY			regadr_regval
	//	fx1e	add		$I, @VX				iadr_regval

	switch(m)
	{
		case regadr_const:
			machine.V[x] += y;
			break;
		case regadr_regval:
			machine.V[x] += machine.V[y];
			machine.V[0xf] = (machine.V[x] + machine.V[y] > 0xff) ? 1 : 0;
			break;
		case iadr_regval:
			machine.I += machine.V[x];
			break;
	}
}

void math_sub(uint8_t x, uint8_t y, enum mode d)
{
	//	8xy5	sub		@VX, @VY			right
	//	8xy7	sub		@VX, @VY			left
	switch(d)
	{
		case right:
			machine.V[x] -= machine.V[y];
			machine.V[0xf] = (machine.V[x] > machine.V[y]) ? 1 : 0;
			break;
		case left:
			machine.V[x] = machine.V[y] - machine.V[x];
			machine.V[0xf] = (machine.V[y] > machine.V[x]) ? 1 : 0;
			break;
	}
}

void math_rand(uint8_t x, uint8_t a)
{
	//	cxkk	rnd		@VX, #0xNN
	machine.V[x] = (rand() % 256) & a;
}

void data_move(uint8_t d, uint16_t s, enum mode m)
{
	//	6xkk	mov		$VX, #0xNN			regadr_const
	//	8xy0	mov		$VX, @VY			regadr_regval
	//	annn	mov		$I, #0xNNN			iadr_const
	//	fx07	mov		$VX, @DT			regadr_dtval
	//	fx0a	mov		$VX, @K				regadr_kval
	//	fx15	mov		$DT, @VX			dtadr_regval
	//	fx18	mov		$ST, @VX			stadr_regval
	//	fx29	mov		$I, @VX				iadr_regval
	//	fx33	mov		[I], @VX			iarr_regval
	//	fx55	mov		[I], $VX			iarr_regadr
	//	fx65	mov		[VX], $I			regarr_iadr
	switch(m)
	{
		case regadr_const:
			machine.V[d] = s;
			break;
		case regadr_regval:
			machine.V[d] = machine.V[s];
			break;
		case iadr_const:
			machine.I = s;
			break;
		case regadr_dtval:
			machine.V[d] = machine.DT;
			break;
		case regadr_kval:
			machine.V[d] = read_key();
			break;
		case dtadr_regval:
			machine.DT = machine.V[s];
			break;
		case stadr_regval:
			machine.ST = machine.V[s];
			break;
		case iadr_regval:
			machine.I = machine.V[s]*5;
			break;
		case iarr_regval:
			machine.mem[machine.I] = machine.V[s] / 100;
			machine.mem[machine.I + 1] = (machine.V[s] - machine.mem[machine.I]*100) / 10;
			machine.mem[machine.I + 2] = machine.V[s] - (machine.V[s] / 10 * 10);
			break;
		case iarr_regadr:
			for(int i = 0; i < s; i++) machine.mem[machine.I + i] = machine.V[i];
			break;
		case regarr_iadr:
			for(int i = 0; i < d; i++) machine.V[i] = machine.mem[machine.I + i];
			break;
	}
}

#endif
