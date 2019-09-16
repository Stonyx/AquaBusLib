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
#include "VDM.h"

AB_VDM_RESPONSE_FRAME VDM::VDMResponseFrame = {};
AB_VDM_EEPROM_RESPONSE_FRAME VDM::EEPROMResponseFrame = {};
	
//ctor
VDM::VDM(unsigned short serial):AquaBusDev(APEX_MODULE_VDM, serial, 0x01, 0xD)
{
//  ModuleStatus.ProbeRangeOriginal = eeprom_read_byte(3);
//	ModuleStatus.pH_DOProbeOffset = eeprom_read_word(4);
//	ModuleStatus.TempProbeOffset = eeprom_read_word(6);
//	ModuleStatus.ORPProbeOffset = eeprom_read_word(8);
//	ModuleStatus.pH_DOProbeScale = eeprom_read_word(10);
//	ModuleStatus.TempProbeScale = eeprom_read_word(12);
//	ModuleStatus.ORPProbeScale = eeprom_read_word(14);
//	ModuleStatus.Unknown_3 = eeprom_read_word(16);
//	ModuleStatus.Unknown_4 = eeprom_read_word(18);
	
}

// Function called to process recieved data
void VDM::processData(byte deviceABAddr, byte* data, unsigned short length)
{
	DEBUG_LOG_LN("VDM ProcessData 1");
  byte requestType = (((AB_VDM_COMMON_REQUEST_PACKET*)data)->RequestType);
  byte profileType = (((AB_VDM_COMMON_REQUEST_PACKET*)data)->ProfileType);
  
  switch (requestType)
  {
  	case 1: //control
  		DEBUG_LOG_LN("VDM ProcessData 2");
  		switch (profileType)
  		{
  			case VDM_PROFILE_TYPE_PUMP:
  			case VDM_PROFILE_TYPE_RAMP:
  			case VDM_PROFILE_TYPE_WEATHER:
					DEBUG_LOG_LN("VDM ProcessData 3");
  				VDMResponseFrame.response.FuncCode = (((AB_VDM_PUMP_REQUEST_PACKET*)data)->common.FuncCode);
					VDMResponseFrame.response.ResponseType = 2;
					VDMResponseFrame.response.ModuleOutletID = (((AB_VDM_PUMP_REQUEST_PACKET*)data)->common.ModuleOutletID);
					VDMResponseFrame.response.AIType = VDM_AI_LIGHT_TYPE_STANDARD;
					VDMResponseFrame.response.field_4 = 1;
					VDMResponseFrame.response.CurrentIntensity = 250;
			 		// Send the response
		  		sendData(deviceABAddr, (byte*)(&VDMResponseFrame.response), sizeof(AB_VDM_RESPONSE_PACKET));
		  		DEBUG_LOG_LN("VDM ProcessData 4");
		  		break;
  			default:
  				break;
  		}
  	default:
  		break;
  }
}

void VDM::processEEPROMRequest(byte deviceABAddr, byte* data, unsigned short length)
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