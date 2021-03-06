/*
	hw.h

	Copyright (c) 2019 Lagoni
	Not for commercial use
 */ 


#ifndef HW_H_
#define HW_H_

#define PCB_VERSION 11

#ifdef PCB_VERSION == 10
#elif  PCB_VERSION == 11
#else
	#error No valid PCB hardware selected.
#endif

extern Uart *SerialGPS;
//Uart gpsSerial(&sercom2, GPSRxPin, GPSTxPin, SERCOM_RX_PAD_1, UART_TX_PAD_0);                                // Create the new UART instance for the GPS module
extern Uart *SerialfrskySPort;
//extern Uart frskySPortSerial(); //&sercom3, fryskySmartPortRXPin, fryskySmartPortTXPin, SERCOM_RX_PAD_3, UART_TX_PAD_2); // Create the new UART instance for the frskySmartPort
extern Uart *SerialSBUS;
//extern Uart SBUSSerial(&sercom4, sBUSRXPin, sBUSRXPin, SERCOM_RX_PAD_1, UART_TX_PAD_0);                             // Create the new UART instance for the SBUS (RC inputs)

// Must be freed up in variant.cpp or alternative use Serial1 (defualt on mkrzero and uses same pins)
extern Uart *SerialAUX; //(&sercom5, auxRXPin, auxTXPin, SERCOM_RX_PAD_3, UART_TX_PAD_2);                                // Create the new UART instance for auxillary serial.


// Pin definition on TransponderRC2_4-10 PCB:
// Define Pins for Radio
const int chipSelectPin = 1;  // Pin D1  on MKRZero Board - Chip pin is 32 or PA23
const int misoPin       = 10; // Pin D10 on MKRZero Board - Chip pin is 28 or PA19
const int mosiPin       = 8;  // Pin D8  on MKRZero Board - Chip pin is 25 or PA16
const int sckPin        = 9;  // Pin D9  on MKRZero Board - Chip pin is 26 or PA17
const int rxEnPin       = 17; // Pin A2  on MKRZero Board - Chip pin is 48 or PB03
const int txEnPin       = 32; // Pin LED on MKRZero Board - Chip pin is  7 or PB08
const int resetPin      = 4;  // Chip pin is 19 or PB10
const int busyPin       = 5;  // Chip pin is 20 or PB11
// For DIO pin see Ifdef for PCB_VERSION XX below

// POWER ON pin for Battery
const int powerOnPin    = 0;  //Pin D0 on MKRZero Board - Chip pin is 31 or PA22

// Analog inputs
const int analogVinPin  = 19;  // Chip pin is 10 or PA05 
const int analogVbatPin = 16;  // Chip pin is 47 or PB02 

// SBUS pins:
const int sBUSInvertPin = 18;     // Chip pin is 9 or PA04
const int sBUSRXPin     = 33;     // Chip pin is 8 or PB09 (SERCOM4 PAD1 RX)

// Frsky Smart Port (S. Port) Pins:
const int fryskyInvertPin = 2;          // Chip pin is 15 or PA10
const int fryskySmartPortTXPin = 6;     // Chip pin is 29 or PA20 (SERCOM3 PAD2 TX)
const int fryskySmartPortRXPin = 7;     // Chip pin is 30 or PA21 (SERCOM3 PAD3 RX)

// Auxillary serial port pins:
const int auxTXPin = 14;     // Chip pin is 29 or PB22 (SERCOM5 PAD2 TX)
const int auxRXPin = 13;     // Chip pin is 30 or PB23 (SERCOM5 PAD3 RX)

// GPS pins:
const int gPSPowerOnPin = 20;      // Chip pin is 11 or PA06
const int GPSTxPin = 11;           // Chip pin is 13 or PA08 (SERCOM2 PAD0 TX)
const int GPSRxPin = 12;           // Chip pin is 14 or PA09 (SERCOM2 PAD1 RX)

// LED pins:
const int led2Pin = 21;      // Chip pin is 12 or PA07

// Unused pins:
const int pa11Pin = 3;  // PA11
const int pa13Pin = 27; // PA13
const int pa14Pin = 28; // PA14
const int pa28Pin = 31; // PA28
// PA30-31 (SWDCLK/SWDIO) Not defined in variants.cpp

#if PCB_VERSION == 10
	
	// Pin 30 used as dio1
	const int dio1Pin = 30; // Chip pin is 39 or PA27 // LED1 is used as input for DI0 interrupt from E28!
	
	// External analogVref pin	
	const int analogVrefPin = 25;  // Chip pin is 4 or PA03

	const int pa18Pin = 24; // PA18
	const int pa12Pin = 26; // PA12
	const int pa15Pin = 29; // PA15
	const int pa02Pin = 15; // PA02
	
#elif PCB_VERSION == 11 
	
	// dio1 pin from radio E29
	const int dio1Pin = 25; // PA03
	
	// Barometer Chip select:
	const int Baro_chipSelectPin = 24; // PA18
	
	// Safty Switch
	const int SaftySwitchPin = 26; // PA12

	// GPS Backup power pin
	const int GPSBackupPowerPin = 29; // PA15

	// LED in external safty switch
	const int SaftyLEDPin = 30;      // PA27

	// Analog input for 5V input.
	const int analogVin5VPin = 15; // PA02

#endif

float getBatteryVoltage(void);
float getInputVoltage(void);
float getInput5VVoltage(void);


void PowerON(void);
void PowerOFF(void);


void PowerONGPS(void);
void PowerOFFGPS(void);
void PowerONGPSBackup(void);
void PowerOFFGPSBackup(void);

void hwInit();
void hwInit_debug_lowpower(void);

void LEDSaftySwitchON(void);
void LEDSaftySwitchOFF(void);

// Unique 128bit serial number from chip flash
// Chip unique serial number part 1: 800256040
// Chip unique serial number part 2:1347311437
// Chip unique serial number part 3: 808333642
// Chip unique serial number part 4:4278389281
// Chip unique serial number: " 80025604013473114378083336424278389281" (Prototype 1 - flight test)

// Chip unique serial number part 1:2946562988
// Chip unique serial number part 2:1347311437
// Chip unique serial number part 3: 808333642
// Chip unique serial number part 4:4278394144
// Chip unique serial number: "294656298813473114378083336424278394144" (Prototype 2 - ground test and low power mod)



#endif /* HW_H_ */