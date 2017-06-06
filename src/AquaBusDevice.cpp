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
#include "AquaBusDevice.h"
#include "modbus/include/mb.h"
#include "modbus/include/mbframe.h"

// Declare the function pointer used to send ModBus data
static peMBFrameSend peMBFrameSendCur;

// Function called to send data
// This function encapsulates the FreeModBus library function used to send the data
void AquaBusDevice::sendData(byte address, byte* pucFrame, unsigned short length)
{
  peMBFrameSendCur(address, pucFrame, length);
}