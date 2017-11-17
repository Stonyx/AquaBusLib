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

// Include header files
#include "PM1.h"

// Function called to process recieved data
void PM1::processData(byte* data, unsigned short length)
{
  // Define the request structure
  struct Request
  {
  };

  // Create the response structure
  struct
  {
  } response;

  // Send the response
  sendData(0, (byte*)&response, sizeof(response));
}