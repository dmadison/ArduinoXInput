# Arduino XInput Library

This library lets you easily emulate an Xbox 360 controller using a USB-capable Arduino microcontroller.

## Getting Started

```cpp
void setup() {
	XInput.begin();
}

void loop() {
	XInput.press(BUTTON_A);
	delay(1000);
	
	XInput.release(BUTTON_A);
	delay(1000);
}
```

Before the library will work, you must install a compatible boards file that contains the XInput USB descriptors, otherwise the microcontroller won't behave like an XInput device. **This is not optional**. See the [compatible boards](#compatible-boards) section below for more information.

After installing a compatible boards package, you must then [download and install the library](https://www.arduino.cc/en/guide/libraries). Once the XInput library is installed, open up the Arduino IDE and load an example sketch, located in `File -> Examples -> XInput` (I suggest trying the 'Blink' sketch first). Double-check that you have the correct XInput board selected in the 'Tools' menu, then upload the sketch to your microcontroller.

On Windows, you can test that the sketch is working properly by opening up the joystick control panel ([joy.cpl](https://support.microsoft.com/en-us/help/831361/how-to-troubleshoot-game-controllers-in-microsoft-games)) or by using [GamepadViewer.com](https://gamepadviewer.com/?p=1). If you uploaded the XInput 'Blink' example, the #1 button ('A') should be slowly turning on and off.

## Control Surfaces

The library gives you access to the following controls available on the Xbox 360 controller:
* 10 + 1 Digital Buttons
* 2 Analog Joysticks (16 bit)
* 2 Analog Triggers (8 bit)
* 1 Four-Way Directional Pad (D-Pad)

The library also processes received data, so you can read the status of the controller's 2 rumble motors (8-bit), the assigned player number (1-4), and the index of the current LED animation. Data is sent and received automatically over USB.

## Compatible Boards

To function as an XInput device, you *must* use a compatible boards package with XInput USB descriptors. **This is not optional**. Without these descriptors the library will only function in "debug" mode and the microcontroller will not behave as an XInput device.

The following boards packages are available:

* #### [Arduino AVR Boards](https://www.github.com/dmadison/ArduinoXInput_AVR)
  Modifies the Arduino AVR core to emulate an XInput device. Includes support for the Arduino Leonardo, Micro, Yun, and more. 

* #### [Teensy 3 Boards](https://www.github.com/dmadison/ArduinoXInput_Teensy)
  Includes an 'XInput' USB mode for the Teensy 3.1, 3.2, 3.5, 3.6, and LC microcontrollers. Requires a preexisting Teensyduino installation.

## License

This library is licensed under the terms of the [MIT license](https://opensource.org/licenses/MIT). See the [LICENSE](LICENSE) file for more information.
