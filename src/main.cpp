////	includes
#include <cstdint>
#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <thread>
#include <unistd.h>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

////	typedefs
typedef struct // cpu registers, memory and counters
{
	uint8_t V[16];

	uint8_t mem[4096];
	uint16_t PC;
	uint16_t I;
	
	uint16_t stack[16];
	uint8_t SP;

	uint8_t DT;
	uint8_t ST;

	uint8_t display[64][32];

	uint8_t K[16];

	int rom_size;
}cpu;

typedef union // opcode with flags for each word
{
	struct
	{
		uint8_t r : 4;
		uint8_t y : 4;
		uint8_t x : 4;
		uint8_t l : 4;
	}b;

	uint16_t nnn : 12;
	uint8_t kk : 8;

	uint16_t opcode;
}opcode;

typedef struct
{
	sf::RenderWindow window;
	sf::RectangleShape bg;
	sf::RectangleShape bs;
	sf::RectangleShape currentSP;
	sf::RectangleShape pixel[64*32];
}win;

typedef struct
{
	sf::Font fnt;
	sf::Text stack_vals;
	sf::Text reg_title;
	sf::Text reg_vals[16];
	sf::Text key_title;
	sf::Text key_vals[16];
	sf::Text I;
	sf::Text ST;
	sf::Text DT;
	sf::Text SP;
	sf::Text PC;
	std::stringstream sv;
}txt;

//sf::Vector2i px(32, 16);

////	declearations

void init(win *mw, cpu *m, txt *ot, char *file);

////	functions

void openRom(cpu *m, char *p)
{
	FILE *f = fopen(p, "rb");
	if(f == NULL) exit(1);

	m->rom_size = 0;

	uint8_t b;
	for(int i = 512; fread(&b, 1, 1, f); i++, m->rom_size++)
		m->mem[i] = b;

	fclose(f);
}

void pollEvents(win *mw, cpu *m, txt *ot = NULL, char *fn = NULL)
{
	sf::Event evt;
	while(mw->window.pollEvent(evt))
	{
		switch(evt.type)
		{
			case sf::Event::Closed:
				mw->window.close();
				break;
			case sf::Event::KeyPressed:
				switch(evt.key.code)
				{
					case sf::Keyboard::Num1:
						m->K[1] = 1;
						break;
					case sf::Keyboard::Num2:
						m->K[2] = 1;
						break;
					case sf::Keyboard::Num3:
						m->K[3] = 1;
						break;
					case sf::Keyboard::Num4:
						m->K[0xc] = 1;
						break;
					case sf::Keyboard::Q:
						m->K[4] = 1;
						break;
					case sf::Keyboard::W:
						m->K[5] = 1;
						break;
					case sf::Keyboard::E:
						m->K[6] = 1;
						break;
					case sf::Keyboard::R:
						m->K[0xd] = 1;
						break;
					case sf::Keyboard::A:
						m->K[7] = 1;
						break;
					case sf::Keyboard::S:
						m->K[8] = 1;
						break;
					case sf::Keyboard::D:
						m->K[9] = 1;
						break;
					case sf::Keyboard::F:
						m->K[0xe] = 1;
						break;
					case sf::Keyboard::Z:
						m->K[0xa] = 1;
						break;
					case sf::Keyboard::X:
						m->K[0] = 1;
						break;
					case sf::Keyboard::C:
						m->K[0xb] = 1;
						break;
					case sf::Keyboard::V:
						m->K[0xf] = 1;
						break;
					case sf::Keyboard::Backspace:
						init(mw, m, ot, fn);
						break;
				}
				break;
			case sf::Event::KeyReleased:
				switch(evt.key.code)
				{
					case sf::Keyboard::Num1:
						m->K[1] = 0;
						break;
					case sf::Keyboard::Num2:
						m->K[2] = 0;
						break;
					case sf::Keyboard::Num3:
						m->K[3] = 0;
						break;
					case sf::Keyboard::Num4:
						m->K[0xc] = 0;
						break;
					case sf::Keyboard::Q:
						m->K[4] = 0;
						break;
					case sf::Keyboard::W:
						m->K[5] = 0;
						break;
					case sf::Keyboard::E:
						m->K[6] = 0;
						break;
					case sf::Keyboard::R:
						m->K[0xd] = 0;
						break;
					case sf::Keyboard::A:
						m->K[7] = 0;
						break;
					case sf::Keyboard::S:
						m->K[8] = 0;
						break;
					case sf::Keyboard::D:
						m->K[9] = 0;
						break;
					case sf::Keyboard::F:
						m->K[0xe] = 0;
						break;
					case sf::Keyboard::Z:
						m->K[0xa] = 0;
						break;
					case sf::Keyboard::X:
						m->K[0] = 0;
						break;
					case sf::Keyboard::C:
						m->K[0xb] = 0;
						break;
					case sf::Keyboard::V:
						m->K[0xf] = 0;
						break;
				}
				break;
			case sf::Event::TextEntered:
				break;
			default:
				break;
		}
	}
}

