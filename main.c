/*
 * main.c
 *
 * Created: 3/29/2022 2:05:04 PM
 *  Author: Camiel Verdult
 */ 

// AVR and GCC libraries
#include <avr/io.h>
#include <string.h>

#define F_CPU 16000000UL

// Drivers / own libraries
#include "USART.h"
#include "WS2812b.h"
#include "INA260.h"
#include "JSON.h"

void TestLeds(void) {
	USART_Transmit_String("Setting LED strip to red!\n");
	WS2812_Set(10, 0, 0);
	_delay_ms(600);
	
	USART_Transmit_String("Setting LED strip to green!\n");
	WS2812_Set(0, 10, 0);
	_delay_ms(600);
	
	USART_Transmit_String("Setting LED strip to blue!\n");
	WS2812_Set(0, 0, 10);
	_delay_ms(600);
	
	USART_Transmit_String("Setting LED strip to white!\n");
	WS2812_Set(10, 10, 10);
	_delay_ms(600);
}

void TestINA260(void) {
	float voltage = INA260_Voltage();
	USART_Transmit_Float(voltage);
	USART_Transmit_String("\n");
}

int main(void)
{
	// Initialize USART
	USART_Initialize();
	 
	USART_Transmit_String("Starting up...\n");
	
	// Initialize WS2812 strip
	WS2812_Initialize();
	
	// Initialize INA260 (This will also initialize I2C)
	INA260_Initialize();
	
	
    while(1)
    {	
        // TODO:: Please write your application code
		// TestLeds();
		// TestINA260();
		
		JSONDictionary json;
			
		JSON_Dictionary_Init(&json);
		
		uint8_t red = rand() % 255;
		JSON_Set_Integer(&json, "Red", red);
		
		uint8_t green = rand() % 255;
		JSON_Set_Integer(&json, "Green", green);
		
		uint8_t blue = rand() % 255;
		JSON_Set_Integer(&json, "Blue", blue);
		
		WS2812_Set(red, green, blue);

		float voltage = INA260_Voltage();
		float current = INA260_Current();
		float power = INA260_Power();

		JSON_Set_Float(&json, "milliVolt", voltage);
		JSON_Set_Float(&json, "milliAmp", current);
		JSON_Set_Float(&json, "milliWatt", power);
		
		
		char buffer[300];
		
		USART_Transmit_String_Debug("Serializing JSON...\n");
		
		JSON_Serialize_Dictionary(&json, buffer, 200);
		
		USART_Transmit_String(buffer);
		USART_Transmit_Char('\n');
		
		JSON_Cleanup(&json);
		
		_delay_ms(1000);
    }
}