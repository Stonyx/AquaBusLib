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
#ifndef VDM_h
#define VDM_h

// Include header files
#include <arduino.h>
#include "AquaBusDev.h"

// The VDM class
class VDM : public AquaBusDev
{
  public:
    // Constructors
    VDM(unsigned short serial) : 
        AquaBusDev(0x23, serial, 0x01, 0x0D) {}
    VDM(unsigned short serial, byte swRevision) :
        AquaBusDev(0x23, serial, 0x01, swRevision) {}

  protected:
    // Member functions
    void processData(byte* data, unsigned short length);
};

#endif