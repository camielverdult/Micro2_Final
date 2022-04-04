/*
 * WS2812b.h
 *
 * Created: 3/29/2022 2:20:27 PM
 *  Author: Camiel Verdult
 */ 

#include <util/delay.h>

#define COLORS 3 // R, G and B
#define NUM_BITS 8 * COLORS;
#define ONE 0xF8
#define ZERO 0xC0

#define NUM_LEDS 8

typedef enum {
	black,
	red,
	blue,
	green,
	purple,
	orange,
	white,
	light_blue,
	light_green
} ws2812b_color;

void SPI_Initialize(void) {
	// Start with settings SPI pins (SCLK, MISO, MOSI and SS) to output
	// SCLK pin is PORTB5 (Pin 13 on Arduino Uno)
	// MISO pin is PORTB4 (Pin 12 on Arduino Uno)
	// MOSI pin is PORTB3 (Pin 11 on Arduino Uno)
	// SS pin is PORTB2 (Pin 10 on Arduino Uno)
	DDRB |= (1 << PORTB5) | (1 << PORTB4) | (1 << PORTB3) | (1 << PORTB2);
	
	// To control the WS2812B using the SPI interface, we want the following options:
	// SPCR � SPI Control Register
	// Bit 7 � SPIE: SPI Interrupt Enable
	// Bit 6 � SPE: SPI Enable
	// Bit 5 � DORD: Data Order (We want MSB first)
	// Bit 4 � MSTR: Master/Slave Select
	// Bit 3 � CPOL: Clock Polarity (We want SPI mode 1, SPI2X at 8MHz)
	// Bit 2 � CPHA: Clock Phase
	// See https://en.wikipedia.org/wiki/Serial_Peripheral_Interface#Mode_numbers for more info
	
	// Bits 1, 0 � SPR1, SPR0: SPI Clock Rate Select 1 and 0	
	SPCR |= (1 << SPE) | (0 << DORD) | (1 << MSTR) | (0 << CPOL) | (1 << CPHA) | (0 << SPR1) | (0 << SPR0);
	
	// SPI2X bit set to 1 (and SPR1 & SPR0 to 0) sets SPI clock divider to 2 (16MHz / 2 = 8MHz)
	SPSR |= (1 << SPI2X);
}

uint8_t SPI_Transceive(uint8_t data) {
	// Send data
	SPDR = data;
	
	// Wait for transmit complete
	while(!(SPSR & (1 << SPIF)));
	
	// Return received data
	return SPDR;
}

void WS2812_update() {
	_delay_us(60000);
}

void WS2812_Send(unsigned char r, unsigned char g, unsigned char b) {
	unsigned char s = NUM_BITS;

	unsigned long value = (((unsigned long)g << 16) | ((unsigned long)r << 8) | ((unsigned long)b));

	while (s > 0) {
		if ((value & 0x800000) != 0) {
			SPI_Transceive(ONE);
			asm("nop");
			asm("nop");
		} 
		else {
			SPI_Transceive(ZERO);
		}
		
		value <<= 1;
		s--;
	}
}

void WS2812_Set(uint8_t r, uint8_t g, uint8_t b) {
	for (int n = 0; n < NUM_LEDS; n++) {
		WS2812_Send(r, g, b);
	}

	WS2812_update();
}

void WS2812_Reset(void) {
	WS2812_Set(0, 0, 0);
}

void WS2812_Initialize(void) {
	SPI_Initialize();
	_delay_ms(10);
	WS2812_Reset();
}
