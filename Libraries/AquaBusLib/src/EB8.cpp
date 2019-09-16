// AquaBus Library
// Copyright (C) 2017
//
// This software is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0
// International License.
//
// You can redistribute and/or modify this software for non-commerical purposes under the terms
// of the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
//
// This software is provided "as is" without express or implied warranty.

// Debug related definitions
//#define DEBUG
#ifdef DEBUG
	#include <SoftwareSerial.h>
	extern SoftwareSerial DebugSerial; // 6 is RX, 7 is TX
  #define DEBUG_LOG(string) DebugSerial.print(string)
  #define DEBUG_LOG_LN(string) DebugSerial.println(string)
#else
  #define DEBUG_LOG(string)
  #define DEBUG_LOG_LN(string)
#endif
#define DEBUG_LOG_FREE_RAM() DEBUG_LOG(F("Free RAM: ")); DEBUG_LOG_LN(FreeRam())

// Include header files
#include "EB8.h"

const unsigned long sampleTime = 100000UL;                           // sample over 100ms, it is an exact number of cycles for both 50Hz and 60Hz mains
const unsigned long numSamples = 40UL;                               // choose the number of samples to divide sampleTime exactly, but low enough for the ADC to keep up
const unsigned long sampleInterval = sampleTime/numSamples;  // the sampling interval, must be longer than then ADC conversion time
const int adc_zero = 0;                                        // relative digital zero of the arudino input from ACS712 (could make this a variable and auto-adjust it)
const float CSA1V_sensitivity_coef = 43.5625;  //comes from solving for X, 5.2275 V / X = .120 V / 1 A from the arduino max analog input voltage and the CSA-1V sensitivity on the spec sheet respectively.             
const float ACS712_sensitivity_coef = 75.7576; //comes from solving for X, 5 V / X = .066 V / 1 A from the arduino max analog input voltage and the ACS712 sensitivity on the spec sheet respectively. 

AB_EB8_RESPONSE_FRAME EB8::EB8ResponseFrame = {};
AB_EB8_EEPROM_RESPONSE_FRAME EB8::EEPROMResponseFrame = {};

//EB8::EB8(unsigned short serial) : AquaBusDev(APEX_MODULE_EB8, serial, 0x01, 0x0C), outletStates(0)
//{
//  
//}

void EB8::Init()
{
	//sensor.calibrate();
	sensor.setZeroPoint(eeprom_read_dword(deviceID*4 + 12*3));
}

void EB8::SetCurSensor(ACS712_type type, uint8_t _pin)
{
	sensor.init(type, _pin);
}

void EB8::SetOutletPins(byte outPins[])
{
	for (int i = 0; i < 8; i++)
	{
		outletPins[i] = outPins[i];
	}
}

// Function called to get the state of an outlet
bool EB8::getOutletState(byte outlet)
{
  return (outletStates >> outlet) & 1;
}

// Function called to process recieved data
void EB8::processData(byte deviceABAddr, byte* data, unsigned short length)
{
	float currentAcc = 0;
  CurrentReading rawCur = {0,0};
  int zero = 0;
  
  if (((AB_EB8_REQUEST_PACKET*)data)->RequestType == 3)
	{
		zero = sensor.calibrate();
		eeprom_update_dword(deviceID*4 + 12*3, zero);
	}
  else if(((AB_EB8_REQUEST_PACKET*)data)->RequestType == 1)
  {
	  // Check if we should update the outlet states
		outletStates = ((AB_EB8_REQUEST_PACKET*)data)->OutletStateBitmap;
		for (int i = 0; i < 8; i++)
		{
		 	digitalWrite(outletPins[i], !getOutletState(i));
		}
	}

	//take current reading
	rawCur = sensor.getRawCurrentAC(175);
			
	//convert current reading to CSA1V data
			
	currentAcc = rawCur.Isum * pow(2,12) * VREF*VREF / (pow(sensor.getSensitivity(),2) * 2788*2788);
	//currentAcc = pow(sqrt(rawCur.Isum/rawCur.measurementCount) * pow(2,6) * VREF / sensor.getSensitivity() / 2788,2)*rawCur.measurementCount;
	//currentAcc = 1.31739 * rawCur.Isum;
			
	// Create the response structure

	EB8ResponseFrame.response.funcCode = (((AB_EB8_REQUEST_PACKET*)data)->funcCode);
	EB8ResponseFrame.response.RequestType = (((AB_EB8_REQUEST_PACKET*)data)->RequestType);
	EB8ResponseFrame.response.OutletStateCurrent = outletStates;
	EB8ResponseFrame.response.unknown = outletStates; // Seems to match the outletStates variable usually
	EB8ResponseFrame.response.legacyCurrent = static_cast<unsigned long>(currentAcc) & 0xFFFF;//161130 & 0xFFFF;
	EB8ResponseFrame.response.frequency = rawCur.measurementCount;
	EB8ResponseFrame.response.rawCurrent = static_cast<unsigned long>(currentAcc);//161130;

	// Send the response
	sendData(deviceABAddr, (byte*)(&EB8ResponseFrame.response), sizeof(AB_EB8_RESPONSE_PACKET)); 
}

void EB8::processEEPROMRequest(byte deviceABAddr, byte* data, unsigned short length)
{
	int index = 0;
	byte requestedLocation = 0;
	byte requestedLength = 0;
	int i = 0;
	
	DEBUG_LOG_LN("processEEPROMRequest");
	switch(((AB_EEPROM_GET_SET_REQUEST_PACKET*)data)->RequestType)
	{
		case 1://GET
			DEBUG_LOG_LN("processEEPROMRequest 1");
			//not quite sure what this is for yet
			//Apex expects two bytes of eeprom back
			break;
		case 2://SET
			DEBUG_LOG_LN("processEEPROMRequest 2");
			//on abdelete Apex tells us to clear ApexSerial from eeprom
			//data->location should be byte 1 of eeprom
			//data->length should be 2 bytes
			//this matches our internal eeprom format
			requestedLocation = ((AB_EEPROM_GET_SET_REQUEST_PACKET*)data)->Location;
			requestedLength = ((AB_EEPROM_GET_SET_REQUEST_PACKET*)data)->Length;
			

			if(requestedLocation == 0)
			{
				//erasing device config
				requestedLength = 3;
				i = deviceID * requestedLength;
			}
			else
			{
				i = requestedLocation;	
			}

			for (; i < requestedLength; i++)
			{
				eeprom_update_byte(i, ((AB_EEPROM_GET_SET_REQUEST_PACKET*)data)->data[index]);
				index++;
			}
			
			break;
		default:
			DEBUG_LOG_LN("processEEPROMRequest default");
			break;
	}
	
	EEPROMResponseFrame.response.funcCode = (((AB_EEPROM_GET_SET_REQUEST_PACKET*)data)->funcCode);
	EEPROMResponseFrame.response.RequestType = (((AB_EEPROM_GET_SET_REQUEST_PACKET*)data)->RequestType);
	EEPROMResponseFrame.response.unknown_1 = 0;
	EEPROMResponseFrame.response.unknown_2 = 0;
	EEPROMResponseFrame.response.unknown_3 = 0;
	EEPROMResponseFrame.response.unknown_4 = 0;
	
	
	sendData(deviceABAddr, (byte*)(&EEPROMResponseFrame.response), sizeof(AB_EEPROM_GET_SET_RESPONSE_PACKET));
}