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
#include "EB8.h"

// Function called to get the state of an outlet
bool EB8::getOutletState(byte outlet)
{
  return (outletStates >> outlet) & 1;
}

// Function called to process recieved data
void EB8::processData(byte* data, unsigned short length)
{
  // Define the request structure
  struct Request
  {
    byte type;
    byte outletStates;
    byte unknown;
  };

  // Switch based on the request type
  switch (((Request*)data)->type)
  {
    case 2:
      // Update the outlet states
      outletStates = ((Request*)data)->outletStates;

      // Create the response structure
      struct
      {
        byte hwId;
        byte type;
        byte outletStates;
        byte unknown;
        unsigned short legacyCurrent;
        unsigned short frequency;
        unsigned long rawCurrent;
      } response;
      response.hwId = hwId;
      response.type = (((Request*)data)->type);
      response.outletStates = outletStates;
      response.unknown = outletStates;
      response.legacyCurrent = 0;
      response.frequency = 0;
      response.rawCurrent = 0;

      // Send the response
      // TODO: figure out which address to actually send the response to
      sendData(address, (byte*)&response, sizeof(response));
      break;
  }
}