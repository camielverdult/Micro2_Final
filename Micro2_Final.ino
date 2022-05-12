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

void loop_uart() {
  USART_Initialize();
   
  USART_Transmit_String("Starting up...\n");

  while (1) {
    JSONDictionary json;
      
    JSON_Dictionary_Init(&json);
    
    uint8_t red = rand() % 255;
    JSON_Set_Integer(&json, (char*)"Red", red);
    
    uint8_t green = rand() % 255;
    JSON_Set_Integer(&json, (char*)"Green", green);
    
    uint8_t blue = rand() % 255;
    JSON_Set_Integer(&json, (char*)"Blue", blue);
    
    WS2812_Set(red, green, blue);
  
    uint16_t voltage = (uint16_t)(INA260_Voltage());
    uint16_t current = (uint16_t)(INA260_Current());
    uint16_t power = (uint16_t)(INA260_Power());
     
    JSON_Set_Integer(&json, (char*)"voltage", voltage);
    JSON_Set_Integer(&json, (char*)"current", current);
    JSON_Set_Integer(&json, (char*)"power", power);
     
    char buffer[300];
    
    USART_Transmit_String_Debug("Serializing JSON...\n");
    
    JSON_Serialize_Dictionary(&json, buffer, 300);
    
    USART_Transmit_String(buffer);
    USART_Transmit_Char('\n');
        
    JSON_Cleanup(&json);
    
    _delay_ms(100);
  }
}

int main(void) {
	// Initialize USART
	//USART_Initialize();
	 
	//USART_Transmit_String("Starting up...\n");
	
	// Initialize WS2812 strip
	WS2812_Initialize();
	
	// Initialize INA260 (This will also initialize I2C)
	INA260_Initialize();
		
	loop_uart();
}
