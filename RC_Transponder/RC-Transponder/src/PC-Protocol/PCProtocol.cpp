/*
	SerialProtocol.cpp

	Copyright (c) 2019 Lagoni
	Not for commercial use
 */ 
#include <Arduino.h>
#include "PCProtocol.h"  // For add and get function to RF protocol RX/TX FIFO

PCProtocol::PCProtocol(RFProtocol *inout, E28_2G4M20S *radio){
	this->_inout = inout;
	this->RadioForCRC = radio; // for CRC function.
	memset(&data, 0,  MAX_PAYLOAD_LENGTH);
}

void PCProtocol::WriteToSerial(RadioData_t *data)
{
		output[0]=0x1E; // Start byte
		output[1]=1+data->payloadLength+2; // Length (length byte + output.payloadlength +2 byte CRC;
		memcpy(&output[2], &data->payload[0], data->payloadLength);

		uint16_t temp_crc = RadioForCRC->CalculateCRC(&output[2], data->payloadLength);

		output[data->payloadLength+2] = (uint8_t)((temp_crc >>  8) & 0xFF);  // CRC
		output[data->payloadLength+3] = (uint8_t)(temp_crc & 0xFF);		     // CRC

		Serial.write(output, data->payloadLength+4);
}

  
 void PCProtocol::Service(){
	NumberOfBytesToRead = Serial.available();
	
	if(NumberOfBytesToRead == 0){
		// Handle TX
		if(_inout->Available()){
			RadioData_t *output = _inout->GetData();

			//Add RSSI and SNR to payload before sending it.
			if(output->payloadLength < MAX_PAYLOAD_LENGTH-2){
				output->payload[output->payloadLength]=output->rssi;
				output->payload[output->payloadLength+1]=output->snr;
				output->payloadLength+=2;
				WriteToSerial(output);
			}

		}
	}else{
		do
		{
			// input data: 0x1E [LENGTH] [PAYLOAD]
			if(NumberOfBytesToRead > 0)
			{
				_newChar=Serial.read(); // read char from input buffer
				 
				switch (SerialState)
				{
					case LOOKING_FOR_START: // look for 0x1E in incoming data
					if(_newChar == 0x1E)
					{
	//					SerialAUX->println("Start found!");
						dataLength = 0;    // counter number of bytes read.
						dataIndex = 0;
						memset(&data, 0,  MAX_PAYLOAD_LENGTH);
						memset(&input.payload, 0,  MAX_PAYLOAD_LENGTH);
						input.payloadLength=0;
						input.rssi=0;
						input.snr=0;
						CRC = 0;
						SerialState=READ_DATA_LENGTH; // Start of string found!
					}
					break;
					 
					case READ_DATA_LENGTH:
						if(_newChar >= MAX_PAYLOAD_LENGTH){
		//					SerialAUX->println("Length too long.");
							SerialState=LOOKING_FOR_START; // Error, restart:
						}
					 
						dataLength = _newChar;
		//				SerialAUX->println("Length is:" + String(dataLength));
						SerialState=READ_PAYLOAD_ID; // Start of string found!
					break;

					case READ_PAYLOAD_ID:
						PayloadID = (PayloadID_t)_newChar;

						//				SerialAUX->println("Length is:" + String(dataLength));
						SerialState=READ_PAYLOAD; 
					break;
					 
					case READ_PAYLOAD:
						data[dataIndex] = _newChar;
					 
						if(dataIndex < MAX_PAYLOAD_LENGTH){
							dataIndex++;
						}else{
							SerialState=LOOKING_FOR_START; // error restart.
						}
							 
						if(dataIndex >= (dataLength-2)){
							SerialState=READ_CRC1; // Done with Data payload, now go to CRC
						}
					break;
					 
					case READ_CRC1:
						CRC = (uint16_t)(_newChar<< 8);
						SerialState=READ_CRC2; 
					break;

					case READ_CRC2:
						CRC |= (uint16_t)_newChar;

						// done reading package, lets check it.
						if(RadioForCRC->CalculateCRC(&data[0] , dataLength) != CRC){
							SerialState=LOOKING_FOR_START; // CRC error restart.						
							break;
						}

						// If we get to here, CRC on new data is ok.
						switch(PayloadID) // MSG ID
						{
							case RADAR_APPLICATION_ID:
							{
								ApplicationCMDHandler();
							}
							break;
											
											
							case RADIO_DATA_TO_RF:
							case RADIO_DATA_TO_PC: 
							{
								RadioDataHandler();						
							}
							break;

							default:
											
							break;
						}

						SerialState=LOOKING_FOR_START; // Done, restart.
					break;

					default:
						SerialState = LOOKING_FOR_START; // error, restart.
					break;
				}
			}
		}while(NumberOfBytesToRead > 0); // loop until buffer is empty		 
	}
}

void PCProtocol::ApplicationCMDHandler(){
		ApplicationCMD_t CMD = (ApplicationCMD_t)data[0];

		switch(CMD) // MSG ID
		{
			case HW_REPLY: // Application ask if we are a transponder (ground station) - Reply with same message to ack.
			{
				// Groundstation needs a reply to indicate this devices is a ground station.
				input.payload[0] = RADAR_APPLICATION_ID;    // PAyload ID 0 (CMD_ID)
				input.payload[1] = HW_REPLY;			    // (HW_REPLY)
				input.payloadLength=2;
				WriteToSerial(&input);
			}
			break;

			default:		
			break;
		}	
}

void PCProtocol::RadioDataHandler(){
	
	switch(PayloadID) // PAyload Id eather RADIO_DATA_TO_RF or RADIO_DATA_TO_PC.
	{
		case RADIO_DATA_TO_RF: // Application ask if we are a transponder (ground station) - Reply with same message to ack.
		{
			
			memcpy(&input.payload[0], &data[0], dataLength-3);   //
			input.payloadLength=dataLength-3;			 // Payload is length minus CRC (2) minus PayloadID (1), thus 3.	
			input.rssi=0;
			input.snr=0;
			this->_inout->AddData(&input); // Add RadioData to RF protocol.
		}
		break;

		case RADIO_DATA_TO_PC: // loopback?
		break;

		default:
		break;
	}
}


						