std::string intToHex(uint64_t i, bool p=true)
{
	std::stringstream stream;
	std::string pre = p ? "0x" : "";

	stream << std::hex << i;
	std::string hex = stream.str();

	std::string filler = (hex.length() % 2 == 0) ? "" : "0";

	stream.str("");
	stream << pre << filler << hex;
	
	return stream.str();
}

void updateObjects(win *mw, cpu *m, txt *ot)
{
	// stack values
	ot->sv.str("");
	for(int i = 15; i >= 0; i--)
		ot->sv << intToHex(m->stack[i]) << "\n";
	ot->stack_vals.setString(ot->sv.str());
	mw->currentSP.setPosition(sf::Vector2f(546, 336+18*abs(m->SP-15)));

	// registers and key values
	for(int j = 0; j < 3; j++)
	{
		for(int i = 0; i < 6; i++)
		{
			if(j == 2 && i > 3) 
				break;

			ot->reg_vals[i+j*6].setString(intToHex(m->V[i+j*6], false));
			ot->key_vals[i+j*6].setString(intToHex(m->K[i+j*6], false));
		}
	}

	// index register
	ot->I.setString(intToHex(m->I, false));

	// sound timer
	ot->ST.setString(std::to_string(m->ST));

	// delay timer
	ot->DT.setString(std::to_string(m->DT));

	// stack pointer
	ot->SP.setString(intToHex(m->SP, false));

	// program counter
	ot->PC.setString(intToHex(m->PC, false));
}

void drawObjects(win *mw, txt *ot)
{
	//mw->window.clear(sf::Color::Black);
	mw->window.draw(mw->bg);	// background
	mw->window.draw(mw->bs);	// background

	mw->window.draw(mw->currentSP);	// stack indicator
	mw->window.draw(ot->stack_vals); // stack values

	mw->window.draw(ot->reg_title); // registers names
	mw->window.draw(ot->key_title); // registers names
	for(int i = 0; i < 16; i++)
	{
		mw->window.draw(ot->reg_vals[i]);
		mw->window.draw(ot->key_vals[i]);
	}

	// other pseudo registers
	mw->window.draw(ot->I);
	mw->window.draw(ot->ST);
	mw->window.draw(ot->DT);
	mw->window.draw(ot->SP);
	mw->window.draw(ot->PC);
	
	// actual chip-8 display
	for(int i = 0; i < 64*32; i++)
		if(mw->pixel[i].getFillColor() == sf::Color::White)
			mw->window.draw(mw->pixel[i]);
}

void decrementCounters(cpu *m)
{
	struct timespec ts;
	ts.tv_sec = 16.6666/1000;
	ts.tv_nsec = 16.6666 * 1000000;

	while(1)
	{
		if(m->DT > 0) m->DT--;
		if(m->ST > 0) m->ST--;

		nanosleep(&ts, &ts);
	}
}

