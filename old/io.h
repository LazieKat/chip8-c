#ifndef CHIP8_IO
#define CHIP8_IO

#include "glb.h"

////	I/O

void buf_append(struct db *b, const char *s, int len) {
    char *new = realloc(b->b, b->len + len);

    if (new == NULL) exit(9);

    memcpy(&new[b->len], s, len);
    b->b = new;
    b->len += len;
}

//!TODO implement sound playing
void sound()
{
	if(machine.ST > 0)
	{
		;
	}
	else
	{
		;
	}
}

//!TODO implement key input
uint8_t check_key(uint8_t key)
{
	return 0;
}

//!TODO implement key input
uint8_t read_key()
{
	return 0;
}

void render()
{
    struct db disp_buf = {NULL, 0};

    buf_append(&disp_buf, ASCII_HIDE_CUR, ASCII_HIDE_CUR_LEN);
	buf_append(&disp_buf, ASCII_RESET_CUR_POS, ASCII_RESET_CUR_POS_LEN);
	buf_append(&disp_buf, ASCII_CLEAR_TER, ASCII_CLEAR_TER_LEN);

	for(int y = 0; y < 32; y++)
	{
		for(int x = 0; x < 64; x++)
		{
			if(machine.display[x][y] != 0) buf_append(&disp_buf, ASCII_BG_COLOR, ASCII_BG_LEN);
			else buf_append(&disp_buf, ASCII_FG_COLOR, ASCII_FG_LEN);
		}
		buf_append(&disp_buf, "\n", 1);
	}

	write(STDOUT_FILENO, disp_buf.b, disp_buf.len);
}

#endif