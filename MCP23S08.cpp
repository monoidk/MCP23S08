/**********************************************************************
 * 
 * This is the C++ part of the MCP23S08 library.
 * See MCP23S08.h and the example files for a full documentation.
 * 
 *********************************************************************/


#include "MCP23S08.h"



/*##################################### PUBLIC FUNCTIONS #####################################*/

MCP23S08::MCP23S08(SPIClass & spi,
		   uint8_t csPin,
		   uint8_t deviceAddr,
		   bool haen,
		   uint32_t spi_speed)
: spi(spi), csPin(csPin), haen(haen), spi_settings(spi_speed, MSBFIRST, SPI_MODE0) {
	deviceOpcode |= ((deviceAddr & 0x03) << 1);
}


void MCP23S08::begin() {
	/* Why do we initialize the pin here
	yet expect the spi bus to be initialized?
	Good question, not sure but, maybe the ESP32 SPI is not yet inialised.*/
	
	pinMode(csPin, OUTPUT);		// Required for ESP32 devices.

	// enable chip hardware addresses
	if (haen) {
		writeRegister(MCP23S08_IOCON, MCP23S08_IOCON_HAEN);
	}
}


// reset all registers to default:
void MCP23S08::reset() {
	spi.beginTransaction(spi_settings);
	digitalWrite(csPin, LOW);
	spi.transfer(deviceOpcode);	// initialize transfer with opcode and R/W-flag cleared
	spi.transfer(MCP23S08_IODIR);	//set address pointer to first register
	spi.transfer(0xFF);				// reset first register
	for (uint8_t i = 0; i < MCP23S08_OLAT; i++) {
		if (haen && (MCP23S08_IOCON == i))
			spi.transfer(MCP23S08_IOCON_HAEN);			// enable hardware address (HAEN)
		else
			spi.transfer(0x00);			// reset other 10 registers
	}
	digitalWrite(csPin, HIGH);
	spi.endTransaction();
}


bool MCP23S08::digitalReadIO(uint8_t pin) {
	if (pin > 7) {
		return 0;
	}
	return (getInputStates() >> pin) & 1;
}


void MCP23S08::digitalWriteIO(uint8_t pin, bool state) {
	if (pin > 7) {
		return;
	}
	
	setOutputStates((getOutputStates() & ~(1 << pin)) | (state << pin));
}


void MCP23S08::pinModeIO(uint8_t pin, uint8_t mode) {
	if (pin > 7) {
		return;
	}
	
	switch (mode) {
		case INPUT:
			setPinModes(getPinModes() & ~(1 << pin));			// set pin to input
			enablePullups(getEnabledPullups() & ~(1 << pin));	// disable pullup for pin
			break;
		case OUTPUT:
			setPinModes(getPinModes() | (1 << pin));				// set pin to output
			enablePullups(getEnabledPullups() & ~(1 << pin));	// disable pullup for pin
			break;
		case INPUT_PULLUP:
			setPinModes(getPinModes() & ~(1 << pin));			// set pin to input
			enablePullups(getEnabledPullups() | (1 << pin));	// enable pullup for pin
			break;
	}
}


void MCP23S08::setOutputStates(uint8_t states) {
	writeRegister(MCP23S08_OLAT, states);
}


void MCP23S08::setPinModes(uint8_t modes) {
	writeRegister(MCP23S08_IODIR, ~(modes));	// inverted to match IDE defaults
}


void MCP23S08::enablePullups(uint8_t enables) {
	writeRegister(MCP23S08_GPPU, enables);
}


uint8_t MCP23S08::getInputStates() {
	return readRegister(MCP23S08_GPIO);
}


uint8_t MCP23S08::getOutputStates() {
	return readRegister(MCP23S08_OLAT);
}


uint8_t MCP23S08::getPinModes() {
	return ~(readRegister(MCP23S08_IODIR));		// inverted to match IDE defaults
}


uint8_t MCP23S08::getEnabledPullups() {
	return readRegister(MCP23S08_GPPU);
}


/*##################################### PRIVATE FUNCTIONS #####################################*/

void MCP23S08::writeRegister(uint8_t address, uint8_t data) {
	spi.beginTransaction(spi_settings);
	digitalWrite(csPin, LOW);
	spi.transfer(deviceOpcode);		// initialize transfer with opcode and R/W-flag cleared
	spi.transfer(address);
	spi.transfer(data);
	digitalWrite(csPin, HIGH);
	spi.endTransaction();
}


uint8_t MCP23S08::readRegister(uint8_t address) {
	uint8_t data;
	spi.beginTransaction(spi_settings);
	digitalWrite(csPin, LOW);
	spi.transfer(deviceOpcode | 1);        // initialize transfer with opcode and R/W-flag set
	spi.transfer(address);
	data = spi.transfer(0);
	digitalWrite(csPin, HIGH);
	spi.endTransaction();
	return data;
}
