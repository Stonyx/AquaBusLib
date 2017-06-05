// AquaBus Library
//
// This software is provided "as is" without express or implied warranty.

// Ensure this header file is included only once
#ifndef AquaBusLib_h
#define AquaBusLib_h

// The AquaBusLib class
class AquaBusLib
{
  public:
    enum Device
    {
      ALD,
      EB8,
      PM1,
      VDM
    };
    AquaBusLib();
    void setup(Device device);
    void loop();

  protected:
    Device mDevice;
};

#endif