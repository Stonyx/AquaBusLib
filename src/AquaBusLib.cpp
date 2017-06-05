// AquaBus Library
//
// This software is provided "as is" without express or implied warranty.

// Include header files
#include <arduino.h>
#include <string.h>
#include "AquaBusLib.h"

// Static callback function for the ModBus 0x01 function code
eMBException AquaBusLib::callbackFor0x01(byte *frame, unsigned short *length)
{
  // Get the data from the frame
  struct
  {
    byte function;
    byte stage;
    byte address;
    byte serial;
  } request;
  memcpy(&request, frame, sizeof(request));

  // Create the reponse
  struct
  {
    byte unknown_1;
    byte unknown_2;
    byte hardwareType;
    byte hardwareRevision;
    byte softwareRevision;
    byte address;
    word serial;
    byte buffer[0];
  } response;
  memcpy(frame, &response, sizeof(response));

  return MB_EX_NONE;
}

// Static callback function for the ModBux 0x20 function code
eMBException AquaBusLib::callbackFor0x20(byte *frame, unsigned short *length)
{
  return MB_EX_NONE;
}

// Constructor
AquaBusLib::AquaBusLib()
{
  // Register callback functions with the FreeModBus library
  eMBRegisterCB(0x01, callbackFor0x01);
  eMBRegisterCB(0x20, callbackFor0x20);
}

// Function that needs be called from the sketch setup function
void AquaBusLib::setup()
{
  // Initialize and enable the FreeModBus library
  eMBInit(MB_RTU, 0x00, 0, 19200, MB_PAR_EVEN);
  eMBEnable();
}

// Function that needs to be called fro the sketch loop function
void AquaBusLib::loop()
{
  eMBPoll();
}