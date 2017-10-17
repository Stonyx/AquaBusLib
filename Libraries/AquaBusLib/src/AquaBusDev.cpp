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
#define DEBUG
#ifdef DEBUG
	#include <SoftwareSerial.h>
	extern SoftwareSerial Serial2;
  #define DEBUG_LOG(string) Serial2.print(string)
  #define DEBUG_LOG_LN(string) Serial2.println(string)
  #define DEBUG_LOG_HEX(string) Serial2.print(string,HEX)
#else
  #define DEBUG_LOG(string)
  #define DEBUG_LOG_LN(string)
#endif
#define DEBUG_LOG_FREE_RAM() DEBUG_LOG(F("Free RAM: ")); DEBUG_LOG_LN(FreeRam())


// Include header files
#include "modbus/include/mb.h"
#include "modbus/include/mbframe.h"
#include "modbus/rtu/mbrtu.h"
#include "AquaBusDev.h"
#include "AquaBusLib.h"

// Declare the function pointer used to send ModBus data
static peMBFrameSend peMBFrameSendCur;

// Constructor
AquaBusDev::AquaBusDev(byte hwId, unsigned short hwSerial, byte hwRevision, byte swRevision) : 
    hwId(hwId), hwSerial(hwSerial), hwRevision(hwRevision), swRevision(swRevision) 
{
  // Add outselves to the devices array
  AquaBusLib::addDevice(this);
}

// Function called to send data
// This function encapsulates the FreeModBus library function used to send the data
void AquaBusDev::sendData(byte* data, unsigned short length)
{
	DEBUG_LOG_LN("sendData");
  //peMBFrameSendCur(abAddress, data, length);
  eMBRTUSend(abAddress, data, length);
}