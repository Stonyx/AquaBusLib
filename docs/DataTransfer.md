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
```
+---------------+--------------------------------------------------------------------------------------------+
| Function Code | Description                                                                                |
+---------------+--------------------------------------------------------------------------------------------+
| 0x01          | Device Probe Request. Used to attach new and existing devices to Apex                      |
| 0x07          | Device firmware update Request. Used by Apex to push firmware updates to connected devices |
+---------------+--------------------------------------------------------------------------------------------+
```

## Apex Device Communication

As Apex uses Modbus for device communication, the head unit always acts as the Modbus master. All attached devices are configured as Modbus slaves. Each device is given a unique address. The head unit always initiates a communication. Slave devices respond to commands from the head unit.

### Introducing a new device to Apex

Apex head unit sends a probe request to all devices connected to the network. If a new device is attached, it response to the probe request, which initiates a series of requests that eventually result in the device being attached to Apex.

```
struct AB_PROBE_REQUEST_PACKET
{
  byte FunctionCode;
  byte ProbeStage;
  byte nextAddress;
  unsigned short ApexSerialNumber;
  byte unknown[3];
}
```

Apex sends this request to the broadcast address 0x00 for all devices to pick up. 
