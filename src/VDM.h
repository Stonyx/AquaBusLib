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

#define VDM_PROFILE_TYPE_PUMP 1
#define VDM_PROFILE_TYPE_RAMP 2
#define VDM_PROFILE_TYPE_VORTECH 3
#define VDM_PROFILE_TYPE_WEATHER 4
#define VDM_PROFILE_TYPE_RADION 5
#define VDM_PROFILE_TYPE_VEGA 6
#define VDM_PROFILE_TYPE_DOSE 7
#define VDM_PROFILE_TYPE_WAV 8

#define VDM_AI_LIGHT_TYPE_STANDARD 0
#define VDM_AI_LIGHT_TYPE_SOL_BLUE 1
#define VDM_AI_LIGHT_TYPE_4_CHANNEL 2

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

struct AB_VDM_EEPROM_RESPONSE_FRAME
{
	byte address;
	AB_EEPROM_GET_SET_RESPONSE_PACKET response;
	unsigned short crc;
} __attribute__((packed));

struct AB_VDM_COMMON_REQUEST_PACKET
{
	byte FuncCode;
	byte RequestType;
	byte ModuleOutletID;
	byte OutletState;
	byte ProfileType;
	byte ProfileID;
} __attribute__((packed));

struct AB_VDM_PUMP_REQUEST_PACKET
{
	AB_VDM_COMMON_REQUEST_PACKET common;
	byte minIntensity;
	byte maxIntensity;
	byte Flags;
	byte InitialoffTime;
	byte zero1;
	byte onTime;
	byte zero2;
	byte offTime;
	byte zero3;
	byte MaxIntensityPercent;
	unsigned long OutletAddTime;
	unsigned long CurrentTime;
} __attribute__((packed));

struct AB_VDM_WEATHER_REQUEST_PACKET
{
	AB_VDM_COMMON_REQUEST_PACKET common;
	byte lightningInt;
	unsigned short field_7;
	byte field_9;
	byte minIntensity;
	byte maxIntensity;
	byte lightningCloud;
	unsigned short cloudDur;
	byte cloudPer;
	byte MaxIntensityPercent;
	unsigned long OutletAddTime;
	unsigned long CurrentTime;
} __attribute__((packed));

struct AB_VDM_RESPONSE_PACKET
{
	byte FuncCode;
	byte ResponseType;
	byte ModuleOutletID;
	byte AIType;
	byte field_4;
	byte CurrentIntensity;
} __attribute__((packed));

struct AB_VDM_RESPONSE_FRAME
{
	byte address;
	AB_VDM_RESPONSE_PACKET response;
	unsigned short crc;
} __attribute__((packed));

// The VDM class
class VDM : public AquaBusDev
{
  public:
    // Constructors
    VDM(unsigned short serial); 
    VDM(unsigned short serial, byte swRevision) :
        AquaBusDev(APEX_MODULE_VDM, serial, 0x01, swRevision) {}
    static AB_VDM_RESPONSE_FRAME VDMResponseFrame;
    static AB_VDM_EEPROM_RESPONSE_FRAME EEPROMResponseFrame;

  protected:
    // Member functions
    void processData(byte deviceABAddr, byte* data, unsigned short length);
    void processEEPROMRequest(byte deviceABAddr, byte* data, unsigned short length);
    
    // Member variables
    //VDM_STATUS_STRUCT ModuleStatus;
};

#endif