void fdeCycle(win *mw, cpu *m)
{
	opcode op;
	op.opcode = (m->mem[m->PC] << 8) | (m->mem[m->PC+1]);
	int OSP;

	switch(op.b.l)
	{
		case 0x0:
			switch(op.b.r)
			{
				case 0x0:	// clear display
					for(int j = 0; j < 32; j++)
						for(int i = 0; i < 64; i++)
							m->display[i][j] = 0;
					break;
				case 0xe:	// return from subroutine
					m->PC = m->stack[m->SP];

					OSP = m->SP;
					m->SP--;
					if(OSP == 0x00 && m->SP == 0xff)
						m->SP = 15;  // set to 0 to disable loopback
					break;
			}
			break;
		case 0x1:			// jump to nnn (subtract 2 for alignment)
			m->PC = op.nnn - 2;
			break;
		case 0x2:			// call subroutine at nnn
			OSP = m->SP;
			m->SP++;
			if(OSP == 0x0f && m->SP == 0x10)
				m->SP = 0;  // set to 15 to disable loopback
			
			m->stack[m->SP] = m->PC;
			m->PC = op.nnn - 2;
			break;
		case 0x3:			// skip if equal
			if(m->V[op.b.x] == op.kk)
				m->PC += 2;
			break;
		case 0x4:			// skip if not equal
			if(m->V[op.b.x] != op.kk)
				m->PC += 2;
			break;
		case 0x5:			// skip if equal
			if(m->V[op.b.x] == m->V[op.b.y])
				m->PC += 2;
			break;
		case 0x6:			// move byte
			m->V[op.b.x] = op.kk;
			break;
		case 0x7:			// add byte
			m->V[op.b.x] += op.kk;
			break;
		case 0x8:
			switch(op.b.r)
			{
				case 0x0:	// move register
					m->V[op.b.x] = m->V[op.b.y];
					break;
				case 0x1:	// OR registers
					m->V[op.b.x] |= m->V[op.b.y];
					break;
				case 0x2:	// AND registers
					m->V[op.b.x] &= m->V[op.b.y];
					break;
				case 0x3:	// XOR registers
					m->V[op.b.x] ^= m->V[op.b.y];
					break;
				case 0x4:	// add with carry
					if(m->V[op.b.x] + m->V[op.b.y] > 255)
						m->V[0xf] = 1;
					else
						m->V[0xf] = 0;
					m->V[op.b.x] += m->V[op.b.y];
					break;
				case 0x5:	// sub NOT borrow
					if(!(m->V[op.b.y] > m->V[op.b.x]))
						m->V[0xf] = 1;
					else
						m->V[0xf] = 0;
					m->V[op.b.x] -= m->V[op.b.y];
					break;
				case 0x6:	// shift right
					if(m->V[op.b.x] | 0x01)
						m->V[0xf] = 1;
					else
						m->V[0xf] = 0;
					m->V[op.b.x] >>= 1;
					break;
				case 0x7:	// subn NOT borrow
					if(!(m->V[op.b.x] > m->V[op.b.y]))
						m->V[0xf] = 1;
					else
						m->V[0xf] = 0;
					m->V[op.b.x] = m->V[op.b.y] - m->V[op.b.x];
					break;
				case 0xe:
					if(m->V[op.b.x] | 0x80)
						m->V[0xf] = 1;
					else
						m->V[0xf] = 0;
					m->V[op.b.x] <<= 1;
					break;
			}
			break;
		case 0x9:			// skip if not equal
			if(m->V[op.b.x] != m->V[op.b.y])
				m->PC += 2;
			break;
		case 0xa:			// move byte to I
			m->I = op.nnn;
			break;
		case 0xb:			// jump with offset
			m->PC = op.nnn + m->V[0x0] - 2;
			break;
		case 0xc:			// random AND 
			m->V[op.b.x] = (rand() % 256) & op.kk;
			break;
		case 0xd:			// display sprite
			m->V[0xf] = 0;
			for(int dy = m->V[op.b.y], sy = 0; sy < op.b.r; dy++, sy++)
			{
				if(dy >= 32)
					dy -= 32;
				uint8_t sprite_row = m->mem[m->I + sy];

				for(int dx = m->V[op.b.x], sx = 7; sx >= 0; dx++, sx--)
				{
					if(dx >= 64)
						dx -= 64;
					uint8_t current_pixel = m->display[dx][dy];
					uint8_t sprite_pixel = (sprite_row >> sx) & 0x1;
					uint8_t px = sprite_pixel ^ current_pixel;

					if(current_pixel && !px)
						m->V[0xf] = 1;
					
					m->display[dx][dy] = px;
					
					if(px)
						mw->pixel[dx+dy*64].setFillColor(sf::Color::White);
					else
						mw->pixel[dx+dy*64].setFillColor(sf::Color::Black);
					//mw->window.draw(mw->pixel[dx+dy*64]);
				}
			}
			break;
		case 0xe:
			switch(op.kk)
			{
				case 0x9e:	// skip if key is pressed
					if(m->K[m->V[op.b.x]])
						m->PC += 2;
					break;
				case 0xa1:	// skip if key is not pressed
					if(!m->K[m->V[op.b.x]])
						m->PC += 2;
					break;
			}
			break;
		case 0xf:
			switch(op.kk)
			{
				case 0x07:
					m->V[op.b.x] = m->DT;
					break;
				case 0x0a:
					{
						uint8_t CK[16];
						for(int i = 0; i < 16; i++)
							CK[i] = m->K[i];
						bool done = false;
						while(!done)
						{
							pollEvents(mw, m);
							for(int i = 0; i < 16; i++)
							{
								if(CK[i] != m->K[i])
								{
									m->V[op.b.x] = i;
									done = true;
									break;
								}
							}
						}
					}
					break;
				case 0x15:
					m->DT = m->V[op.b.x];
					break;
				case 0x18:
					m->ST = m->V[op.b.x];
					break;
				case 0x1e:
					m->I += m->V[op.b.x];
					break;
				case 0x29:
					m->I = m->V[op.b.x]*5;
					break;
				case 0x33:
					m->mem[m->I] = m->V[op.b.x] / 100;
					m->mem[m->I + 1] = (m->V[op.b.x] - m->mem[m->I]*100) / 10;
					m->mem[m->I + 2] = m->V[op.b.x] - (m->V[op.b.x] / 10 * 10);
					break;
				case 0x55:
					for(int i = 0; i < op.b.x; i++)
						m->mem[m->I + i] = m->V[i];
					break;
				case 0x65:
					for(int i = 0; i < op.b.x; i++)
						m->V[i] = m->mem[m->I + i];
					break;
			}
			break;
	}

	m->PC += 2;
}

