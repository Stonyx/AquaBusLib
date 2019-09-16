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
#include "AquaBusDev.h"

struct AB_PROBE_RESPONSE_PACKET
{
	byte code;
	byte stage;
	byte hwId;
	byte hwRevision;
	byte swRevision;
	byte nextAddress;
	unsigned short hwSerial;
	byte unknown[3];
}__attribute__((packed));
    
struct AB_PROBE_RESPONSE_FRAME
{
	byte address;
	AB_PROBE_RESPONSE_PACKET response;
	unsigned short crc;
}__attribute__((packed));

struct AB_PROBE_RESPONSE_STAGE5_PACKET
{
	byte code;
	byte stage;
	byte hwId;
	byte hwRevision;
	byte swRevision;
	byte nextAddress;
	unsigned short hwSerial;
}__attribute__((packed));
    
struct AB_PROBE_RESPONSE_STAGE5_FRAME
{
	byte address;
	AB_PROBE_RESPONSE_STAGE5_PACKET response;
	unsigned short crc;
}__attribute__((packed));

// The AquaBusLib class
class AquaBusLib
{
  // Make the AquaBusDev class friends with this class
  friend class AquaBusDev;

  public:
    // Static member functions
    static eMBException probeCallback(byte address, byte* frame, unsigned short length);
    static eMBException deviceCallback(byte address, byte* frame, unsigned short length);
    static eMBException deviceCallbackCode21(byte address, byte* frame, unsigned short length);
    static eMBException deviceEEPROMCallback(byte address, byte* frame, unsigned short length);
    static AB_PROBE_RESPONSE_FRAME ProbeResponseFrame;
    static AB_PROBE_RESPONSE_STAGE5_FRAME ProbeResponseStage5Frame;

    // Constructor
    AquaBusLib(byte numberOfDevices);

    // Member functions
    void setup();
    void loop();

  protected:
    // Static member variables
    static AquaBusDev** devices;
    static byte devicesCount;

    // Static member functions
    static void addDevice(AquaBusDev* device);
};

#endif