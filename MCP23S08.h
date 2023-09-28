/**********************************************************************************************
 * 
 * This library for the Arduino IDE allows the simple usage of the MCP23S08 SPI based
 * I/O port expander by providing functions like digitalWrite, digitalRead and pinMode.
 * It is licensed under the MIT License, see LICENSE.txt.
 * 
 *********************************************************************************************/
 

#ifndef __MCP23S08_h__
#define __MCP23S08_h__

#include "Arduino.h"		// Required for ESP32 devices.
#include <SPI.h>


// register addresses
#define MCP23S08_IODIR		0x00
#define MCP23S08_IPOL		0x01
#define MCP23S08_GPINTEN	0x02
#define MCP23S08_DEFVAL		0x03
#define MCP23S08_INTCON		0x04
#define MCP23S08_IOCON		0x05
#define MCP23S08_GPPU		0x06
#define MCP23S08_INTF		0x07
#define MCP23S08_INTCAP		0x08
#define MCP23S08_GPIO		0x09
#define MCP23S08_OLAT		0x0A
#define MCP23S08_IOCON_HAEN	0x08


class MCP23S08 {
public:
	// constructors
	MCP23S08(SPIClass & spi, uint8_t csPin, uint8_t deviceAddr, bool haen = false, uint32_t spi_speed = 1000000);
	
	// call in setup
	void begin();

	// reset all registers (unneeded for defaults from POR)
	void reset();
	
	// usage equivalent to the default IDE functions, see examples
	bool digitalReadIO(uint8_t pin);               // equiv. (bitRead(MCP23S::readPort())?HIGH:LOW); doesn't read cached OLAT
	void digitalWriteIO(uint8_t pin, bool state);  // only sets OLAT; not pullup on INPUT ports
	void pinModeIO(uint8_t pin, uint8_t mode);     // equiv. MCP23S::pinMode
	
	// advanced, write settings for all pins at once
	void setOutputStates(uint8_t states);          // equiv. MCP23S::writePort
	void setPinModes(uint8_t modes);               // equiv. MCP23S::setDirsInput(~modes)
	void enablePullups(uint8_t enables);           // equiv. MSP23S::setPullups

	static const uint8_t PIN_MODE_INPUT_ALL = 0x00;
	static const uint8_t PIN_MODE_OUTPUT_ALL = 0xff;
	
	// advanced, get settings for all pin at once
	uint8_t getInputStates();                      // equiv. MCP23S::readPort
	uint8_t getOutputStates();                     // equiv. MCP23S::writePort
	uint8_t getPinModes();                         // equiv. ~(MCP23S::getDirsInput())
	uint8_t getEnabledPullups();                   // equiv. MCP23S::getPullups
	
private:
	// private variables
	SPIClass & spi;
	uint8_t csPin;
	uint8_t deviceOpcode = 0x40;
	bool haen;
	SPISettings spi_settings;
	
	// low level SPI communication
	void writeRegister(uint8_t address, uint8_t data);
	uint8_t readRegister(uint8_t address);
};

#endif	// __MCP23S08_h__
