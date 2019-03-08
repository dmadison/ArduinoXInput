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
 *  Example:      SimulateAll
 *  Description:  Automatically activate all possible XInput controls.
 *                Useful to test that everything is functioning properly.
 * 
 *  WARNING: This will spam inputs! Ground pin '0' (RX) to stop.
 *
 */

#include <XInput.h>

// Config Settings
const unsigned long CycleTime = 5000;  // ms
const int SafetyPin = 0;  // Ground this pin to prevent inputs

// Button Setup
const int NumButtons = 10;
const int Buttons[NumButtons] = {
	BUTTON_A,
	BUTTON_B,
	BUTTON_X,
	BUTTON_Y,
	BUTTON_LB,
	BUTTON_RB,
	BUTTON_BACK,
	BUTTON_START,
	BUTTON_L3,
	BUTTON_R3,
};
const unsigned long ButtonDuration = CycleTime / NumButtons;
unsigned long buttonTimeLast = 0;
int buttonID = 0;

// DPad Setup
const int NumDirections = 4;
const unsigned long DPadDuration = CycleTime / NumDirections;
unsigned long dpadTimeLast = 0;
int dpadPosition = 0;

// Triggers
const int TriggerMax = 255;  // uint8_t max
const unsigned long TriggerDuration = CycleTime / (TriggerMax * 2);  // Go up and down
unsigned long triggerTimeLast = 0;
uint8_t triggerVal = 0;
boolean triggerDirection = 0;

// Joystick Setup
const int JoyMax = 32767;  // int16_t max
const double angle_precision = (2 * PI) / (CycleTime / 4);  // 4 because 250 Hz update rate
double angle = 0.0;

void setup() {
	pinMode(SafetyPin, INPUT_PULLUP);
	XInput.setAutoSend(false);  // Wait for all controls before sending

	XInput.begin();
}

void loop() {
	if (digitalRead(SafetyPin) == LOW) {
		return;
	}

	unsigned long t = millis();  // Get timestamp for comparison

	// DPad
	if (t - dpadTimeLast >= DPadDuration) {  // If enough time has passed, change the dpad
		XInput.setDpad(dpadPosition == 0, dpadPosition == 1, dpadPosition == 2, dpadPosition == 3);

		dpadPosition++;  // Increment the dpad counter
		if (dpadPosition >= NumDirections) dpadPosition = 0;  // Go back to 0 if we hit the limit
		dpadTimeLast = t;  // Save time we last did this
	}

	// Buttons
	if (t - buttonTimeLast >= ButtonDuration) {  // If enough time has passed, change the button pressed
		for (int i = 0; i < NumButtons; i++) {
			XInput.release((XInputControl)Buttons[i]);  // Relase all buttons
		}
		
		XInput.press((XInputControl)Buttons[buttonID]); // Press the next button
		buttonID++;  // Increment the button counter
		if (buttonID >= NumButtons) buttonID = 0;  // Go back to 0 if we hit the limit

		buttonTimeLast = t;  // Save time we last did this
	}

	// Triggers
	if (t - triggerTimeLast >= TriggerDuration) {  // If enough time has passed, update the triggers
		XInput.setTrigger(TRIGGER_LEFT, triggerVal);
		XInput.setTrigger(TRIGGER_RIGHT, TriggerMax - triggerVal);  // Inverse

		// Increment trigger value based on direction
		if (triggerDirection == 0) { triggerVal++; }
		else { triggerVal--; }

		if (triggerVal == TriggerMax || triggerVal == 0) {  // If we've hit the edge of the range
			triggerDirection = !triggerDirection;  // Reverse direction
		}

		triggerTimeLast = t;  // Save time we last did this
	}

	// Calculate joystick x/y values using trig
	int axis_x = sin(angle) * JoyMax;
	int axis_y = cos(angle) * JoyMax;

	angle += angle_precision;
	if (angle >= 360) {
		angle -= 360;
	}

	XInput.setJoystick(JOY_LEFT, axis_x, axis_y);  // Clockwise
	XInput.setJoystick(JOY_RIGHT, -axis_x, axis_y);  // Counter-clockwise

	// Send values to PC
	XInput.send();
}