void init(win *mw, cpu *m, txt *ot, char *file = NULL)
{
	////	CPU init
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

	for(int i = 0; i < 16; i++)
		m->V[i] = 0;
	
	for(int i = 80; i < 4096; i++)
		m->mem[i] = 0;

	for(int i = 0; i < 80; i++)
		m->mem[i] = font[i];

	m->PC = 512;
	m->I = 0;

	for(int i = 0; i < 16; i++)
	{
		m->stack[i] = 0;
		m->K[i] = 0;
	}

	m->SP = 0;

	m->DT = 0;
	m->ST = 0;

	for(int j = 0; j < 32; j++)
		for(int i = 0; i < 64; i++)
			m->display[i][j] = 0;

	////	Window init
	mw->window.create(sf::VideoMode(640, 640), "CHIP-8", sf::Style::Close);
	//mw->window.setFramerateLimit(500);

	mw->bg = sf::RectangleShape(sf::Vector2f(640, 320));
	mw->bg.setFillColor(sf::Color(127, 127, 127));
	mw->bg.setPosition(sf::Vector2f(0,320));

	mw->bs = sf::RectangleShape(sf::Vector2f(640, 320));
	mw->bs.setFillColor(sf::Color::Black);
	mw->bs.setPosition(sf::Vector2f(0,0));
	
	mw->currentSP = sf::RectangleShape(sf::Vector2f(61, 18));
	mw->currentSP.setFillColor(sf::Color(53, 81, 117));

	for(int i = 0; i < 64*32; i++)
		mw->pixel[i] = sf::RectangleShape(sf::Vector2f(10, 10));

	for(int j = 0; j < 32; j++)
	{
		for(int i = 0; i < 64; i++)
		{
			mw->pixel[i+j*64].setPosition(sf::Vector2f(i*10, j*10));
			mw->pixel[i+j*64].setFillColor(sf::Color::Black);
		}
	}

	////	Textual objects init
	ot->fnt.loadFromFile("C:/Users/ahmad/OneDrive/Documents/GitHub/chip8-c/src/font.ttf");
	ot->stack_vals = sf::Text("", ot->fnt, 18);
	ot->reg_title = sf::Text("V0\tV1\tV2\tV3\tV4\tV5\n\n\n\nV6\tV7\tV8\tV9\tVA\tVB\n\n\n\nVC\tVD\tVE\tVF\n\n\n\nI\t   DT\tST\tSP\tPC", ot->fnt, 20);
	ot->key_title = sf::Text("K0\tK1\tK2\tK3\tK4\tK5\n\n\n\nK6\tK7\tK8\tK9\tKA\tKB\n\n\n\nKC\tKD\tKE\tKF", ot->fnt, 20);
	ot-> I = sf::Text("", ot->fnt, 20);
	ot-> ST = sf::Text("", ot->fnt, 20);
	ot->DT = sf::Text("", ot->fnt, 20);
	ot->SP = sf::Text("", ot->fnt, 20);
	ot->PC = sf::Text("", ot->fnt, 20);
	ot->stack_vals.setPosition(sf::Vector2f(550, 332));
	ot->reg_title.setPosition(sf::Vector2f(300, 332));
	ot->key_title.setPosition(sf::Vector2f(40, 332));
	ot->PC.setPosition(sf::Vector2f(460 ,594));
	ot->SP.setPosition(sf::Vector2f(420 ,594));
	ot->DT.setPosition(sf::Vector2f(380 ,594));
	ot->ST.setPosition(sf::Vector2f(340 ,594));
	ot->I.setPosition(sf::Vector2f(300 , 594));
	for(int i = 0; i < 16; i++)
	{
		ot->reg_vals[i] = sf::Text("", ot->fnt, 20);
		ot->key_vals[i] = sf::Text("", ot->fnt, 20);
	}

	for(int j = 0; j < 3; j++)
	{
		for(int i = 0; i < 6; i++)
		{
			if(j == 2 && i > 3) 
				break;
			ot->reg_vals[i+j*6].setPosition(sf::Vector2f(300+i*40, 360+j*78));
			ot->key_vals[i+j*6].setPosition(sf::Vector2f(40+i*40, 360+j*78));
		}
	}

	////	General
	sf::err().rdbuf(NULL);
	std::srand(std::time(NULL));

	if(file != NULL)
		openRom(m, file);
}

////	main
int main(int argc, char **argv)
{
	cpu m;
	win mw;
	txt ot;
	std::string zz;

	char *fn;

	if(argc > 1)
		fn = argv[1];
	else
		fn = NULL;

	init(&mw, &m, &ot, fn);
	std::thread decrementers(decrementCounters, &m);

	struct timespec ts;
	ts.tv_sec = 0.001;
	ts.tv_nsec = 0.001 * 1000000000;

	////	game loop
	while(mw.window.isOpen())
	{
		if(argc == 3)
			getline(std::cin, zz);

		fdeCycle(&mw, &m);
		pollEvents(&mw, &m, &ot, fn);
		updateObjects(&mw, &m, &ot);
		drawObjects(&mw, &ot);
		mw.window.display();

		nanosleep(&ts, &ts);
	}

	return 0;
}
