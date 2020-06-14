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
 *  Example:      WiiClassicController
 *  Description:  Maps the inputs from a Wii Classic Controller to an emulated
 *                Xbox 360 gamepad using XInput. Requires using the
 *                NintendoExtensionCtrl library and an extension adapter.
 */

#include <XInput.h>
#include <NintendoExtensionCtrl.h>  // Library for the Wii controller communication

ClassicController classic;

const int RangeOffset = 45;  // to make sure the values reach min/max outputs
const int RangeMin =   0 + RangeOffset;
const int RangeMax = 255 - RangeOffset;

void setup() {
	classic.begin();

	XInput.setTriggerRange(RangeMin, RangeMax);  // set both left and right

	XInput.setRange(JOY_LEFT,  RangeMin, RangeMax);
	XInput.setRange(JOY_RIGHT, RangeMin, RangeMax);

	XInput.setAutoSend(false);  // Wait for all controls before sending

	XInput.begin();

	while (!classic.connect()) {
		delay(1000);  // Controller not connected
	}
}

void loop() {
	if(classic.update()) {  // Get new data!
		XInput.setJoystick(JOY_LEFT,  classic.leftJoyX(), classic.leftJoyY());
		XInput.setJoystick(JOY_RIGHT, classic.rightJoyX(), classic.rightJoyY());

		XInput.setButton(BUTTON_A, classic.buttonB());
		XInput.setButton(BUTTON_B, classic.buttonA());
		XInput.setButton(BUTTON_X, classic.buttonY());
		XInput.setButton(BUTTON_Y, classic.buttonX());

		XInput.setButton(BUTTON_START, classic.buttonPlus());
		XInput.setButton(BUTTON_BACK,  classic.buttonMinus());
		XInput.setButton(BUTTON_LOGO,  classic.buttonHome());

		XInput.setDpad(classic.dpadUp(), classic.dpadDown(), classic.dpadLeft(), classic.dpadRight());

		XInput.setTrigger(TRIGGER_LEFT,  classic.triggerL());
		XInput.setTrigger(TRIGGER_RIGHT, classic.triggerR());

		XInput.setButton(BUTTON_LB, classic.buttonZL());
		XInput.setButton(BUTTON_RB, classic.buttonZR());

		// XInput.setButton(BUTTON_L3, classic.buttonZL());  // The Classic Controller doesn't have L3 and R3
		// XInput.setButton(BUTTON_R3, classic.buttonZR());  // but you can uncomment these to check that they work

		XInput.send();
	}
	else {  // Data is bad :(
		XInput.releaseAll();  // clear set inputs...
		XInput.send();  // ...and send that update

		while (!classic.connect()) {  // attempt to reconnect
			delay(1000);
		}
	}
}
