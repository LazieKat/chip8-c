#ifndef CHIP8_GLOBALS
#define CHIP8_GLOBALS

#define MEM_SIZE 4096
#define DISP_W 64
#define DISP_H 32
#define SCALE 10

#define I_REG 0
#define DT_REG 0
#define ST_REG 0
#define KEY 0

#define ASCII_HIDE_CUR "\x1b[?25l"
#define ASCII_HIDE_CUR_LEN 6
#define ASCII_CLEAR_TER "\x1b[K"
#define ASCII_CLEAR_TER_LEN 3
#define ASCII_RESET_CUR_POS "\x1b[1;1H"
#define ASCII_RESET_CUR_POS_LEN 6
#define ASCII_FG_COLOR "\x1b[7m  "
#define ASCII_BG_COLOR "\x1b[m  "
#define ASCII_FG_LEN 6
#define ASCII_BG_LEN 5
#define ASCII_RESET_COLOR "\x1b[m"
#define ASCII_RESET_COLOR_LEN 3

#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

////	Globals

struct
{
	uint8_t mem[MEM_SIZE];
	uint8_t V[16];
	uint8_t display[DISP_W][DISP_H];
	uint8_t K, DT, ST;
	uint16_t I, PC, SP;
	int rom_size;
	uint16_t stack[16];
}machine;

struct db
{
    char *b;
    int len;
};

enum mode
{
	regval_const,
	regval_regval,
	regadr_const,
	regadr_regval,
	regadr_dtval,
	regadr_kval,
	regarr_iadr,
	dtadr_regval,
	stadr_regval,
	iadr_const,
	iadr_regval,
	iarr_regval,
	iarr_regadr,
	no_offset,
	offset,
	right,
	left,
	execute,
	disassemble
};

uint16_t font[80] = 
{
	0xf0, 0x90, 0x90, 0x90, 0xf0,
	0x20, 0x60, 0x20, 0x20, 0x70,
	0xf0, 0x10, 0xf0, 0x80, 0xf0,
	0xf0, 0x10, 0xf0, 0x10, 0xf0,
	0x90, 0x90, 0xf0, 0x10, 0x10,
	0xf0, 0x80, 0xf0, 0x10, 0xf0,
	0xf0, 0x80, 0xf0, 0x90, 0xf0,
	0xf0, 0x10, 0x20, 0x40, 0x40,
	0xf0, 0x90, 0xf0, 0x90, 0xf0,
	0xf0, 0x90, 0xf0, 0x10, 0xf0,
	0xf0, 0x90, 0xf0, 0x90, 0x90,
	0xe0, 0x90, 0xe0, 0x90, 0xe0,
	0xf0, 0x80, 0x80, 0x80, 0xf0,
	0xe0, 0x90, 0x90, 0x90, 0xe0,
	0xf0, 0x80, 0xf0, 0x80, 0xf0,
	0xf0, 0x80, 0xf0, 0x80, 0x80
};

#endif