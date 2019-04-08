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
 *  Example:      API Demo
 *  Description:  Tests for the functionality of the XInputUSB API
 *                without using the Arduino XInput library. Useful
 *                for testing boards packages without the library
 *                as a factor.
 *
 *                Tests the following XInputUSB functions:
 *                  * static boolean connected(void)
 *                  * static uint8_t available(void)
 *                  * static int send(const void *buffer, uint8_t nbytes)
 *                  * static int recv(void *buffer, uint8_t nbytes)
 *                  * static void setRecvCallback(void(*callback)(void))
 *
 *                If the example is running properly, the controller's
 *                'A' button should toggle on and off every 2 seconds
 *                and the built-in LED should turn on when rumble data
 *                is received.
 */

#ifndef USB_XINPUT
#error "USB_XINPUT not defined! No XInput API detected"
#else

uint8_t txData[20] = { 0x00, 0x14, 0x00 };
boolean buttonState = false;

void setup() {
	#ifdef LED_BUILTIN
		pinMode(LED_BUILTIN, OUTPUT);
		digitalWrite(LED_BUILTIN, LOW);
	#endif

	XInputUSB::setRecvCallback(receiveCallback);
	while (!XInputUSB::connected()) {}  // wait for connection
}

void loop() {
	buttonState = !buttonState;
	setButtonA(txData, buttonState);
	XInputUSB::send(txData, sizeof(txData));
	
	delay(1000);
}

void setButtonA(uint8_t * ptr, boolean state) {
	const uint8_t ButtonIndex = 3;
	const uint8_t ButtonPosition = 4;

	if (state == true) {
		ptr[ButtonIndex] |= (1 << ButtonPosition);
	}
	else {
		ptr[ButtonIndex] &= ~(1 << ButtonPosition);
	}
}

void receiveCallback() {
	if (XInputUSB::available() > 0) {
		uint8_t rxData[8] = { 0x00 };
		const int success = XInputUSB::recv(rxData, sizeof(rxData));

		if (success > 5 && rxData[0] == 0x00) {  // rumble packet
			#ifdef LED_BUILTIN
				boolean rumbling = (rxData[3] > 0 || rxData[4] > 0 );
				digitalWrite(LED_BUILTIN, rumbling);
			#endif
		}
	}
}

#endif
