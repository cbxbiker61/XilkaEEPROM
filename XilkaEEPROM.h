#pragma once
/*
 *  Use the I2C bus with EEPROM 24LC64
 *  Sketch:    eeprom.pde
 *
 *  Author: hkhijhe
 *  Date: 01/10/2010
 *
 *  2015.01.23 Converted to a class library
 *  Kelly Anderson
 *
 */

#include <stdint.h>
#include <Wire.h> // I2C library

class XilkaEEPROM
{
public:
	XilkaEEPROM(int devAddr);
	bool begin(void);

	void write(unsigned int address, uint8_t data);
	void write(unsigned int address, const uint8_t *data, int length);
	uint8_t read(unsigned int address);
	void read(unsigned int address, uint8_t *buffer, int length);

private:
	int writeStride(unsigned int address, const uint8_t *data, int length);
	int readStride(unsigned int address, uint8_t *data, int length);
	void writeDelay(void);

	int _devAddr;
	unsigned long _lastWrite; // track uS
};

