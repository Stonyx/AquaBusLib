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
	extern SoftwareSerial DebugSerial;
  #define DEBUG_LOG(string) DebugSerial.print(string)
  #define DEBUG_LOG_LN(string) DebugSerial.println(string)
  #define DEBUG_LOG_HEX(string) DebugSerial.print(string,HEX)
#else
  #define DEBUG_LOG(string)
  #define DEBUG_LOG_LN(string)
#endif
#define DEBUG_LOG_FREE_RAM() DEBUG_LOG(F("Free RAM: ")); DEBUG_LOG_LN(FreeRam())

// Include header files
#include <string.h>
#include "AquaBusLib.h"

// Static member variables
AquaBusDev** AquaBusLib::devices = NULL;
byte AquaBusLib::devicesCount = 0;
AB_PROBE_RESPONSE_FRAME AquaBusLib::ProbeResponseFrame = {};
AB_PROBE_RESPONSE_STAGE5_FRAME AquaBusLib::ProbeResponseStage5Frame = {};


// Static callback function for handling initial probing
eMBException AquaBusLib::probeCallback(byte address, byte* frame, unsigned short length)
{
	DEBUG_LOG_LN("probeCallback enter");
  // Define the request structure
  //int i = 0;
  struct AB_PROBE_REQUEST_PACKET
  {
    byte code;
    byte stage;
    byte nextAddress;
    unsigned short hwSerial;
    byte unknown[3];
  }__attribute__((packed));

	if (address != 0)
		return MB_EX_NONE;
		
	// Loop through the devices
  for (int i = 0; i < devicesCount; i++)
	{
			byte ABRetAddr = 0;
			
			for (int j = 0; j < devicesCount; j++)
			{
				//cannot respond to a new stage 1 probe request while there are other devices in probe progress
				if (devices[j]->probeStage > 0 && devices[j]->probeStage < 5 && ((AB_PROBE_REQUEST_PACKET*)frame)->stage == 1)
					return MB_EX_NONE;
			}
			//check if already attached
			if (devices[i]->probeStage == 5 && ((AB_PROBE_REQUEST_PACKET*)frame)->stage != 5)
			{
				DEBUG_LOG("probeCallback: ");
				DEBUG_LOG(i);
				DEBUG_LOG_LN(" Already Attached");
				continue;
			}
				
		  // Check if we should ignore this stage
			if (((AB_PROBE_REQUEST_PACKET*)frame)->stage == 4 || ((AB_PROBE_REQUEST_PACKET*)frame)->stage > 5)
		    return MB_EX_NONE;

		  DEBUG_LOG("probeCallback: ");
			DEBUG_LOG(i);
			DEBUG_LOG(" Attaching. Stage");
			DEBUG_LOG_LN(((AB_PROBE_REQUEST_PACKET*)frame)->stage);
		  
		  if ((((AB_PROBE_REQUEST_PACKET*)frame)->stage == 1 || ((AB_PROBE_REQUEST_PACKET*)frame)->stage == 2) && devices[i]->abAddress && devices[i]->ApexSerial)
		  {
		  	//this device has already been registered with an apex
		  	//attempt to reattach existing		
		  	ProbeResponseFrame.response.nextAddress = devices[i]->abAddress;
		  	ProbeResponseFrame.response.hwSerial = devices[i]->ApexSerial;
		  }
		  else
		  {
		  	ProbeResponseFrame.response.nextAddress = ((AB_PROBE_REQUEST_PACKET*)frame)->nextAddress;
		    ProbeResponseFrame.response.hwSerial = ((AB_PROBE_REQUEST_PACKET*)frame)->hwSerial;
		  }
		  
		  if (((AB_PROBE_REQUEST_PACKET*)frame)->stage == 3)
		  {
		  	//this is the probeSET request
		  	//save new device config to eeprom
		  	devices[i]->abAddress = ((AB_PROBE_REQUEST_PACKET*)frame)->nextAddress;
		  	devices[i]->ApexSerial = ((AB_PROBE_REQUEST_PACKET*)frame)->hwSerial;
		  	eeprom_update_byte(i*(sizeof(devices[i]->abAddress)+sizeof(devices[i]->ApexSerial)), devices[i]->abAddress);
		  	eeprom_update_word(i*(sizeof(devices[i]->abAddress)+sizeof(devices[i]->ApexSerial))+sizeof(devices[i]->abAddress), devices[i]->ApexSerial);
		  }
		  
		  ProbeResponseFrame.response.hwId = devices[i]->hwId;
		  
		  if (((AB_PROBE_REQUEST_PACKET*)frame)->stage == 5)
		  {
		  	//cannot jump from nothing to stage 5 unless reattaching existing
		  	if (devices[i]->probeStage < 3 && (devices[i]->abAddress == 0 || devices[i]->ApexSerial == 0))
		  		continue;
		  	ProbeResponseStage5Frame.response.hwId = devices[i]->hwId;
		  	if (devices[i]->probeStage == 5)
		  		ProbeResponseStage5Frame.response.hwId |= 0x80;
		  	ABRetAddr = 0xFF;
		  	ProbeResponseStage5Frame.response.nextAddress = ((AB_PROBE_REQUEST_PACKET*)frame)->nextAddress;
		    ProbeResponseStage5Frame.response.hwSerial = ((AB_PROBE_REQUEST_PACKET*)frame)->hwSerial;
		  	ProbeResponseStage5Frame.address = ABRetAddr;//((AB_PROBE_REQUEST_PACKET*)frame)->nextAddress;
		    ProbeResponseStage5Frame.response.code = ((AB_PROBE_REQUEST_PACKET*)frame)->code;
		    ProbeResponseStage5Frame.response.stage = ((AB_PROBE_REQUEST_PACKET*)frame)->stage;
		    ProbeResponseStage5Frame.response.hwRevision = devices[i]->hwRevision;
		    ProbeResponseStage5Frame.response.swRevision = devices[i]->swRevision;
		    
		    devices[i]->probeStage = ((AB_PROBE_REQUEST_PACKET*)frame)->stage;
		    devices[i]->sendData(ABRetAddr, (byte*)(&ProbeResponseStage5Frame.response), sizeof(AB_PROBE_RESPONSE_STAGE5_PACKET));
		  }
		  else
		  {
		    ProbeResponseFrame.address = ABRetAddr;//((AB_PROBE_REQUEST_PACKET*)frame)->nextAddress;
		    ProbeResponseFrame.response.code = ((AB_PROBE_REQUEST_PACKET*)frame)->code;
		    ProbeResponseFrame.response.stage = ((AB_PROBE_REQUEST_PACKET*)frame)->stage;
		    ProbeResponseFrame.response.hwRevision = devices[i]->hwRevision;
		    ProbeResponseFrame.response.swRevision = devices[i]->swRevision;
		    ProbeResponseFrame.response.unknown[0] = 0;
		    ProbeResponseFrame.response.unknown[1] = 0;
		    ProbeResponseFrame.response.unknown[2] = 0;

				devices[i]->probeStage = ((AB_PROBE_REQUEST_PACKET*)frame)->stage;
				devices[i]->sendData(ABRetAddr, (byte*)(&ProbeResponseFrame.response), sizeof(AB_PROBE_RESPONSE_PACKET));
			}
			
			if (((AB_PROBE_REQUEST_PACKET*)frame)->stage == 5)
			{
				DEBUG_LOG("probeCallback: ");
				DEBUG_LOG(i);
				DEBUG_LOG_LN(" Attached");
			}	
			
			break;
	}
	DEBUG_LOG_LN("probeCallback exit");
  return MB_EX_NONE;
}

