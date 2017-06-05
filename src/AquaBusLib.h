// AquaBus Library
//
// This software is provided "as is" without express or implied warranty.

// Ensure this header file is included only once
#ifndef AquaBusLib_h
#define AquaBusLib_h

// Included header files
#include "modbus/include/mb.h"

// The AquaBusLib class
class AquaBusLib
{
  public:
    // Static callback functions
    static eMBException callbackFor0x01(byte *frame, unsigned short *length);
    static eMBException callbackFor0x20(byte *frame, unsigned short *length);

    // Constructor
    AquaBusLib();

    // Setup and loop functions
    void setup();
    void loop();  
};

#endif