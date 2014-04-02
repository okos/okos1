rm okos1.o
rm Debug/okos1.elf
rm Debug/okos1.hex
avr-gcc -g -Os -mmcu=atmega32 -c okos1.cpp
avr-gcc -g -mmcu=atmega32 -o Debug/okos1.elf okos1.o
avr-objcopy -j .text -j .data -O ihex Debug/okos1.elf Debug/okos1.hex
