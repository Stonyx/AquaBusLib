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
	extern SoftwareSerial Serial2; // 6 is RX, 7 is TX
  #define DEBUG_LOG(string) Serial2.print(string)
  #define DEBUG_LOG_LN(string) Serial2.println(string)
#else
  #define DEBUG_LOG(string)
  #define DEBUG_LOG_LN(string)
#endif
#define DEBUG_LOG_FREE_RAM() DEBUG_LOG(F("Free RAM: ")); DEBUG_LOG_LN(FreeRam())

// Include header files
#include "EB8.h"

AB_EB8_RESPONSE_FRAME EB8::EB8ResponseFrame = {};
AB_EB8_EEPROM_RESPONSE_FRAME EB8::EEPROMResponseFrame = {};

// Function called to get the state of an outlet
bool EB8::getOutletState(byte outlet)
{
  return (outletStates >> outlet) & 1;
}

// Function called to process recieved data
void EB8::processData(byte deviceABAddr, byte* data, unsigned short length)
{
  // Check if we should update the outlet states
  if (((AB_EB8_REQUEST_PACKET*)data)->RequestType == 1)
    outletStates = 0xAA;

  // Create the response structure

  EB8ResponseFrame.response.funcCode = (((AB_EB8_REQUEST_PACKET*)data)->funcCode);
  EB8ResponseFrame.response.RequestType = (((AB_EB8_REQUEST_PACKET*)data)->RequestType);
  EB8ResponseFrame.response.OutletStateCurrent = outletStates;
  EB8ResponseFrame.response.unknown = outletStates; // Seems to match the outletStates variable usually
  EB8ResponseFrame.response.legacyCurrent = 161130 & 0xFFFF;
  EB8ResponseFrame.response.frequency = 40;
  EB8ResponseFrame.response.rawCurrent = 161130;

  // Send the response
  sendData(deviceABAddr, (byte*)(&EB8ResponseFrame.response), sizeof(AB_EB8_RESPONSE_PACKET));
}

void EB8::processEEPROMRequest(byte deviceABAddr, byte* data, unsigned short length)
{
	int index = 0;
	
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
			for (int i = ((AB_EEPROM_GET_SET_REQUEST_PACKET*)data)->Location; i < ((AB_EEPROM_GET_SET_REQUEST_PACKET*)data)->Length; i++)
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