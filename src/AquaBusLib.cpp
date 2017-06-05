// AquaBus Library

// Include header files
#include "modbus/include/mb.h"
#include "AquaBusLib.h"

// Constructor
AquaBusLib::AquaBusLib()
{
}

// Function that needs be called from the sketch setup function
AquaBusLib::setup()
{
  // Initialize and enable the FreeModBus library
  eMBInit(MB_RTU, 0x00, 0, 19200, MB_PAR_EVEN);
  eMBEnable();
}

// Function that needs to be called fro the sketch loop function
AquaBusLib::loop()
{
  eMBPoll();
}

// Callback function called to get or set the value of a Coil Register
eMBErrorCode eMBRegCoilsCB (UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode)
{  
}

// Callback function called to get the value of an Input Discrete Register
eMBErrorCode eMBRegDiscreteCB (UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNDiscrete)
{  
}

// Callback function called to get or set the value of a Holding Register
eMBErrorCode eMBRegHoldingCB (UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode)
{  
}

// Callback function called to get the value of an Input Register
eMBErrorCode eMBRegInputCB (UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{  
}