/*
 *  Project     Arduino XInput Library
 *  @author     David Madison
 *  @link       github.com/dmadison/ArduinoXInput
 *  @license    MIT - Copyright (c) 2019 David Madison
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *  Example:      ReceiveCallback
 *  Description:  Shows how to set up a custom callback to process rumble or LED
 *                data when the device receives a new USB packet from the host.
 *
 *  WARNING: The callback is called from within the USB ISR. Keep it short!
 *
 */

#include <XInput.h>

const uint8_t LED_Pin = LED_BUILTIN;
const uint8_t Button_Pin = 6;

void setup() {
	pinMode(Button_Pin, INPUT_PULLUP);  // Set button pin as input w/ pullup

	pinMode(LED_Pin, OUTPUT);    // Set LED pin as output
	digitalWrite(LED_Pin, LOW);  // Turn LED off

	// Set callback function. Function must have a 'void' return type
	// and take a single uint8_t as an argument
	XInput.setReceiveCallback(rumbleCallback);

	XInput.begin();
}

void loop() {
	boolean buttonState = digitalRead(Button_Pin);
	
	if(buttonState == LOW) { 
		XInput.press(TRIGGER_RIGHT);
	}
	else {
		XInput.release(TRIGGER_RIGHT);
	}
}

void rumbleCallback(uint8_t packetType) {
	// If we have an LED packet (0x01), do nothing
	if (packetType == (uint8_t) XInputReceiveType::LEDs) {
		return;
	}

	// If we have a rumble packet (0x00), see our rumble data on the LED
	else if (packetType == (uint8_t) XInputReceiveType::Rumble) {
		uint8_t rumbleValue = XInput.getRumbleLeft() | XInput.getRumbleRight();
		analogWrite(LED_Pin, rumbleValue);
	}
}
