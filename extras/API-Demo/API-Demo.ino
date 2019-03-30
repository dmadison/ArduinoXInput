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
 *                'A' button should toggle on and off every 2 seconds.
 */

#ifndef USB_XINPUT
#error "USB_XINPUT not defined! No XInput API detected"
#else

uint8_t txData[20] = { 0x00, 0x14, 0x00 };
boolean buttonState = false;

volatile uint8_t rxData[8]  = { 0x00 };
volatile uint8_t playerNum = 0;

void setup() {
	while (!XInputUSB::connected()) {}  // wait for connection
	XInputUSB::setRecvCallback(receiveCallback);
}

void loop() {
	// Set D-PAD based on player number (test receive)
	const uint8_t pNum = playerNum;
	if (pNum != 0) {
		setPlayerControl(pNum);
		playerNum = 0;
	}

	// Toggle button 'A' (test send)
	buttonState = !buttonState;
	setButtonA(buttonState);
	XInputUSB::send(txData, sizeof(txData));
	
	delay(1000);
}

void setButton(uint8_t index, uint8_t position, uint8_t state) {
	if (state == true) {
		txData[index] |= (1 << position);
	}
	else {
		txData[index] &= ~(1 << position);
	}
}

void setButtonA(boolean state) {
	setButton(3, 4, state);
}

void setPlayerControl(uint8_t num) {
	// Player 1: Dpad Up
	// Player 2; Dpad Down
	// Player 3: Dpad Left
	// Player 4: Dpad Right
	setButton(2, num - 1, true);
}

void receiveCallback() {
	if (XInputUSB::available() > 0) {
		const int success = XInputUSB::recv((uint8_t*) rxData, sizeof(rxData));

		if (success > 0 && rxData[0] == 0x01) {  // LED Packet
			uint8_t ledPattern = rxData[2];
			if (ledPattern >= 2 && ledPattern <= 9) {  // Player # Pattern
				playerNum = ((ledPattern - 2) % 4) + 1;  // Player # 1-4
			}
		}
	}
}

#endif