// Static callback function for handling device communication
eMBException AquaBusLib::deviceCallback(byte address, byte* frame, unsigned short length)
{
	//int i = 0;
  //DEBUG_LOG_LN("deviceCallback enter");
  //DEBUG_LOG("deviceCallback");
  //DEBUG_LOG("devicesCount = ");
  //DEBUG_LOG_LN(devicesCount);
  //DEBUG_LOG("address = ");
  //DEBUG_LOG_LN(address);
  
  // Loop through the devices
  for (int i = 0; i < devicesCount; i++)
	{
    // Check if the address matches the device's AquaBus address
    if (address == devices[i]->abAddress)
    {
    	//DEBUG_LOG_LN("deviceCallback: calling processData");
      // Call the device's processData function
      devices[i]->processData(address, frame, length);
      
      break;
    }
    else
    {
    	//DEBUG_LOG_LN("deviceCallback: abAddress doesn't match");		
    }
  }

	//DEBUG_LOG_LN("deviceCallback exit");
  return MB_EX_NONE;
}

// Static callback function for handling device communication "Code 0x21"
eMBException AquaBusLib::deviceCallbackCode21(byte address, byte* frame, unsigned short length)
{
	return deviceCallback(address, frame, length);
}

// Static callback function for handling device EEPROM communication
eMBException AquaBusLib::deviceEEPROMCallback(byte address, byte* frame, unsigned short length)
{
	//int i = 0;
  // Loop through the devices
  for (int i = 0; i < devicesCount; i++)
	{
    // Check if the address matches the device's AquaBus address
    if (address == devices[i]->abAddress)
    {
      devices[i]->processEEPROMRequest(address, frame, length);
      
      break;
    }
    else
    {
    	DEBUG_LOG_LN("deviceEEPROMCallback: abAddress doesn't match");		
    }
	}
  return MB_EX_NONE;
}

