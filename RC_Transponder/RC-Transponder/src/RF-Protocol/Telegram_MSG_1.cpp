/*
	Telegram_MSG_1.cpp

	Copyright (c) 2019 Lagoni
	Not for commercial use
 */ 
  #include "Telegram_MSG_1.h"
  
// constructor 
Telegram_MSG_1::Telegram_MSG_1(uint32_t _Unique_ID_1, uint32_t _Unique_ID_2, uint32_t _Unique_ID_3, uint32_t _Unique_ID_4, 
							   uint32_t _UTCTime,  uint32_t _Lattitude, uint32_t _Longitude, uint8_t _NumberOfSat, uint8_t _Fix,
							   bool _RunningOnBattery, float _Pressure, float _GroundSpeed, uint8_t _SecondsSinceLastGSContact,
							   float _BatteryVoltage, float _FirmwareVersion, uint8_t _PCBVersion, uint8_t _NumberOfBeaconsToRelay)
							   : Telegram(MSG_Beacon_Broadcast, _Unique_ID_1, _Unique_ID_2, _Unique_ID_3, _Unique_ID_4)
{	
	//Payload
	this->UTCTime = _UTCTime;
	this->Latitude = _Lattitude;
	this->Longitude = _Longitude;			
	this->NumberOfSat = _NumberOfSat;
	this->Fix = _Fix;
	this->RunningOnBattery = _RunningOnBattery;
	this->Pressure = _Pressure;
	this->GroundSpeed = _GroundSpeed;
	this->SecondsSinceLastGSContact = _SecondsSinceLastGSContact;
	this->BatteryVoltage = _BatteryVoltage;
	this->FirmwareVersion = _FirmwareVersion;
	this->PCBVersion = _PCBVersion;
	this->NumberOfBeaconsToRelay = _NumberOfBeaconsToRelay;	

	// test outside safezone:
//	Latitude =  55562000;
//	Longitude = 12281500;
	// test inside safezone:
//	Latitude =  55562260;
//	Longitude = 12283510;

	//Set TelegramData (RadioData) Payload
	uint16_t temp_16= 0;
	uint8_t temp_8= 0;
		
	// UTC Time
	TelegramData.payload[HEADER_SIZE+0] = (uint8_t)((UTCTime >> 16) & 0xFF);
	TelegramData.payload[HEADER_SIZE+1] = (uint8_t)((UTCTime >> 8) & 0xFF);
	TelegramData.payload[HEADER_SIZE+2] = (uint8_t)(UTCTime & 0xFF);
		
	// Latitude transformed to 32bits.
	TelegramData.payload[HEADER_SIZE+3] = (uint8_t)((Latitude >> 24) & 0xFF);
	TelegramData.payload[HEADER_SIZE+4] = (uint8_t)((Latitude >> 16) & 0xFF);
	TelegramData.payload[HEADER_SIZE+5] = (uint8_t)((Latitude >>  8) & 0xFF);
	TelegramData.payload[HEADER_SIZE+6] = (uint8_t)(Latitude & 0xFF);

	// Longitude transformed to 32bits.
	TelegramData.payload[HEADER_SIZE+7]  = (uint8_t)((Longitude >> 24) & 0xFF);
	TelegramData.payload[HEADER_SIZE+8]  = (uint8_t)((Longitude >> 16) & 0xFF);
	TelegramData.payload[HEADER_SIZE+9]  = (uint8_t)((Longitude >>  8) & 0xFF);
	TelegramData.payload[HEADER_SIZE+10] = (uint8_t)(Longitude & 0xFF);
		
	temp_8 = NumberOfSat & 0b00011111;
	temp_8 |= ((Fix & 0b00000011) << 5);
	if(RunningOnBattery)
	temp_8 |= 0b10000000;
		
	TelegramData.payload[HEADER_SIZE+11] = temp_8;
		
	// Pressure
	temp_16=(uint16_t)(Pressure);
	TelegramData.payload[HEADER_SIZE+12]  = (uint8_t)((temp_16 >>  8) & 0xFF);
	TelegramData.payload[HEADER_SIZE+13] = (uint8_t)(temp_16 & 0xFF);
		
	// Ground speed
	temp_16=(uint16_t)(GroundSpeed*10);
	TelegramData.payload[HEADER_SIZE+14]  = (uint8_t)(((temp_16) >>  8) & 0xFF);
	TelegramData.payload[HEADER_SIZE+15] = (uint8_t)((temp_16) & 0xFF);
		
	// Seconds since last ground contact
	TelegramData.payload[HEADER_SIZE+16] = SecondsSinceLastGSContact;
		
	// Battery Voltage
	temp_8=(uint8_t)((BatteryVoltage-2.0)*100);
	TelegramData.payload[HEADER_SIZE+17] = temp_8;
		
	// Firmware Version
	temp_8 = (uint8_t)(FirmwareVersion);
	TelegramData.payload[HEADER_SIZE+18] = temp_8;
	TelegramData.payload[HEADER_SIZE+19] = (uint8_t)((FirmwareVersion*100)-temp_8*100);
		
	// PCB Version
	TelegramData.payload[HEADER_SIZE+20] = PCBVersion;
		
	//Number of Beacons to Relay
	TelegramData.payload[HEADER_SIZE+21] = (uint8_t)NumberOfBeaconsToRelay;

	TelegramData.payloadLength = HEADER_SIZE + 21 + 1;
}


