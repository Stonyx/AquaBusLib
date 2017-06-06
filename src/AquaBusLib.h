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
#ifndef AquaBusLib_h
#define AquaBusLib_h

// Included header files
#include <arduino.h>
#include "modbus/include/mb.h"
#include "AquaBusDevice.h"
#include "EB8.h"

// The AquaBusLib class
class AquaBusLib
{
  public:
    // Static member functions
    static eMBException probeCallback(byte* frame, unsigned short* length);
    static eMBException deviceCallback(byte* frame, unsigned short* length);

    // Constructor and destructor
    AquaBusLib(byte numberOfDevices);
    virtual ~AquaBusLib();

    // Various functions
    void addDevice(byte number, AquaBusDevice& device);
    void initializeDevices();
    void loop();

  protected:
    // Static member variables
    static AquaBusDevice** devices;
};

#endif