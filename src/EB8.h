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
#include "AquaBusDevice.h"

// Define default device properties
#define EB8_HW_ID 0x20
#define EB8_HW_REVISION 0x01
#define EB8_SW_REVISION 0x0C

// The EB8 class
class EB8 : public AquaBusDevice
{
  public:
    // Constructors
    EB8(unsigned short serial) : AquaBusDevice(EB8_HW_ID, serial, EB8_HW_REVISION, EB8_SW_REVISION) {}

    // Function called to process received data
    virtual void processData(byte* data, unsigned short length);
};

#endif