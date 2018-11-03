/*
 *  Project     Teensy XInput Library
 *  @author     David Madison
 *  @link       github.com/dmadison/TeensyXInput
 *  @license    MIT - Copyright (c) 2018 David Madison
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
 */

#ifndef XInput_h
#define XInput_h

#include "Arduino.h"

enum XInputControl {
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
	BUTTON_LOGO,
	DPAD_UP,
	DPAD_DOWN,
	DPAD_LEFT,
	DPAD_RIGHT,
	TRIGGER_LEFT,
	TRIGGER_RIGHT,
	JOY_LEFT,
	JOY_RIGHT,
};

enum class XInputLEDPattern {
	Off = 0x00,
	Blinking = 0x01,
	Flash1 = 0x02,
	Flash2 = 0x03,
	Flash3 = 0x04,
	Flash4 = 0x05,
	On1 = 0x06,
	On2 = 0x07,
	On3 = 0x08,
	On4 = 0x09,
	Rotating = 0x0A,
	BlinkOnce = 0x0B,
	BlinkSlow = 0x0C,
	Alternating = 0x0D,
};


class XInputGamepad {
public:
	XInputGamepad();

	// Control surfaces
	void press(XInputControl button);
	void release(XInputControl button);
	void setButton(XInputControl button, boolean state);

	void setDpad(XInputControl pad, boolean state);
	void setDpad(boolean up, boolean down, boolean left, boolean right);

	void setTrigger(XInputControl trigger, uint8_t val);

	void setJoystick(XInputControl joy, int16_t x, int16_t y);

	void releaseAll();

	// Received Data
	uint8_t getPlayer() const;  // Player # assigned to the controller (0 is unassigned)
	uint16_t getRumble() const;  // Rumble motors. MSB is large weight, LSB is small
	uint8_t getRumbleLeft() const;  // Large rumble motor, left grip
	uint8_t getRumbleRight() const; // Small rumble motor, right grip
	
	uint8_t getLEDPatternID() const;  // Returns LED pattern ID #
	XInputLEDPattern getLEDPattern() const;  // Returns LED pattern type

	// USB IO
	void send();
	void receive();

	// Other
	void reset();

private:
	static const uint32_t USB_Timeout = 12840;  // Packet timeout, in milliseconds
	uint8_t tx[20];  // USB transmit data

	uint8_t player;  // Gamepad player #, buffered
	uint8_t rumble[2];  // Rumble motor data in, buffered
	XInputLEDPattern ledPattern;  // LED pattern data in, buffered

	void parseLED(uint8_t leds);  // Parse LED data and set pattern/player data
};

extern XInputGamepad XInput;

#endif