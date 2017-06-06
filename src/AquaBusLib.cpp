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
  /*
  // Get the data from the frame
  struct
  {
    byte function;
    byte stage;
    byte address;
    byte serial;
  } request;
  memcpy(&request, frame, sizeof(request));

  // Create the reponse
  struct
  {
    byte unknown_1;
    byte unknown_2;
    byte hardwareType;
    byte hardwareRevision;
    byte softwareRevision;
    byte address;
    word serial;
    byte buffer[0];
  } response;
  memcpy(frame, &response, sizeof(response));
  */

  return MB_EX_NONE;
}

// Static callback function for handling device communication
eMBException AquaBusLib::deviceCallback(byte* frame, unsigned short* length)
{
  // Loop through the devices
  for (int i = 0; i < sizeof(devices) / sizeof(AquaBusDevice*); ++i)
  {
    // Check if the first byte of the frame which is the function code matches the device ID
    if (frame[0] == devices[i]->id)
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
  for (int i = 0; i < sizeof(devices) / sizeof(AquaBusDevice*); ++i)
  {
    // Register the device callback function with the FreeModBus library using the device ID 
    //   as the function code
    eMBRegisterCB(devices[i]->id, deviceCallback);
  }

  // Enable the FreeModBus library
  eMBEnable();
}

// Function called from the sketch's loop function
void AquaBusLib::loop()
{
  eMBPoll();
}