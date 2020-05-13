////	includes
#include <cstdint>
#include <iostream>
#include <string>
#include <iomanip>
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

	int rom_size;
}cpu;

typedef union // opcode with flags for each word
{
	struct
	{
		uint8_t q : 4;
		uint8_t w : 4;
		uint8_t e : 4;
		uint8_t r : 4;
	}b;

	uint16_t nnn : 12;
	uint8_t kk : 8;

	uint16_t opcode;
}opcode;

typedef struct
{
	sf::RenderWindow window;
	sf::RectangleShape bg;
	sf::RectangleShape currentSP;
	sf::RectangleShape pixel[64*32];
}win;

typedef struct
{
	sf::Font fnt;
	sf::Text stack_vals;
	sf::Text reg_title;
	sf::Text reg_vals[16];
	sf::Text I;
	sf::Text ST;
	sf::Text DT;
	sf::Text SP;
	sf::Text PC;
	std::stringstream sv;
}txt;

////	functions
void init(cpu *m, win *mw, txt *ot)
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
	
	for(int i = 0; i < 80; i++)
		m->mem[i] = font[i];

	m->PC = 512;
	m->I = 0;

	for(int i = 0; i < 16; i++)
		m->stack[i] = 0;

	m->SP = 15;

	m->DT = 0;
	m->ST = 0;

	for(int j = 0; j < 32; j++)
		for(int i = 0; i < 64; i++)
			m->display[i][j] = 0;

	////	Window init
	mw->window.create(sf::VideoMode(640, 640), "CHIP-8", sf::Style::Close);
	mw->window.setFramerateLimit(60);

	mw->bg = sf::RectangleShape(sf::Vector2f(640, 320));
	mw->bg.setFillColor(sf::Color::Black);
	mw->bg.setPosition(sf::Vector2f(0,0));
	
	mw->currentSP = sf::RectangleShape(sf::Vector2f(61, 18));
	mw->currentSP.setFillColor(sf::Color(53, 81, 117));

	for(int i = 0; i < 64*32; i++)
		mw->pixel[i] = sf::RectangleShape(sf::Vector2f(10, 10));


	////	Textual objects init
	ot->fnt.loadFromFile("font.ttf");
	ot->stack_vals = sf::Text("", ot->fnt, 18);
	ot->reg_title = sf::Text("V0\tV1\tV2\tV3\tV4\tV5\n\n\n\nV6\tV7\tV8\tV9\tVA\tVB\n\n\n\nVC\tVD\tVE\tVF\n\n\n\nI\t   DT\tST\tPC\tSP", ot->fnt, 20);
	ot-> I = sf::Text("", ot->fnt, 20);
	ot-> ST = sf::Text("", ot->fnt, 20);
	ot->DT = sf::Text("", ot->fnt, 20);
	ot->SP = sf::Text("", ot->fnt, 20);
	ot->PC = sf::Text("", ot->fnt, 20);
	ot->stack_vals.setPosition(sf::Vector2f(550, 332));
	ot->reg_title.setPosition(sf::Vector2f(300, 332));
	for(int i = 0; i < 16; i++)
		ot->reg_vals[i] = sf::Text("", ot->fnt, 20);


	////	General
	sf::err().rdbuf(NULL);
}

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

void pollEvents(win *mw, cpu *m)
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
						std::cout << "1" << std::endl;
						break;
					case sf::Keyboard::Num2:
						std::cout << "2" << std::endl;
						break;
					case sf::Keyboard::Num3:
						std::cout << "3" << std::endl;
						break;
					case sf::Keyboard::Num4:
						std::cout << "C" << std::endl;
						break;
					case sf::Keyboard::Q:
						std::cout << "4" << std::endl;
						m->SP--;
						break;
					case sf::Keyboard::W:
						std::cout << "5" << std::endl;
						sz.y++;
						break;
					case sf::Keyboard::E:
						std::cout << "6" << std::endl;
						m->SP++;
						break;
					case sf::Keyboard::R:
						std::cout << "D" << std::endl;
						break;
					case sf::Keyboard::A:
						std::cout << "7" << std::endl;
						sz.x--;
						break;
					case sf::Keyboard::S:
						std::cout << "8" << std::endl;
						sz.y--;
						break;
					case sf::Keyboard::D:
						std::cout << "9" << std::endl;
						sz.x++;
						break;
					case sf::Keyboard::F:
						std::cout << "E" << std::endl;
						break;
					case sf::Keyboard::Z:
						std::cout << "A" << std::endl;
						break;
					case sf::Keyboard::X:
						std::cout << "0" << std::endl;
						break;
					case sf::Keyboard::C:
						std::cout << "B" << std::endl;
						break;
					case sf::Keyboard::V:
						std::cout << "F" << std::endl;
						break;
					case sf::Keyboard::Up:
						tp.y--;
						break;
					case sf::Keyboard::Down:
						tp.y++;
						break;
					case sf::Keyboard::Left:
						tp.x--;
						break;
					case sf::Keyboard::Right:
						tp.x++;
						break;

					case sf::Keyboard::I:
						px.y--;
						m->display[px.x][px.y] = !m->display[px.x][px.y];
						break;
					case sf::Keyboard::K:
						px.y++;
						m->display[px.x][px.y] = !m->display[px.x][px.y];
						break;
					case sf::Keyboard::L:
						px.x++;
						m->display[px.x][px.y] = !m->display[px.x][px.y];
						break;
					case sf::Keyboard::J:
						px.x--;
						m->display[px.x][px.y] = !m->display[px.x][px.y];
						break;
				}

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

