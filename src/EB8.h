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
#include "ACS712.h"

struct AB_EB8_RESPONSE_PACKET
{
  byte funcCode;
  byte RequestType;
  byte OutletStateCurrent;
  byte unknown;
  unsigned short legacyCurrent;
  unsigned short frequency;
  unsigned long rawCurrent;
} __attribute__((packed));

struct AB_EEPROM_GET_SET_REQUEST_PACKET
{
  byte funcCode;
  byte RequestType;
  byte Location;
  byte Length;
  byte data[8];
} __attribute__((packed));

struct AB_EEPROM_GET_SET_RESPONSE_PACKET
{
	byte funcCode;
  byte RequestType;
  byte unknown_1;
  byte unknown_2;
  byte unknown_3;
  byte unknown_4;
} __attribute__((packed));

struct AB_EB8_EEPROM_RESPONSE_FRAME
{
	byte address;
	AB_EEPROM_GET_SET_RESPONSE_PACKET response;
	unsigned short crc;
} __attribute__((packed));

// Define the request structure
struct AB_EB8_REQUEST_PACKET
{
  byte funcCode;
	byte RequestType;
	byte OutletStateBitmap;
  byte unknown;
}__attribute__((packed));

struct AB_EB8_RESPONSE_FRAME
{
	byte address;
	AB_EB8_RESPONSE_PACKET response;
	unsigned short crc;
} __attribute__((packed));

// The EB8 class
class EB8 : public AquaBusDev
{
  public:
    // Constructors
    EB8(unsigned short serial) : 
        AquaBusDev(APEX_MODULE_EB8, serial, 0x01, 0x0C), outletStates(0) {}
    EB8(unsigned short serial, byte swRevision) :
        AquaBusDev(APEX_MODULE_EB8, serial, 0x01, swRevision), outletStates(0) {}
    static AB_EB8_RESPONSE_FRAME EB8ResponseFrame;
    static AB_EB8_EEPROM_RESPONSE_FRAME EEPROMResponseFrame;

    // Member functions
    bool getOutletState(byte outlet);
    void SetOutletPins(byte outPins[]);
    void SetCurSensor(ACS712_type type, uint8_t _pin);
    void Init();

  protected:
    // Member variables
    byte outletStates;
    byte currentPin;
    byte outletPins[8];
    ACS712 sensor;

    // Member functions
    void processData(byte deviceABAddr, byte* data, unsigned short length);
    void processEEPROMRequest(byte deviceABAddr, byte* data, unsigned short length);
};

#endif