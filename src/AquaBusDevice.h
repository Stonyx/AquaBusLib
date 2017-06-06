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

// Ensure this header file is included only once
#ifndef AquaBusDevice_h
#define AquaBusDevice_h

// Include header files
#include <arduino.h>

// The AquaBusDevice class
class AquaBusDevice
{
  // Make the AquaBusLib class friends with this class
  friend class AquaBusLib;

  public:
    // Constant fields
    const byte hwId;
    const unsigned short hwSerial;
    const byte hwRevision;
    const byte swRevision;

    // Constructor
    AquaBusDevice(byte hwId, unsigned short hwSerial, byte hwRevision, byte swRevision) : 
        hwId(hwId), hwSerial(hwSerial), hwRevision(hwRevision), swRevision(swRevision) {}

  protected:
    virtual void processData(byte* data, unsigned short length) = 0;
    inline void sendData(byte address, byte* pucFrame, unsigned short length);
};

#endif