// Constructor
AquaBusLib::AquaBusLib(byte numberOfDevices)
{
  DEBUG_LOG_LN("AquaBusLib ctor enter");
  // Check if the devices array has to be allocated
  if (devices == NULL)
  {
    // Allocate and initalize memory for the number of devices specified and save the devices count
    devices = new AquaBusDev*[numberOfDevices]();
    devicesCount = numberOfDevices;
  }
#ifdef DEBUG
  // Check if a different device count was specified than before
  else if (numberOfDevices != devicesCount)
  {
    DEBUG_LOG_LN("WARNING: Creating a second AquaBusLib object with a different number of devices");
  }
  else
  {
    DEBUG_LOG_LN("WARNING: Creating a second AquaBusLib object");
  }
#endif
	DEBUG_LOG_LN("AquaBusLib ctor exit");
}

// Function called from the sketch's setup function
void AquaBusLib::setup()
{
  // Initialize the FreeModBus library
  DEBUG_LOG_LN("eMBInit enter");
  eMBErrorCode ret = MB_ENOERR;
  ret = eMBInit(MB_RTU, MB_ADDRESS_MAX, 0, 19200, MB_PAR_EVEN);
  
  if (ret != MB_ENOERR)
  	DEBUG_LOG_LN("ERROR: eMBInit");
  else
  	DEBUG_LOG_LN("eMBInit success");

  // Register the probe and device callback functions with the FreeModBus library
  ret = eMBRegisterCB(0x01, probeCallback);
  if (ret != MB_ENOERR)
  	DEBUG_LOG_LN("ERROR: eMBRegisterCB probeCallback");
  else
  	DEBUG_LOG_LN("eMBRegisterCB probeCallback success");
  	
  ret = eMBRegisterCB(0x20, deviceCallback);
  if (ret != MB_ENOERR)
  	DEBUG_LOG_LN("ERROR: eMBRegisterCB deviceCallback");
  else
  	DEBUG_LOG_LN("eMBRegisterCB deviceCallback success");
  	
  ret = eMBRegisterCB(0x21, deviceCallbackCode21);
  if (ret != MB_ENOERR)
  	DEBUG_LOG_LN("ERROR: eMBRegisterCB deviceCallbackCode21");
  else
  	DEBUG_LOG_LN("eMBRegisterCB deviceCallbackCode21 success");
  	
   ret = eMBRegisterCB(0x8, deviceEEPROMCallback);
  if (ret != MB_ENOERR)
  	DEBUG_LOG_LN("ERROR: eMBRegisterCB deviceEEPROMCallback");
  else
  	DEBUG_LOG_LN("eMBRegisterCB deviceEEPROMCallback success");

  // Enable the FreeModBus library
  ret = eMBEnable();
  if (ret != MB_ENOERR)
  	DEBUG_LOG_LN("ERROR: eMBEnable");
  else
  	DEBUG_LOG_LN("eMBEnable success");
  	
  DEBUG_LOG_LN("MB Enabled");
}

// Function called from the sketch's loop function
void AquaBusLib::loop()
{
	eMBErrorCode ret = MB_ENOERR;
  ret = eMBPoll();
  if (ret != MB_ENOERR)
  	DEBUG_LOG_LN("ERROR: eMBPoll");
  //else
  	//DEBUG_LOG_LN("eMBPoll success");
}

// Function called to add a device
void AquaBusLib::addDevice(AquaBusDev* device)
{
  DEBUG_LOG_LN("addDevice enter");
  // Loop through the devices
  int i;
  for (i = 0; i < devicesCount; i++)
  {
    // Check if this slot is available and save the passed in device
    if (devices[i] == NULL)
    {
    	DEBUG_LOG_LN("Adding device");
      //Get device AB configuration from eeprom if available
      device->abAddress = eeprom_read_byte(i*(sizeof(device->abAddress)+sizeof(device->ApexSerial)));
  		device->ApexSerial = eeprom_read_word(i*(sizeof(device->abAddress)+sizeof(device->ApexSerial))+sizeof(device->abAddress));
      device->deviceID = i;
      devices[i] = device;
      
      break;
    }
  }

#ifdef DEBUG
  // Check if the devices array was full
  if (i == devicesCount)
    DEBUG_LOG_LN("WARNING: Trying to add too many devices");
#endif
}