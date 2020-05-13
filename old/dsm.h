#ifndef CHIP8_DISASSEMBLE
#define CHIP8_DISASSEMBLE

#include "glb.h"
#include "io.h"
#include "opc.h"

////	Disassembly

void fde_disassemble()
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

	//  decode and print
	switch(m)
	{
		case 0x0:
			switch(l)
			{
				case 0x0:
					printf("%x\t%x%x%x%x\t\tcls\n",machine.PC,m,x,y,l);
					break;
				case 0xe:
					printf("%x\t%x%x%x%x\t\trts\n",machine.PC,m,x,y,l);
					break;
				default:
					printf("%x\t%x%x%x%x\t\tnop\n",machine.PC,m,x,y,l);
					break;
			}
			break;
		case 0x1:
			printf("%x\t%x%x%x%x\t\tjmp\t$0x%x\n",machine.PC,m,x,y,l,nnn);
			break;
		case 0x2:
			printf("%x\t%x%x%x%x\t\tcall\t$0x%x\n",machine.PC,m,x,y,l,nnn);
			break;
		case 0x3:
			printf("%x\t%x%x%x%x\t\tseq\t@V%x, #0x%x\n",machine.PC,m,x,y,l,x,kk);
			break;
		case 0x4:
			printf("%x\t%x%x%x%x\t\tsneq\t@V%x, #0x%x\n",machine.PC,m,x,y,l,x,kk);
			break;
		case 0x5:
			printf("%x\t%x%x%x%x\t\tseq\t@V%x, @V%x\n",machine.PC,m,x,y,l,x,y);
			break;
		case 0x6:
			printf("%x\t%x%x%x%x\t\tmov\t@V%x, #0x%x\n",machine.PC,m,x,y,l,x, kk);
			break;
		case 0x7:
			printf("%x\t%x%x%x%x\t\tadd\t@V%x, #0x%x\n",machine.PC,m,x,y,l,x, kk);
			break;
		case 0x8:
			switch(l)
			{
				case 0x0:
					printf("%x\t%x%x%x%x\t\tmov\t@V%x, @V%x\n",machine.PC,m,x,y,l,x,y);
					break;
				case 0x1:
					printf("%x\t%x%x%x%x\t\tor\t@V%x, @V%x\n",machine.PC,m,x,y,l,x,y);
					break;
				case 0x2:
					printf("%x\t%x%x%x%x\t\tand\t@V%x, @V%x\n",machine.PC,m,x,y,l,x,y);
					break;
				case 0x3:
					printf("%x\t%x%x%x%x\t\txor\t@V%x, @V%x\n",machine.PC,m,x,y,l,x,y);
					break;
				case 0x4:
					printf("%x\t%x%x%x%x\t\tadd\t@V%x, @V%x\n",machine.PC,m,x,y,l,x,y);
					break;
				case 0x5:
					printf("%x\t%x%x%x%x\t\tsub\t@V%x, @V%x\n",machine.PC,m,x,y,l,x,y);
					break;
				case 0x6:
					printf("%x\t%x%x%x%x\t\tshr\t@V%x\n",machine.PC,m,x,y,l,x);
					break;
				case 0x7:
					printf("%x\t%x%x%x%x\t\tsubl\t@V%x, @V%x\n",machine.PC,m,x,y,l,x,y);
					break;
				case 0xe:
					printf("%x\t%x%x%x%x\t\tshl\t@V%x\n",machine.PC,m,x,y,l,x);
					break;
			}
			break;
		case 0x9:
			printf("%x\t%x%x%x%x\t\tsneq\t@V%x, @V%x\n",machine.PC,m,x,y,l,x,y);
			break;
		case 0xa:
			printf("%x\t%x%x%x%x\t\tmov\t$I, #0x%x\n",machine.PC,m,x,y,l,nnn);
			break;
		case 0xb:
			printf("%x\t%x%x%x%x\t\tjmp\t@V0, $0x%x\n",machine.PC,m,x,y,l,nnn);
			break;
		case 0xc:
			printf("%x\t%x%x%x%x\t\trnd\t@V%x, #0x%x\n",machine.PC,m,x,y,l,x,kk);
			break;
		case 0xd:
			printf("%x\t%x%x%x%x\t\tdsp\t@V%x, @V%x, #0x%x\n",machine.PC,m,x,y,l,x,y,l);
			break;
		case 0xe:
			switch(kk)
			{
				case 0x9e:
					printf("%x\t%x%x%x%x\t\tskp\t@V%x\n",machine.PC,m,x,y,l,x);
					break;
				case 0xa1:
					printf("%x\t%x%x%x%x\t\tsknp\t@V%x\n",machine.PC,m,x,y,l,x);
					break;
			}
			break;
		case 0xf:
			switch(kk)
			{
				case 0x07:
					printf("%x\t%x%x%x%x\t\tmov\t$V%x, @DT\n",machine.PC,m,x,y,l,x);
					break;
				case 0x0a:
					printf("%x\t%x%x%x%x\t\tmov\t$V%x, $K\n",machine.PC,m,x,y,l,x);
					break;
				case 0x15:
					printf("%x\t%x%x%x%x\t\tmov\t$DT, @V%x\n",machine.PC,m,x,y,l,x);
					break;
				case 0x18:
					printf("%x\t%x%x%x%x\t\tmov\t$ST, @V%x\n",machine.PC,m,x,y,l,x);
					break;
				case 0x1e:
					printf("%x\t%x%x%x%x\t\tadd\t$I, @V%x\n",machine.PC,m,x,y,l,x);
					break;
				case 0x29:
					printf("%x\t%x%x%x%x\t\tmov\t$I, @V%x\n",machine.PC,m,x,y,l,x);
					break;
				case 0x33:
					printf("%x\t%x%x%x%x\t\tmov\t[I], @V%x\n",machine.PC,m,x,y,l,x);
					break;
				case 0x55:
					printf("%x\t%x%x%x%x\t\tmov\t[I], $V%x\n",machine.PC,m,x,y,l,x);
					break;
				case 0x65:
					printf("%x\t%x%x%x%x\t\tmov\t[V%x], $I\n",machine.PC,m,x,y,l,x);
					break;
			}
			break;
	}

	//  increment PC
	machine.PC += 2;
	machine.rom_size -=2;
}

#endif