Telegram_MSG_1::Telegram_MSG_1(RadioData_t *radioData) : Telegram(radioData)
{
	// Data is copied in Telegram constructor to TelegramData.

	if( (TelegramData.payloadLength >= HEADER_SIZE+21+1)){
		UTCTime =   (uint32_t)((TelegramData.payload[HEADER_SIZE+0] << 16) + (TelegramData.payload[HEADER_SIZE+1] << 8) + TelegramData.payload[HEADER_SIZE+2]);
		Latitude =  (uint32_t)((TelegramData.payload[HEADER_SIZE+3] << 24) + (TelegramData.payload[HEADER_SIZE+4] << 16) + (TelegramData.payload[HEADER_SIZE+5] << 8) + TelegramData.payload[HEADER_SIZE+6]);
		Longitude = (uint32_t)((TelegramData.payload[HEADER_SIZE+7] << 24) + (TelegramData.payload[HEADER_SIZE+8] << 16) + (TelegramData.payload[HEADER_SIZE+9] << 8) + TelegramData.payload[HEADER_SIZE+10]);
			
		NumberOfSat = (uint8_t)(TelegramData.payload[HEADER_SIZE+11] & 0b00011111);
		Fix			= (uint8_t)((TelegramData.payload[HEADER_SIZE+11]& 0b01100000) >> 5);
		if((TelegramData.payload[HEADER_SIZE+11] & 0b10000000)){
			RunningOnBattery = true;
		}
			
		Pressure = 	(float)((TelegramData.payload[HEADER_SIZE+12] << 8) + TelegramData.payload[HEADER_SIZE+13]);
		GroundSpeed = (float)((TelegramData.payload[HEADER_SIZE+14] << 8) + TelegramData.payload[HEADER_SIZE+15]);
		GroundSpeed = GroundSpeed / 10;
			
		SecondsSinceLastGSContact = (uint8_t)TelegramData.payload[HEADER_SIZE+16];
		BatteryVoltage  = (float)TelegramData.payload[HEADER_SIZE+17];
		BatteryVoltage = (BatteryVoltage/100)+2.0;

		FirmwareVersion = (float)(TelegramData.payload[HEADER_SIZE+18]) + ((float)(TelegramData.payload[HEADER_SIZE+19]))/100;
			
		PCBVersion      = (uint8_t)TelegramData.payload[HEADER_SIZE+20];
		NumberOfBeaconsToRelay = (uint8_t)TelegramData.payload[HEADER_SIZE+21];
	}
}

Telegram_MSG_1::Telegram_MSG_1() : Telegram() 
{}

