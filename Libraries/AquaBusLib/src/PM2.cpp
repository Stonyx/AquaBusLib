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
#include "PM2.h"

AB_PM2_DATA_RESPONSE_FRAME PM2::PM2DataResponseFrame = {};
AB_PM2_INIT_RESPONSE_FRAME PM2::PM2InitResponseFrame = {};
AB_PM2_EEPROM_RESPONSE_FRAME PM2::EEPROMResponseFrame = {};

//ctor
PM2::PM2(unsigned short serial):AquaBusDev(APEX_MODULE_PM2, serial, 0x01, 0x03)
{
  ModuleStatus.ProbeRangeOriginal = eeprom_read_byte(3);
	ModuleStatus.TempProbeOffset = eeprom_read_word(4);
	ModuleStatus.ConductivityProbeOffset = eeprom_read_word(6);
	ModuleStatus.Unknown_1 = eeprom_read_word(8);
	ModuleStatus.Unknown_2 = eeprom_read_word(10);
	ModuleStatus.TempProbeScale = eeprom_read_word(12);
	ModuleStatus.ConductivityProbeScale = eeprom_read_word(14);
	ModuleStatus.Unknown_3 = eeprom_read_word(16);
	ModuleStatus.Unknown_4 = eeprom_read_word(18);
}
// Function called to process recieved data
void PM2::processData(byte deviceABAddr, byte* data, unsigned short length)
{
  byte requestType = (((AB_PM2_REQUEST_PACKET*)data)->RequestType);
  
  switch (requestType)
  {
  	case 1: //init
  		PM2InitResponseFrame.response.FunctionCode = (((AB_PM2_REQUEST_PACKET*)data)->FunctionCode);
		  PM2InitResponseFrame.response.RequestType = (((AB_PM2_REQUEST_PACKET*)data)->RequestType);
		  PM2InitResponseFrame.response.ProbeRangeOriginal = ModuleStatus.ProbeRangeOriginal;
			PM2InitResponseFrame.response.TempProbeOffset = ModuleStatus.TempProbeOffset;
			PM2InitResponseFrame.response.ConductivityProbeOffset = ModuleStatus.ConductivityProbeOffset;
			PM2InitResponseFrame.response.Unknown_1 = ModuleStatus.Unknown_1;
			PM2InitResponseFrame.response.Unknown_2 = ModuleStatus.Unknown_2;
			PM2InitResponseFrame.response.TempProbeScale = ModuleStatus.TempProbeScale;
			PM2InitResponseFrame.response.ConductivityProbeScale = ModuleStatus.ConductivityProbeScale;
			PM2InitResponseFrame.response.Unknown_3 = ModuleStatus.Unknown_3;
			PM2InitResponseFrame.response.Unknown_4 = ModuleStatus.Unknown_4;
			 // Send the response
		  sendData(deviceABAddr, (byte*)(&PM2InitResponseFrame.response), sizeof(AB_PM2_INIT_RESPONSE_PACKET));
		  break;
  	case 2: //calibrate
  		PM2InitResponseFrame.response.FunctionCode = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->FunctionCode;
		  PM2InitResponseFrame.response.RequestType = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->RequestType;
		  PM2InitResponseFrame.response.ProbeRangeOriginal = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->ProbeRangeOriginal;
			PM2InitResponseFrame.response.TempProbeOffset = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->TempProbeOffset;
			PM2InitResponseFrame.response.ConductivityProbeOffset = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->ConductivityProbeOffset;
			PM2InitResponseFrame.response.Unknown_1 = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->Unknown_1;
			PM2InitResponseFrame.response.Unknown_2 = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->Unknown_2;
			PM2InitResponseFrame.response.TempProbeScale = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->TempProbeScale;
			PM2InitResponseFrame.response.ConductivityProbeScale = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->ConductivityProbeScale;
			PM2InitResponseFrame.response.Unknown_3 = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->Unknown_3;
			PM2InitResponseFrame.response.Unknown_4 = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->Unknown_4;
			 // Send the response
		  sendData(deviceABAddr, (byte*)(&PM2InitResponseFrame.response), sizeof(AB_PM2_INIT_RESPONSE_PACKET));
		  
		  eeprom_update_byte(3, ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->ProbeRangeOriginal);
		  eeprom_update_word(4, ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->TempProbeOffset);
		  eeprom_update_word(6, ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->ConductivityProbeOffset);
		  eeprom_update_word(8, ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->Unknown_1);
		  eeprom_update_word(10, ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->Unknown_2);
		  eeprom_update_word(12, ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->TempProbeScale);
		  eeprom_update_word(14, ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->ConductivityProbeScale);
		  eeprom_update_word(16, ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->Unknown_3);
		  eeprom_update_word(18, ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->Unknown_4);
  		
  		ModuleStatus.ProbeRangeOriginal = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->ProbeRangeOriginal;
			ModuleStatus.TempProbeOffset = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->TempProbeOffset;
			ModuleStatus.ConductivityProbeOffset = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->ConductivityProbeOffset;
			ModuleStatus.Unknown_1 = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->Unknown_1;
			ModuleStatus.Unknown_2 = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->Unknown_2;
			ModuleStatus.TempProbeScale = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->TempProbeScale;
			ModuleStatus.ConductivityProbeScale = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->ConductivityProbeScale;
			ModuleStatus.Unknown_3 = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->Unknown_3;
			ModuleStatus.Unknown_4 = ((AB_PM2_CALIBRATE_REQUEST_PACKET*)data)->Unknown_4;
  		
  		break;
  	case 3:
  	case 4:
  	case 5: //get data
  		PM2DataResponseFrame.response.FunctionCode = (((AB_PM2_REQUEST_PACKET*)data)->FunctionCode);
		  PM2DataResponseFrame.response.RequestType = (((AB_PM2_REQUEST_PACKET*)data)->RequestType);
		  PM2DataResponseFrame.response.ProbeRangeOriginal = ModuleStatus.ProbeRangeOriginal;
			PM2DataResponseFrame.response.CondReading = 0x348A;
			PM2DataResponseFrame.response.TempReading = 0x2244;
			PM2DataResponseFrame.response.Unknown = 0x4018;
			PM2DataResponseFrame.response.SwitchState = 2;
			 // Send the response
  		sendData(deviceABAddr, (byte*)(&PM2DataResponseFrame.response), sizeof(AB_PM2_DATA_RESPONSE_PACKET));
  		break;
  	default:
  		break;
  }
}

void PM2::processEEPROMRequest(byte deviceABAddr, byte* data, unsigned short length)
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