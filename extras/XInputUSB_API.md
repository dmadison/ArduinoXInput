# Using the XInputUSB API

The Arduino XInput implementation is broken up into three separate components, of which this library is just one part:

1. **The Library**, which is user-facing and contains lots of syntactic sugar to make it easy to manipulate the USB data.
2. **The Backend**, which handles the USB communication itself including endpoint memory management.
3. **The USB API**, which provides a standardized interface between these two components.

The goal of this document is to describe how the USB API works so that you can use it to build XInput support for other board types while maintaining compatibility with this library.

---
	
## Overview

Here is the C++ API header in all of its glory:

```cpp
#define USB_XINPUT

class XInputUSB {
public:
	static boolean connected(void);
	static uint8_t available(void);
	static int send(const void *buffer, uint8_t nbytes);
	static int recv(void *buffer, uint8_t nbytes);
	static void setRecvCallback(void(*callback)(void));
};
```

Note the `USB_XINPUT` preprocessor definition. This definition is what tells the library that the API is present and that it can send data over USB.

## Functions

The API consists of five core static functions of the `XInputUSB` class that pass the USB data back and forth from the bus to the library. Here is the breakdown:

### boolean connected(void)

The `connected` function returns the state of the USB interface with respect to XInput. If the device is connected to a host and functional this should return `true`. If the device is disconnected this should return `false`.

### uint8_t available(void)

The `available` function returns the number of bytes currently stored in the USB data banks for the received control surface data. If there are no bytes available this function returns 0.

### int send(const void *buffer, uint8_t nbytes)

The `send` function is used to send control surface data to the host over USB. It takes two arguments:
1. A pointer to the start of an array of data to be sent
2. The number of bytes to be sent

The function then blocks until the data is sent, and returns the number of bytes that were transmitted. If an error occurs, the function returns -1.

### int recv(void *buffer, uint8_t nbytes)

The `recv` function is used to receive control surface data from the host. It takes two arguments:
1. A pointer to the start of a buffer to store the data
2. The number of bytes available in the buffer

The function should move any received data from the USB data banks into the user-provided buffer.

The function returns how many bytes were copied into the provided buffer. If no bytes are copied the function returns 0. If an error occurs the function should return -1.

### void setRecvCallback(void(*callback)(void))

The `setRecvCallback` function is used to set the function callback for received data. It takes one argument, a function pointer with no arguments and a 'void' return type. This function pointer should be invoked whenever a new control surface data packet has been received.

The API does not specify the storage for this callback pointer.

## Building a Board Implementation

These functions make up the API that allows the library to communicate over USB. However, the lower level USB implementation itself is up to the developer's discretion. To be detected by the Windows driver as an XInput device, your board needs to define the following, taken from an existing XInput product:

* VID / PID
* Device Descriptor
* Configuration Descriptor
* String Descriptors

This library is built around the official Xbox 360 wired controller's data layout. You can find these descriptors and an explanation of what they mean [here](http://www.partsnotincluded.com/reverse-engineering/understanding-the-xbox-360-wired-controllers-usb-data). If you're emulating another device the layout of the data packet may be different and need to be modified by your implementation.

Once the board is detected by the host as an XInput device, define the USB API per the information above. Then fire up the library and try it out!

## Examples

Here are my two API implementations for reference:

* [Arduino AVR Boards](https://github.com/dmadison/ArduinoXInput_AVR/blob/master/cores/arduino/xinput/USB_XInput_API.cpp)
* [Teensy 3 Boards](https://github.com/dmadison/ArduinoXInput_Teensy/blob/master/teensy/avr/cores/teensy3/usb_xinput.c)

I've also included a test sketch for validating an XInputUSB API implementation without the library, which you can find [here](API-Demo/API-Demo.ino).
