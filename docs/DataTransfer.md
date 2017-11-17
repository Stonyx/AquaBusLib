## Neptune Apex Data Transfer

Although, Apex uses CAN transceiver at the physical layer, it does not implement CAN for data transfers. Instead, Apex uses [Modbus
RTU](https://en.wikipedia.org/wiki/Modbus) to communicate with addon modules and for arbitration. Specifically, it relies heavily
on [freemodbus](https://www.freemodbus.org) library for Modbus data transfer implementation.

### Apex Modbus configuration:
Baudrate: 19200  
Data Bits: 8  
Parity: Even  
Stop bits: 1  

### Apex Modbus Frame structure:
```
<------------------------ MODBUS SERIAL LINE PDU (1) ------------------->
             <----------- MODBUS PDU (1') ---------------->
 +-----------+---------------+----------------------------+-------------+
 | Address   | Function Code | Data                       | CRC/LRC     |
 +-----------+---------------+----------------------------+-------------+
 ```
 
 Where:
- Address - one byte modbus address of the destination device 
- Function Code - one byte function code tells destination device what to do
- Data - rest of the packet
- CRC/LRC - Modbus CRC16 over the entire frame, starting at "Address" until the end of the packet.

### Known Apex Function Codes

This is the list of known Function Codes supported by Apex:

+---------------+------------------------------------------------------------------------------------+
| Function Code | Description                                                                        |
+---------------+------------------------------------------------------------------------------------+
| 0x01          | Device Probe Request. Used to attach new and existing devices to Apex              |
+---------------+------------------------------------------------------------------------------------+
| 0x07          | Device firmware update Request. Used by Apex to push firmware updates to           |
|               | connected devices                                                                  |
+---------------+------------------------------------------------------------------------------------+
| 0x08          | Device EEPROM Request. Used by Apex to set and get EEPROM of attached device       |
+---------------+------------------------------------------------------------------------------------+
| 0x11          | Apex Display Request. Used to communicate with Apex Display                        |
+---------------+------------------------------------------------------------------------------------+
| 0x20          | Device Communication Request. Apex uses this function code to send commands to     |
|               | already connected devices                                                          |
+---------------+------------------------------------------------------------------------------------+

Table 1: Known Apex Function Codes

## Apex Device Communication

As Apex uses Modbus for device communication, the head unit always acts as the Modbus master. All attached devices are configured as Modbus slaves. Each device is given a unique address. The head unit always initiates a communication. Slave devices respond to commands from the head unit.

### Introducing new device to Apex

Apex head unit sends a probe request to all devices connected to the network. If a new device is attached, it response to the probe request, which initiates a series of requests that eventually result in the device being attached to Apex.

```
struct AB_PROBE_REQUEST_PACKET
{
  byte FunctionCode;
  byte ProbeStage;
  byte Address;
  unsigned short ApexSerialNumber;
  byte unknown[3];
}
```

Apex sends this request to the broadcast address 0x00 for all devices to pick up. FunctionCode is 0x01 for all Probe stages. Here's the list of probe stages:
```
+---------------+------------------------------------------------------------------------------------+
| Probe Stage   | Description                                                                        |
+---------------+------------------------------------------------------------------------------------+
| 0x01          | Initial stage of probe request. Apex sends this to broadcast looking for newly     |
|               | attached devices                                                                   |
+---------------+------------------------------------------------------------------------------------+
| 0x02          | Second state of probe request. Apex sends this if the response from initial stage  |
|               | came from device not previously registered with Apex                               |
+---------------+------------------------------------------------------------------------------------+
| 0x03          | Third stage of probe request. Apex agreed to install the new device and is in    |
|               | final configuration stage                                                          |
+---------------+------------------------------------------------------------------------------------+
| 0x05          | Final Stage of probe request. Also called the Attach stage. This is the request    |
|               | sent to new devices telling them that they are now attached and ready to go.       |
|               | This is also the request sent to known devices on reattach.                        |
+---------------+------------------------------------------------------------------------------------+
```
Table 2: Probe Request Stages

#### Probe Request Stage 1
In the first probe stage, Apex sends two important pieces of information: Next available AquaBus address and Apex's serial number. This way the device being probed can check if it has already been previously attached to this Apex and on what address. Or if this is a new device, it can take the next available AquaBus address on registration.
The first two AquaBus addresses (0x01 and 0x02) are always reserved for up to two Displays. Typically, the next available address (0x03) gets taken by the Power Bar (Energy Bar), but this is not strictly reserved for it.
Here's an example of a complete initial probe request:
```
00 01 01 03 34 12 00 00 00 6F 73
```
Where:
  0x00 - Modbus broadcast address  
  0x01 - Device Probe Request  
  0x01 - Initial Stage Probe Request  
  0x03 - Next available Modbus (AquaBus) address  
  0x1234 - Apex Serial Number  
  0x000000 - unknown. Appears to be reserved and unused  
  0x736F - Modbus CRC16 checksum  
  
Since this is a new device, it takes Address and ApexSerialNumber values from the request, verifies internally that it has no record of previous connections to this Apex head unit, and send the response back. The structure of the response is as follows:
```
struct AB_PROBE_RESPONSE_PACKET
{
	byte FunctionCode;
	byte ProbeStage;
	byte hwId;
	byte hwRevision;
	byte swRevision;
	byte ABAddress;
	unsigned short ApexSerial;
	byte unknown[3];
}
```
In the response, the device keeps the original Function Code and stage Probe Stage provided in the request. It also claims the available AB address in ABAddress field and acqnowledges Apex Serial nuber received. In addition, it also provides its hardware ID, hardware revision number and software (firmware) revision number. Apex uses this information to validate that the device can be supported by the version of Apex that it is trying to attach to. 

As of Apex firmware update 4.52_5A17, the following list of devices is known:
```
+-----------------+-------+------+----------+----------+
|Module           | HW_ID |HW_Rev|SW_Rev_Min|SW_Rev_Max|
+-----------------+-------+------+----------+----------+
|Display          |  0x1  |   1  |  10      |    11    |
|PM1              |  0x11 |   1  |   4      |     7    |
|PM2              |  0x12 |   1  |   2      |     3    |
|PM3              |  0x13 |   1  |   3      |     7    |
|ALD              |  0x14 |   1  |   7      |     7    |
|ASM              |  0x15 |   1  |   7      |     7    |
|FMM              |  0x16 |   1  |   5      |     5    |
|EB8              |  0x20 |   1  |   9      |    12    |
|WXM              |  0x21 |   1  |  10      |    11    |
|EB4              |  0x22 |   1  |   9      |    12    |
|VDM              |  0x23 |   1  |  13      |    13    |
|LSM              |  0x24 |   1  |  13      |    13    |
|EB6              |  0x25 |   1  |  11      |    12    |
|AWM              |  0x26 |   1  |   7      |     7    |
|AFS              |  0x27 |   1  |   2      |     2    |
|DOS              |  0x28 |   1  |   7      |     7    |
|WAV              |  0x29 |   3  |  16      |    16    |
|1Link            |  0x2A |   1  |   4      |     4    |
+-----------------+-------+------+----------+----------+
```
Table 3: List of Apex Modules

From this table, Apex checks hwID from the response to match one in the table. This allows Apex to choose how to handle the new device. swRevision from the response must be within SW_Rev_min and SW_Rev_max in the table. Otherwise, Apex will either attempt to update the firmware or refuse attaching the device.

#### Probe Request Stage 2

Apex may issues Probe Request Stage 2 if the device being attached has never previously registered with Apex. The Stage 2 request packet is identical to Stage 1, except for the updated ProbeStage field set to 2. The Address field is set to the same address as before. It is also sent to modbus broadcast address (0x00). The device response is also identical to the response to Stage 1 except for the ProbeStage Field.

#### Probe Request Stage 3
This is the SET stage of the new device probe. In this stage, Apex agrees to install the new device and updates it's own internal structures with information about the device. Apex sends the Stage 3 request to let the device know that it is set and allow the device to update it's internal stage as well with information about the Apex head unit it is now connected to, such as Apex serial number and AB address. Otherwise, information in Stage 3 request and response is identical to the previous two stages. The Address field is set to the same address as before. It is also sent to modbus broadcast address (0x00). After this stage, the device will be recognized by the Apex even after power loss or loss of communication.

#### Probe Request Stage 5
This is the Attach stage of the probe cycle. Apex sends this request only to previously set devices. The format of the request and the expected response repeat previous probe stages. The Address field is set to the same address as before. It is also sent to modbus broadcast address (0x00). This request tells the device that it's now attached to Apex and should expect regular communication from this point on. The response tells Apex that the device acknowledges attachment and is ready for communication.

### Reconnecting existing module to Apex
Once the device is initialized with Apex, meaning that it has gone through probe request stages 1-3, it can then be reattached to Apex with a single "Attach" request. This request assumes that the device already knows its AB address and the Apex serial number. If this is the case, the device first must respond to the initial Probe Stage 1 request, but instead of choosing the next available AB address from the request packet, it responds with its own AB address previously assigned and Apex's serial number. Upon receiving the response, Apex verifies that it already has a record of the device, and sends Probe Request Stage 5 "Attach" immediately. The request is still sent to modbus broadcast address (0x00). This request tells the device that it's now attached to Apex and should expect regular communication from this point on. The response tells Apex that the device acknowledges attachment and is ready for communication.

## Device Specific Apex Communication

This section covers communication protocols and considerations specific to particular Apex modules. For the complete list of Apex modules refer to Table 3.

### Energy Bar - 8 outlets (EB8)

For the purpose of this discussion, we will only consider two features of this device: 
- 8 controllable outlets
- Current sensing combined across all outlets

Once this module is connected to Apex and completes the initial probe sequence. It is ready to receive EB8 specific requests from Apex. EB8 supports two types of requests:
- Set outlets and get current reading
- Calibrate current sensor

```
+---------------+------------------------------------------------------------------------------------+
| Request Type  | Description                                                                        |
+---------------+------------------------------------------------------------------------------------+
| 0x01          | Set EB8 outlets states and get current reading                                     |
+---------------+------------------------------------------------------------------------------------+
| 0x03          | Calibrate (zero out) EB8 current sensor                                            |
+---------------+------------------------------------------------------------------------------------+
```
Table 4: Available EB8 Request Types

#### Setting outlets and getting current reading
EB8 and most other Apex modules use the same function code to communicate with connected modules. The format of EB8 request is as follows:
```
struct AB_EB8_REQUEST_PACKET
{
  byte FunctionCode;
  byte RequestType;
  byte OutletStateBitmap;
  byte unknown;
}
```
In this packet:
FunctionCode - 0x20  
RequestType - 0x01  
OutletStateBitmap - Bitmap of outlets that should be ON  
unknown - The purpose of this is not immediately clear, in some instances it seems to repeat OutletStateBitmap.  

The OutletStateBitmap byte tells EB8 which outlets should be in ON or OFF position. For example:
```
01000111
```
Sets outlets 1,2,3 and 7 to ON and outlets 4,5,6 and 8 to OFF position.

EB8 module responds to this request as follows:
```
struct AB_EB8_RESPONSE_PACKET
{
  byte FunctionCode;
  byte RequestType;
  byte OutletStateCurrent;
  byte unknown;
  unsigned short legacyCurrent;
  unsigned short frequency;
  unsigned long rawCurrent;
}
```
FunctionCode and RequestType fields repeat what was sent in the request.  
OutletStateCurrent returns the bitmap of outlets in their current state.  
unknown - the purpose of this is not currently clear  
legacyCurrent - reports current reading in legacy format  
frequency - Frequency constant to calculate Amperage. EB8 reports this as 40 (decimal) 
rawCurrent - reports current reading in full format  

It appears that EB8 uses RMS method to report total current. The formula used by apex to arrive at the final amperage number is:
```
AmpCurrent = ((sqrt(divsi3(RawCurrent,Frequency))*0x6ce8)/2^16)/10
```

#### Calibrating EB8 current sensor

EB8 supports calibration of its current sensor. The way this works is with nothing plugged into EB8 to draw any current, a user can issue a telnet command
```
eb8zero
```
This would then send a message to EB8 module to adjust the delta it uses to calculate rawCurrent. The format of the eb8zero request is as follows:
```
struct AB_EB8ZERO_REQUEST_PACKET
{
  byte FunctionCode;
  byte RequestType;
  byte Reserved[2];
}
```
Where:
FunctionCode - 0x20  
RequestType - 0x03  
Reserved - Not used, uninitialized  

EB8 response to this request follows the standard AB_EB8_RESPONSE_PACKET format.
