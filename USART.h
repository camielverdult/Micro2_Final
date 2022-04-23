/*
 * USART.h
 *
 * Created: 3/29/2022 2:11:53 PM
 *  Author: Camiel Verdult
 */ 

#pragma once

#include <string.h>
#include <stdlib.h>
#include <util/delay.h>

// Calculations for UART BAUD rate
#define BAUD 9600
#define UBBR (F_CPU / 16 / BAUD - 1) // This is 103

const uint8_t print_debug = 0;

void USART_Initialize(void) {
	  // Set BAUD rate through calculated UBRR
	  UBRR0H = (uint8_t) (UBBR >> 8);
	  UBRR0L = (uint8_t) UBBR;
	  
	  // Enable receiver and transmitter */
	  UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	  
	  // Set USART mode to Asynchronous USART, disable parity
	  // set 1 stop bit and set 8 data bits
	  UCSR0C = (0 << UMSEL01) | (0 << UMSEL00) | (0 << UPM01) | (0 << UPM00) | (0 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00);
}

void USART_Transmit_Char(const unsigned char data)
{
	// Wait for the buffer to be empty
	while(!(UCSR0A & (1 << UDRE0))) {};
	
	// Transmit the next character
	UDR0 = data;
}

void USART_Transmit_Char_Debug(const unsigned char data)
{
	if (print_debug == 0) {
		return;
	}
	
	// Wait for the buffer to be empty
	while(!(UCSR0A & (1 << UDRE0))) {};
	
	// Transmit the next character
	UDR0 = data;
}

void USART_Transmit_String(const char* data) {
	for (uint16_t i = 0; i < strlen(data); i++) {
		USART_Transmit_Char(data[i]);
	}
}

void USART_Transmit_String_Debug(const char* data) {
	if (print_debug == 0) {
		return;
	}
	
	for (uint16_t i = 0; i < strlen(data); i++) {
		USART_Transmit_Char(data[i]);
	}
}

void USART_Transmit_Float(const float value) {
	char buffer[10];
	dtostrf(value, 0, 4, buffer);
	
	USART_Transmit_String(buffer);
}

void USART_Transmit_Float_Debug(const float value) {
	if (print_debug == 0) {
		return;
	}
	
	char buffer[10];
	dtostrf(value, 0, 4, buffer);
	
	USART_Transmit_String(buffer);
}

void USART_Transmit_Integer(const uint16_t value) {
	char buffer[6];
	utoa(value, buffer, 10);
	
	USART_Transmit_String(buffer);
}

void USART_Transmit_Integer_Debug(const uint8_t value) {
	if (print_debug == 0) {
		return;
	}
	
	char buffer[4];
	utoa(value, buffer, 10);
	
	USART_Transmit_String(buffer);
}
