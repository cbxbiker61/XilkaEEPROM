/*
 *  EEPROM 24LC64 class library
 *      should work with a huge swath of the EEPROMs
 *      that are available
 *
 *  Author: hkhijhe
 *  Date: 01/10/2010
 *
 *  2015.01.23 Converted to a class library
 *  2015.02.06 Tested and refined
 *  Kelly Anderson
 *
 */

/*
 * AT24C256 (ATMLH348) 32K EEPROM
 *
 * MicroChip 16K 24AA16/24LC16B 304176C
 */
#include <Arduino.h>
#include <Wire.h>
#include "XilkaEEPROM.h"

#if ARDUINO >= 100
#define WIRE_WRITE(x) Wire.write(x)
#define WIRE_READ() Wire.read()
#else
#define WIRE_WRITE(x) Wire.send(x)
#define WIRE_READ() Wire.receive()
#endif

/*
 * For proper operation we'll read/write in chunks
 * we need to keep out stride to (BUFFER_LENGTH - 4)
 * 4 is reserved for the read/write commands.
 *
 * Since we want to keep our STRIDE to an even divisor
 * of the EEPROM page size, this means that we are
 * stuck using 1/2 of the BUFFER_LENGTH max.
 */
#if (BUFFER_LENGTH >= 0x80)
#define WIRE_STRIDE 0x40
#else
#define WIRE_STRIDE (BUFFER_LENGTH/2)
#endif

/*
 * uS time to wait before starting new write
 */
#define SAFE_DELAY 5000

XilkaEEPROM::XilkaEEPROM(int devAddr)
	: _devAddr(devAddr)
	, _lastWrite(micros() - SAFE_DELAY)
{
}

void XilkaEEPROM::writeDelay(void)
{
	const unsigned long previousWrite(_lastWrite);
	const unsigned long safeTime(previousWrite + SAFE_DELAY);

	if ( previousWrite > micros() ) // handle uS wrap which happens every 70 min
		delayMicroseconds(SAFE_DELAY);
	else if ( safeTime > _lastWrite )
		delayMicroseconds(safeTime - _lastWrite);
}

bool XilkaEEPROM::begin(void)
{
	Wire.begin();

    Wire.beginTransmission(_devAddr);
	WIRE_WRITE(0); // MSB
	WIRE_WRITE(0); // LSB
	Wire.endTransmission();

	Wire.requestFrom(_devAddr, 1);

	return Wire.available();
}

void XilkaEEPROM::write(unsigned int eeaddress, uint8_t data)
{
	writeDelay();

	int rdata(data);

	Wire.beginTransmission(_devAddr);
	WIRE_WRITE((int)(eeaddress >> 8)); // MSB
	WIRE_WRITE((int)(eeaddress & 0xFF)); // LSB
	WIRE_WRITE(rdata);
	Wire.endTransmission();

	_lastWrite = micros();
}

void XilkaEEPROM::write(unsigned int eeaddress
	, const uint8_t *data, int length)
{
	const unsigned int nextStrideBoundary((eeaddress & ~(WIRE_STRIDE - 1)) + WIRE_STRIDE);
	int bytesToNextStrideBoundary(nextStrideBoundary - eeaddress);

	/*
	 * write bytes until we hit a stride boundary
	 */
	while ( bytesToNextStrideBoundary )
	{
		const int nWritten(writeStride(eeaddress, data, min(length, bytesToNextStrideBoundary)));
		eeaddress += nWritten;
		data += nWritten;
		bytesToNextStrideBoundary -= nWritten;
		length -= nWritten;

		if ( ! length )
			break;
	}

	/*
	 * it's aligned, so let's go
	 */
	while ( length )
	{
		const int nWritten(writeStride(eeaddress, data, length));
		eeaddress += nWritten;
		data += nWritten;
		length -= nWritten;
	}
}

int XilkaEEPROM::writeStride(unsigned int eeaddress
	, const uint8_t *data, int length)
{
	writeDelay();

	Wire.beginTransmission(_devAddr);
	WIRE_WRITE((int)(eeaddress >> 8)); // MSB
	WIRE_WRITE((int)(eeaddress & 0xFF)); // LSB

	const int nWrite(min(length, WIRE_STRIDE));

	for (int i = 0; i < nWrite; ++i )
		WIRE_WRITE(data[i]);

	Wire.endTransmission();

	_lastWrite = micros();

	return nWrite;
}

uint8_t XilkaEEPROM::read(unsigned int eeaddress)
{
	writeDelay();

	Wire.beginTransmission(_devAddr);
	WIRE_WRITE((int)(eeaddress >> 8)); // MSB
	WIRE_WRITE((int)(eeaddress & 0xFF)); // LSB
	Wire.endTransmission();

	Wire.requestFrom(_devAddr, 1);

	uint8_t ret(Wire.available() ? WIRE_READ() : 0xFF);

	_lastWrite = micros();

	return ret;
}

void XilkaEEPROM::read(unsigned int eeaddress
	, uint8_t *data, int length)
{
	const unsigned int nextStrideBoundary((eeaddress & ~(WIRE_STRIDE - 1)) + WIRE_STRIDE);
	int bytesToNextStrideBoundary(nextStrideBoundary - eeaddress);

	/*
	 * read bytes until we hit a stride boundary
	 */
	while ( bytesToNextStrideBoundary )
	{
		const int nRead(readStride(eeaddress, data, min(length, bytesToNextStrideBoundary)));
		eeaddress += nRead;
		data += nRead;
		bytesToNextStrideBoundary -= nRead;
		length -= nRead;

		if ( ! length )
			break;
	}

	/*
	 * it's aligned, so let's go
	 */
	while ( length )
	{
		const int nRead(readStride(eeaddress, data, length));
		eeaddress += nRead;
		data += nRead;
		length -= nRead;
	}
}

int XilkaEEPROM::readStride(unsigned int eeaddress
	, uint8_t *data, int length)
{
	writeDelay();

	Wire.beginTransmission(_devAddr);
	WIRE_WRITE((int)(eeaddress >> 8)); // MSB
	WIRE_WRITE((int)(eeaddress & 0xFF)); // LSB
	Wire.endTransmission();

	Wire.requestFrom(_devAddr, length);

	const int nRead(min(length, WIRE_STRIDE));

	for ( int i = 0; i < nRead; ++i )
		if ( Wire.available() )
			data[i] = WIRE_READ();

	_lastWrite = micros();

	return nRead;
}

