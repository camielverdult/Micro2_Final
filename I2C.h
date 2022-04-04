/*
 * I2C.h
 *
 * Created: 3/29/2022 2:19:05 PM
 *  Author: Camiel Verdult
 */ 

#define WRITE 0
#define READ 1
#include <compat/twi.h>

uint8_t I2C_Get_Status(void)
{
	return TWSR & 0xF8;
}

void I2C_Initialize() {
	// TWSR: TWI Status Register
	// Bits 1..0 : TWPS: TWI Prescaler Bits
	// We want this to be 1, so we set bit TWPS1 and TWPS0 to 0
	TWSR = (0 << TWPS1) | (0 << TWPS0);
	
	// TWBR: TWI Bit Rate Register
	// Bits 7..0: TWI Bit Rate Register
	
	// SCL frequency = FCPU/(16+2(TWBR)x(PrescalerValue))
	//		  100kHz = 16MHz / (16 + 2(TWBR)x(1))
	//		  16+2TWBR = 16000000/100000
	//		  16+2TWBR = 160
	//		  2TWBR	   = 144
	//		  TWBR	   = 77
	//		  TWBR	   = 0b1001101
	TWBR = 72;

	// TWCR : TWI Control Register
	// Bit 2 : TWEN: TWI Enable Bit
	TWCR = (1 << TWEN);
}

void I2C_Start() {
	// initiate start condition and wait for status to update
	USART_Transmit_String_Debug("I2C: Initiating start condition...\n");
	
	TWCR = (1 << TWEN) | (1 << TWSTA) | (1 << TWINT);
	while(!(TWCR & (1 << TWINT)));
	
	uint8_t status = I2C_Get_Status();
	
	if (status == TW_MT_ARB_LOST) {
		// Retry I2C start
		USART_Transmit_String_Debug("I2C: Initiating start condition failed, trying again...\n");
		I2C_Start();
	}
}

void I2C_Write(uint8_t data) {
	// put data in buffer, send it and then wait for status to update
	USART_Transmit_String_Debug("I2C: Writing...\n");
	
	TWDR = data;
	TWCR = (1 << TWEN) | (1 << TWINT);

	while(!(TWCR & (1 << TWINT))) {
		USART_Transmit_String_Debug("I2C: Waiting for slave acknowledge...\n");
	}
	
	uint8_t status = I2C_Get_Status();
	
	if (status == TW_MT_ARB_LOST || status == TW_MT_SLA_NACK) {
		// Retry I2C start
		USART_Transmit_String_Debug("I2C: Writing data failed...\n");
		return;
	}
}
uint8_t I2C_Read() {
	// Receive a byte, wait for the status to update then retrieve it
	USART_Transmit_String_Debug("I2C: Configuring Two-wire interface for read...\n");
	
	TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
  
	while(!(TWCR & (1 << TWINT))) {
		USART_Transmit_String_Debug("I2C: Waiting for slave transmission...\n");
	}
	
	uint8_t status = I2C_Get_Status();
	
	if (status == TW_MT_ARB_LOST || status == TW_MT_SLA_NACK) {
		// Retry I2C start
		USART_Transmit_String_Debug("I2C: Reading data failed...\n");
		return 0xFF;
	}
	
	return TWDR;
}

void I2C_Stop(void)
{
	USART_Transmit_String_Debug("I2C: Initiating stop condition...\n");
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}
