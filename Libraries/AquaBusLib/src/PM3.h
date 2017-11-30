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
#ifndef PM3_h
#define PM3_h

// Include header files
#include <arduino.h>
#include "AquaBusDev.h"

#define PROBE_TYPE_None 0
#define PROBE_TYPE_Temp 1
#define PROBE_TYPE_pH   2
#define PROBE_TYPE_ORP  4
#define PROBE_TYPE_DO   8
#define PROBE_TYPE_Cond 0x40	

#define DO_PROBE_RANGE_TYPE_SAT 0
#define DO_PROBE_RANGE_TYPE_PPM 1

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

struct AB_PM3_EEPROM_RESPONSE_FRAME
{
	byte address;
	AB_EEPROM_GET_SET_RESPONSE_PACKET response;
	unsigned short crc;
} __attribute__((packed));

struct AB_PM3_REQUEST_PACKET
{
	byte FunctionCode;
	byte RequestType;
}__attribute__((packed));

struct AB_PM3_CALIBRATE_REQUEST_PACKET
{
	byte FunctionCode;
	byte RequestType;
  byte ProbeRangeOriginal;
  unsigned short pH_DOProbeOffset;
  unsigned short TempProbeOffset;
  unsigned short ORPProbeOffset;
  unsigned short pH_DOProbeScale;
  unsigned short TempProbeScale;
  unsigned short ORPProbeScale;
  unsigned short Unknown_3;
  unsigned short Unknown_4;
}__attribute__((packed));

struct AB_PM3_DATA_RESPONSE_PACKET
{
	byte FunctionCode;
	byte RequestType;
	byte ProbeRangeOriginal;
	unsigned short pH_DOReading;
	unsigned short TempReading;
	unsigned short ORPReading;
	unsigned short SwitchState;
}__attribute__((packed));

struct AB_PM3_DATA_RESPONSE_FRAME
{
	byte address;
	AB_PM3_DATA_RESPONSE_PACKET response;
	unsigned short crc;
} __attribute__((packed));

struct AB_PM3_INIT_RESPONSE_PACKET
{
	byte FunctionCode;
	byte RequestType;
	byte ProbeRangeOriginal;
	unsigned short pH_DOProbeOffset;
	unsigned short TempProbeOffset;
	unsigned short ORPProbeOffset;
	unsigned short pH_DOProbeScale;
	unsigned short TempProbeScale;
	unsigned short ORPProbeScale;
	unsigned short Unknown_3;
	unsigned short Unknown_4;
}__attribute__((packed));

struct AB_PM3_INIT_RESPONSE_FRAME
{
	byte address;
	AB_PM3_INIT_RESPONSE_PACKET response;
	unsigned short crc;
} __attribute__((packed));

struct PM3_STATUS_STRUCT
{
	byte ProbeRangeOriginal;
	unsigned short pH_DOProbeOffset;
	unsigned short TempProbeOffset;
	unsigned short ORPProbeOffset;
	unsigned short pH_DOProbeScale;
	unsigned short TempProbeScale;
	unsigned short ORPProbeScale;
	unsigned short Unknown_3;
	unsigned short Unknown_4;
}__attribute__((packed));

// The PM3 class
class PM3 : public AquaBusDev
{
  public:
    // Constructors
    PM3(unsigned short serial);
    PM3(unsigned short serial, byte swRevision) :
        AquaBusDev(APEX_MODULE_PM3, serial, 0x01, swRevision) {}
    static AB_PM3_DATA_RESPONSE_FRAME PM3DataResponseFrame;
    static AB_PM3_INIT_RESPONSE_FRAME PM3InitResponseFrame;
    static AB_PM3_EEPROM_RESPONSE_FRAME EEPROMResponseFrame;

  protected:
    // Member functions
    void processData(byte deviceABAddr, byte* data, unsigned short length);
    void processEEPROMRequest(byte deviceABAddr, byte* data, unsigned short length);
    
    // Member variables
    PM3_STATUS_STRUCT ModuleStatus;
};

#endif