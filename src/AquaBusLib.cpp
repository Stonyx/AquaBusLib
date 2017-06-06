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
#include <string.h>
#include "AquaBusLib.h"

// Static member variables
static AquaBusDevice** AquaBusLib::devices;

// Static callback function for handling initial probing
eMBException AquaBusLib::probeCallback(byte* frame, unsigned short* length)
{
  // Check the length of the frame
  if (*length == 8)
  {
    // Define the request structure for frames with this length
    struct Request
    {
      byte functionCode;
      byte probeStage;
      byte nextAddress;
      word hwSerial;
      byte unknown[3];
    };

    // Define the response structure
    struct Response
    {
      byte functionCode;
      byte probeStage;
      byte hwId;
      byte hwRevision;
      byte swRevision;
      byte nextAddress;
      word hwSerial;
      byte unknown[3];
    };

    // Loop through the devices
    for (byte i = 0; i < sizeof(devices) / sizeof(AquaBusDevice*); ++i)
    {
      // Switch based on the probe stage
      switch (((Request*)frame)->probeStage)
      {
        case 1:
          // Prepare the response structure
          Response response;
          response.functionCode = ((Request*)frame)->functionCode;
          response.probeStage = ((Request*)frame)->probeStage;
          response.hwId = devices[i]->hwId;
          // response.hwRevision
          // response.swRevision
          response.nextAddress = ((Request*)frame)->nextAddress;
          // response.serialNumber
          break;
        case 2:
          break;
        case 3:
          break;
        case 5:
          break;
      }
    }
  }
  else if (*length == 5)
  {
    // Define the request structore for frames with this length
    struct Request
    {
      byte functionCode;
      byte probeStage;
      byte abAddress;
      byte unknown[2];
    };
  }

  return MB_EX_NONE;
}

// Static callback function for handling device communication
eMBException AquaBusLib::deviceCallback(byte* frame, unsigned short* length)
{
  // Loop through the devices
  for (byte i = 0; i < sizeof(devices) / sizeof(AquaBusDevice*); ++i)
  {
    // Check if the first byte of the frame which is the function code matches the device ID
    if (frame[0] == devices[i]->hwId)
    {
      // Call the device's processData function
      devices[i]->processData(frame[1], length - 1);
    }
  }

  return MB_EX_NONE;
}

// Constructor
AquaBusLib::AquaBusLib(byte numberOfDevices)
{
  // Allocate memory for the number of devices specified
  devices = new AquaBusDevice*[numberOfDevices];
}

// Destructor
AquaBusLib::~AquaBusLib()
{
  // Free up memory
  delete devices;
}

// Function called to add a device
void AquaBusLib::addDevice(byte number, AquaBusDevice& device)
{
  // Save the passed in device
  devices[number] = &device;
}

// Function called to initialize all devices that have been added
void AquaBusLib::initializeDevices()
{
  // Initialize the FreeModBus library
  eMBInit(MB_RTU, 0x00, 0, 19200, MB_PAR_EVEN);

  // Register the probe callback function with the FreeModBus library
  eMBRegisterCB(0x01, probeCallback);

  // Loop through the devices
  for (byte i = 0; i < sizeof(devices) / sizeof(AquaBusDevice*); ++i)
  {
    // Register the device callback function with the FreeModBus library using the device ID 
    //   as the function code
    eMBRegisterCB(devices[i]->hwId, deviceCallback);
  }

  // Enable the FreeModBus library
  eMBEnable();
}

// Function called from the sketch's loop function
void AquaBusLib::loop()
{
  eMBPoll();
}