void updateObjects(cpu *m, win *mw, txt *ot)
{
	// stack values
	ot->sv.str("");
	for(int i = 15; i >= 0; i--)
		ot->sv << intToHex(m->stack[i]) << "\n";
	ot->stack_vals.setString(ot->sv.str());
	mw->currentSP.setPosition(sf::Vector2f(546, 336+18*abs(m->SP-15)));

	// registers values
	for(int j = 0; j < 3; j++)
	{
		for(int i = 0; i < 6; i++)
		{
			if(j == 2 && i > 3) 
				break;

			ot->reg_vals[i+j*6].setString(intToHex(m->V[i+j*6], false));
			ot->reg_vals[i+j*6].setPosition(sf::Vector2f(300+i*40, 360+j*78));
		}
	}

	// index register
	ot->I .setString(intToHex(m->I, false));
	ot->I.setPosition(sf::Vector2f(300 , 594));

	// sound timer
	ot->ST.setString(intToHex(m->ST, false));
	ot->ST.setPosition(sf::Vector2f(340 ,594));

	// delay timer
	ot->DT.setString(intToHex(m->DT, false));
	ot->DT.setPosition(sf::Vector2f(380 ,594));

	// stack pointer
	ot->SP.setString(intToHex(m->SP, false));
	ot->SP.setPosition(sf::Vector2f(420 ,594));

	// program counter
	ot->PC.setString(intToHex(m->PC, false));
	ot->PC.setPosition(sf::Vector2f(460 ,594));

	// actual chip-8 display
	for(int j = 0; j < 32; j++)
	{
		for(int i = 0; i < 64; i++)
		{
			mw->pixel[i+j*64].setPosition(sf::Vector2f(i*10, j*10));
			if(m->display[i][j])
				mw->pixel[i+j*64].setFillColor(sf::Color::White);
			else
				mw->pixel[i+j*64].setFillColor(sf::Color::Black);
		}
	}
}

void drawObjects(win *mw, txt *ot)
{
	mw->window.clear(sf::Color(127, 127, 127));

	mw->window.draw(mw->bg);	// black background

	mw->window.draw(mw->currentSP);	// stack indicator
	mw->window.draw(ot->stack_vals); // stack values

	mw->window.draw(ot->reg_title); // registers names
	for(int i = 0; i < 16; i++)
		mw->window.draw(ot->reg_vals[i]);

	// other pseudo registers
	mw->window.draw(ot->I);
	mw->window.draw(ot->ST);
	mw->window.draw(ot->DT);
	mw->window.draw(ot->SP);
	mw->window.draw(ot->PC);
	
	// actual chip-8 display
	for(int i = 0; i < 64*32; i++)
		mw->window.draw(mw->pixel[i]);
}

template <typename T> 
std::string coorString(sf::Vector2<T> v)
{
	std::stringstream s;
	s << v.x << " " << v.y;
	return s.str();
}

////	main
int main(int argc, char **argv)
{
	cpu m;
	win mw;
	txt ot;
	
	init(&m, &mw, &ot);

	if(argc > 1)
		openRom(&m, argv[1]);

	////	game loop
	while(mw.window.isOpen())
	{
		////	poll events
		pollEvents(&mw, &m);		

		////	update
		updateObjects(&m, &mw, &ot);

		//// draw to screen
		drawObjects(&mw, &ot);

		//// render
		mw.window.display();
	}
	return 0;
}