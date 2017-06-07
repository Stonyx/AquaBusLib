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
#ifndef AquaBusDev_h
#define AquaBusDev_h

// Include header files
#include <arduino.h>

// The AquaBusDevice class
class AquaBusDev
{
  // Make the AquaBusLib class friends with this class
  friend class AquaBusLib;

  public:
    // Constant member fields
    const byte hwId;
    const unsigned short hwSerial;
    const byte hwRevision;
    const byte swRevision;

  protected:
    // Constructor
    AquaBusDev(byte hwId, unsigned short hwSerial, byte hwRevision, byte swRevision);

    // Member functions
    virtual void processData(byte* data, unsigned short length) = 0;
    inline void sendData(byte address, byte* pucFrame, unsigned short length);
};

#endif