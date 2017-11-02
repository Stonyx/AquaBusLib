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
#ifndef PM1_h
#define PM1_h

// Include header files
#include <arduino.h>
#include "AquaBusDev.h"

// The PM1 class
class PM1 : public AquaBusDev
{
  public:
    // Constructors
    PM1(unsigned short serial) : 
        AquaBusDev(0x11, serial, 0x01, 0x07) {}
    PM1(unsigned short serial, byte swRevision) :
        AquaBusDev(0x11, serial, 0x01, swRevision) {}

  protected:
    // Member functions
    void processData(byte* data, unsigned short length);
};

#endif