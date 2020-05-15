# CHIP-8 Emularor

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/b31e7b68cf32421b8e6aeefe971825c6)](https://app.codacy.com/manual/LazieKat/chip8-c?utm_source=github.com&utm_medium=referral&utm_content=LazieKat/chip8-c&utm_campaign=Badge_Grade_Dashboard)

According to wikipedia "CHIP-8 is an interpreted programming language
that runs in a virtual machine. It was initially used on the COSMAC VIP and 
Telmac 1800 8-bit microcomputers in the mid-1970s. 
It was made to allow video games to be more easily programmed for these 
computers. 

## What is this repo

A friend of mine told me about CHIP-8 and I found it rather simple 
so I attempted to write an emulator as a practice and for fun, but as it
might be obvious from the code, I stopped midway. I migh complete it
later but I don't want too dedicated to it since it is not a main project
right now, maybe later...

Currently the display is just writing spaces in the terminal with VT-100
escapes. I wanted to use SFML but I haven't used it with C before and 
I'm having some issues with using _objects_ out of their scope (by passing
pointers), it just crashes even if the _object_ was moved from main to
global scope, kinda weird... I used SFML before in a C++ environemnt
but I wanna keep this project pure C cuz that's how real men code!

Never mind I used it after all!
