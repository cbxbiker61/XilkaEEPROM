I2C EEPROMs behave very similarly so a quite a few EEPROMs
should work without modification.

Here is the breakout board that I used for testing:

EEPROM AT24C256 breakout board : http://www.amazon.com/gp/product/B00FHN6XLU/

And here is an Arduino shield that should work:

EEPROM Shield With AT24C256 : http://www.emartee.com/product/41950/Arduino-EEPROM-Shield-With-256K-AT24C256

If you need to do some data storage in Arduino, then using the EEPROM is probably the most simple practices. In actual,Arduino used ATmega chip itself has certain EEPROM memory, but only limited number. We design the external EEPROM memory module which use I2C bus line to connect with Arduino, and with pluggable chip, insert in this series, so it will easy to expand capacity, basically just need to insert a bigger capacity of EEPROM chip.

The storage module is based on EEPROM chips of AT24C series, 256K bit capacity, that's 32k bytes. The EEPROM module communicate with Arduino through the I2C interface.It will be better to use with Arduino-Sensor-Shield-V4.0 and the Arduino-I2C-COM-Cable.

This module can be used with Arduino Special Sensor Shield V4.0.
You can do some simple design about it，it support“plug and play”。

The storage module is based on AT24C series of EEPROM chips, which the I2C base address is 0×50, and the last three addresses can be set according to need.
So we should set the last three address when we firstly use, it is through the A2, A1 and A0 of the four position DIP switch. Dial the DIP switch to up,the corresponding is 1; Dial the DIP switch to down,the corresponding is 0. Namely,if move the A2, A0, A1 to up, the corresponding address is 0×57, But if move the A2, A0, A1 to down, the corresponding address is 0×50.


The other place which need to be set is switch RS of this storage module, it is used to set that whether connect the SDA and SCl of I2C bus line with pull-up resistors. We know that the I2C is called bus line, just because it can connect multiple the I2C equipment, according to the provisions of this agreement, the I2C allowed that we only be permitted to connect pull-up resistors with one I2C device that is the nearest from controllor. Here you may already know, if there are multiple this storage module Arduino connected to the I2C bus line, we must set the RS switch of storage module of the nearest from Arduino to the position "ON", and the other RS of other storage module can not be set to ON location:

If you are using Arduino sensor shield V4 board, pay attention to that the position of IIC/COM jump line should be set up to the IIC side, because we need to use the I2C connection mode :

Then connect with Arduino Special Sensor Shield V4.0.