void Telegram_MSG_1::FixedPayload( void ) // debug!
{
	MSG_ID = MSG_Beacon_Broadcast;
	/*
	Unique_ID_1 = 123456;
	Unique_ID_2 = 123456;
	Unique_ID_3 = 123456;
	Unique_ID_4 = 123456;
	*/

	UTCTime =   (uint32_t)((TelegramData.payload[17] << 16) + (TelegramData.payload[18] << 8) + TelegramData.payload[19]);
	Latitude =  55562000;
	Longitude = 12281500;

	NumberOfSat = 12;
	Fix			= 2;
	RunningOnBattery = false;
	
	Pressure = 	0;
	GroundSpeed = 123;
	GroundSpeed = GroundSpeed / 10;
	
	SecondsSinceLastGSContact = 250;
	BatteryVoltage = 4.2;
	
	FirmwareVersion = 1.90;
	
	PCBVersion      = 11;
	NumberOfBeaconsToRelay = 0;
	
	TelegramData.rssi=10;
	TelegramData.snr=100; 
}


uint8_t Telegram_MSG_1::GetNumberOfSecondsSinceLastGroundStationCom( void )
{	
	return SecondsSinceLastGSContact;
}

uint32_t Telegram_MSG_1::GetUTCTime( void )
{
	return UTCTime;
}

int32_t Telegram_MSG_1::GetLatitude( void )
{
	return Latitude;
}

int32_t Telegram_MSG_1::GetLongitude( void )
{
	return Longitude;
}

uint8_t Telegram_MSG_1::GetNumberOfSat( void )
{
	return NumberOfSat;
}

uint8_t Telegram_MSG_1::GetFix( void )
{
	return Fix;
}

bool Telegram_MSG_1::GetRunningOnBattery( void )
{
	return RunningOnBattery;
}

float Telegram_MSG_1::GetPressure( void )
{
	return Pressure;
}

float Telegram_MSG_1::GetGroundSpeed( void )
{
	return GroundSpeed;
}

uint8_t Telegram_MSG_1::GetSecondsSinceLastGSContact( void )
{
	return SecondsSinceLastGSContact;
}

float Telegram_MSG_1::GetBatteryVoltage( void )
{
	return BatteryVoltage;
}

float Telegram_MSG_1::GetFirmwareVersion( void )
{
	return FirmwareVersion;
}


uint8_t Telegram_MSG_1::GetPCBVersion( void )
{
	return PCBVersion;
}


uint8_t Telegram_MSG_1::GetNumberOfBeaconsToRelay( void )
{
	return NumberOfBeaconsToRelay;
}


void Telegram_MSG_1::SerialPrintMessage( void )
{
	/*
		Serial.println("I Have received this:");
		Serial.println("");
		Serial.println("");
		Serial.println("Like this:");
		
		for(int a=0;a<PayloadLength;a++){
			Serial.println("Payload["+String(a)+"]=" + String(Payload[a]));
		}
		*//*
	SerialAUX->println("");
	SerialAUX->println("Message Header:");
	SerialAUX->println("MSG ID                 :" + String(this->MSG_ID));
	SerialAUX->println("From Unique ID         :" + String(this->Unique_ID_1) + String(this->Unique_ID_2) + String(this->Unique_ID_3) + String(this->Unique_ID_4));
	SerialAUX->println("Message Data:");
	SerialAUX->println("UTC  Time              :" + String(this->UTCTime));
	SerialAUX->println("Latitude               :" + String(this->Latitude));
	SerialAUX->println("Longitude              :" + String(this->Longitude));
	SerialAUX->println("# of satellites        :" + String(this->NumberOfSat));
	SerialAUX->println("Fix                    :" + String(this->Fix));
	if(this->RunningOnBattery)
		SerialAUX->println("Running on Battery!");
	else
		SerialAUX->println("Powered from external source");
	SerialAUX->println("Pressure              :" + String(this->Pressure));		
	SerialAUX->println("Ground Speed          :" + String(this->GroundSpeed));
	SerialAUX->println("SSLGC                 :" + String(this->SecondsSinceLastGSContact));
	SerialAUX->println("Battery Voltage       :" + String(this->BatteryVoltage));
	SerialAUX->println("Firmware Version      :" + String(this->FirmwareVersion));
	SerialAUX->println("PCB Version           :" + String(this->PCBVersion));
	SerialAUX->println("# of Beacons to relay :" + String(this->NumberOfBeaconsToRelay));
	SerialAUX->println("----------------------------------------------------------------");
	SerialAUX->println("");*/
}
