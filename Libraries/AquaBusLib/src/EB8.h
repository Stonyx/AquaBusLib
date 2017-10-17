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
#ifndef EB8_h
#define EB8_h

// Include header files
#include <arduino.h>
#include "AquaBusDev.h"

// The EB8 class
class EB8 : public AquaBusDev
{
  public:
    // Constructors
    EB8(unsigned short serial) : 
        AquaBusDev(0x20, serial, 0x01, 0x0C), outletStates(0) {}
    EB8(unsigned short serial, byte swRevision) :
        AquaBusDev(0x20, serial, 0x01, swRevision), outletStates(0) {}

    // Member functions
    bool getOutletState(byte outlet);

  protected:
    // Member variables
    byte outletStates;

    // Member functions
    void processData(byte* data, unsigned short length);
};

#endif