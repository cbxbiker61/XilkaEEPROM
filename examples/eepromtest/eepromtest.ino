
#include <Wire.h>
#include <XilkaEEPROM.h>
XilkaEEPROM eeprom(0x50);

void setup()
{
	Serial.begin(115200);

	if ( ! eeprom.begin() )
	{
		Serial.println("XilkaEEPROM not found");

		while ( true )
			;
	}
}

void testSequential256(int offset, bool up = true)
{
	Serial.print("sequential ");
	Serial.print(up ? "up ": "down ");
	Serial.print(offset, DEC);
	Serial.print("-");
	Serial.println(offset + 256, DEC);

	for ( int i = 0; i < 256; ++i )
		eeprom.write(i + offset, up ? i : 255 - i);

	for ( int i = 0; i < 256; ++i )
	{
		uint8_t d(eeprom.read(i + offset));

		if ( d != (up ? i : 255 - i) )
		{
			Serial.print("fail ");
			Serial.print(i+offset, DEC);
			Serial.print(" ");
			Serial.print(i, DEC);
			Serial.print("=");
			Serial.println(d, HEX);
		}
	}
}

void testSequential4K(void)
{
/*
 * Test the first 4K of EEPROM
 * this test writes/reads 8K of data
 */
	Serial.println("XilkaEEPROM sequential test 0-4096");

	for ( int i = 0; i < 4096; i += 256 )
	{
		testSequential256(i, true);
		testSequential256(i, false);
	}
}

void testBlock256(int offset, bool up = true)
{
	Serial.print("block ");
	Serial.print(up ? "up ": "down ");
	Serial.print(offset, DEC);
	Serial.print("-");
	Serial.println(offset + 256, DEC);

	uint8_t out[256];
	uint8_t in[256];

	for ( int i = 0; i < 256; ++i )
		out[i] = up ? i : 255 - i;

	/*
	 * write the test block
	 */
	eeprom.write(offset, out, sizeof(out));

	/*
	 * read the test block
	 */
	eeprom.read(offset, in, sizeof(in));

	for ( int i = 0; i < 256; ++i )
	{
		if ( out[i] != in[i] )
		{
			Serial.print("fail ");
			Serial.print(i+offset, DEC);
			Serial.print(" ");
			Serial.println(out[i], HEX);
			Serial.print("!=");
			Serial.println(in[i], HEX);
		}
	}
}

void testBlock4K(int offset = 0)
{
/*
 * Test the first 4K of EEPROM
 * this test writes/reads 8K of data
 */
	Serial.print("XilkaEEPROM block test 0-4096 offset=");
	Serial.println(offset, DEC);

	for ( int i = 0; i < 4096; i += 256 )
	{
		testBlock256(i+offset, true);
		testBlock256(i+offset, false);
	}
}

void loop()
{
#if 0
	testSequential4K();
	delay(2000);
#endif
	testBlock4K();
	testBlock4K(33);
	testBlock4K(63);
	delay(5000);
}

