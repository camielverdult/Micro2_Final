/*
 * INA260.h
 *
 * Created: 3/30/2022 2:16:53 PM
 *  Author: Camiel Verdult
 */ 

#pragma once

#include "I2C.h"
#include "USART.h"

const uint8_t INA260_ADDR = 0x40;

// Register Set Summary
// Found on page 21
// const uint8_t CONFIG_REGISTER = 0x00;
const uint8_t INA260_CURRENT_REGISTER = 0x01;
const uint8_t INA260_BUS_VOLTAGE_REGISTER = 0x02;
const uint8_t INA260_POWER_REGISTER = 0x03;

void INA260_Initialize(void) {
	I2C_Initialize();
}

uint16_t INA260_Read(uint8_t reg) {
	// To change the register pointer for a read operation, a new value must be
	// written to the register pointer. This write is accomplished by issuing a slave address byte with the R/W bit low,
	// followed by the register pointer byte.
	USART_Transmit_String_Debug("INA260: Changing register pointer for read operation...\n");
	I2C_Start();
	I2C_Write((INA260_ADDR << 1) + WRITE); // This gets sent, verified by scope (hex value 40)
	I2C_Write(reg); // This also gets sent, verified by scope (hex value 2)
	I2C_Stop();

	// The master then generates a start condition
	// and sends the slave address byte with the R/W bit high to initiate the read command
	USART_Transmit_String_Debug("INA260: Reading register...\n");
	I2C_Start();
	I2C_Write((INA260_ADDR << 1) + READ); // This gets sent, verified by scope (hex value 41 because read bit)

	uint8_t msb = I2C_Read();
	uint8_t lsb = I2C_Read();
	
	I2C_Stop();

	uint16_t data = (msb << 8) + lsb;
	
	return data;
}

float INA260_Voltage(void) {
	// Read bus voltage register at 0x01

	uint16_t value = INA260_Read(INA260_BUS_VOLTAGE_REGISTER);

	USART_Transmit_String_Debug("\nINA260: Got voltage measurement from sensor!\n");

	float m_voltage = (float)value * 1.25f;

	return m_voltage;
}

float INA260_Current(void) {
	// Read bus voltage register at 0x01

	uint16_t value = INA260_Read(INA260_CURRENT_REGISTER);

	USART_Transmit_String_Debug("INA260: Got current measurement from sensor!\n");

	float m_current = (float)~(-value - 1) * 1.25f;

	return m_current;
}

float INA260_Power(void) {
	// This value is always positive
	uint16_t value = INA260_Read(INA260_POWER_REGISTER);

	USART_Transmit_String_Debug("INA260: Got power measurement from sensor!\n");

	// Steps of 10 mW
	float m_power = (float)value * 10.0;

	return m_power;
}
