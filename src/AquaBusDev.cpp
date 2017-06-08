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

// Include header files
#include "modbus/include/mb.h"
#include "modbus/include/mbframe.h"
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
void AquaBusDev::sendData(byte abAddress, byte* data, unsigned short length)
{
  peMBFrameSendCur(abAddress, data, length);
}