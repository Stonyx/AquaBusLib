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
