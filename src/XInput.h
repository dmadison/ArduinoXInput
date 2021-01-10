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
 */

#ifndef XInput_h
#define XInput_h

#include <Arduino.h>

enum XInputControl : uint8_t {
	BUTTON_LOGO = 0,
	BUTTON_A = 1,
	BUTTON_B = 2,
	BUTTON_X = 3,
	BUTTON_Y = 4,
	BUTTON_LB = 5,
	BUTTON_RB = 6,
	BUTTON_BACK = 7,
	BUTTON_START = 8,
	BUTTON_L3 = 9,
	BUTTON_R3 = 10,
	DPAD_UP = 11,
	DPAD_DOWN = 12,
	DPAD_LEFT = 13,
	DPAD_RIGHT = 14,
	TRIGGER_LEFT = 15,
	TRIGGER_RIGHT = 16,
	JOY_LEFT,
	JOY_RIGHT,
};

enum class XInputReceiveType : uint8_t {
	Rumble = 0x00,
	LEDs = 0x01,
};

enum class XInputLEDPattern : uint8_t {
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


class XInputController {
public:
	XInputController();

	void begin();

	// Set Control Surfaces
	void press(uint8_t button);
	void release(uint8_t button);
	void setButton(uint8_t button, boolean state);

	void setDpad(XInputControl pad, boolean state);
	void setDpad(boolean up, boolean down, boolean left, boolean right, boolean useSOCD=true);

	void setTrigger(XInputControl trigger, int32_t val);

	void setJoystick(XInputControl joy, int32_t x, int32_t y);
	void setJoystick(XInputControl joy, boolean up, boolean down, boolean left, boolean right, boolean useSOCD=true);
	void setJoystickX(XInputControl joy, int32_t x, boolean invert=false);
	void setJoystickY(XInputControl joy, int32_t y, boolean invert=false);

	void releaseAll();

	// Auto-Send Data
	void setAutoSend(boolean a);

	// Read Control Surfaces
	boolean getButton(uint8_t button) const;
	boolean getDpad(XInputControl dpad) const;
	uint8_t getTrigger(XInputControl trigger) const;
	int16_t getJoystickX(XInputControl joy) const;
	int16_t getJoystickY(XInputControl joy) const;

	// Received Data
	uint8_t getPlayer() const;  // Player # assigned to the controller (0 is unassigned)

	uint16_t getRumble() const;  // Rumble motors. MSB is large weight, LSB is small
	uint8_t  getRumbleLeft() const;  // Large rumble motor, left grip
	uint8_t  getRumbleRight() const; // Small rumble motor, right grip

	XInputLEDPattern getLEDPattern() const;  // Returns LED pattern type

	// Received Data Callback
	using RecvCallbackType = void(*)(uint8_t packetType);
	void setReceiveCallback(RecvCallbackType);

	// USB IO
	boolean connected();
	int send();
	int receive();

	// Control Input Ranges
	struct Range { int32_t min; int32_t max; };

	void setTriggerRange(int32_t rangeMin, int32_t rangeMax);
	void setJoystickRange(int32_t rangeMin, int32_t rangeMax);
	void setRange(XInputControl ctrl, int32_t rangeMin, int32_t rangeMax);

	// Setup
	void reset();

	// Debug
	void printDebug(Print& output=Serial) const;

private:
	// Sent Data
	uint8_t tx[20];  // USB transmit data
	boolean newData;  // Flag for tx data changed
	boolean autoSendOption;  // Flag for automatically sending data
	
	void setJoystickDirect(XInputControl joy, int16_t x, int16_t y);

	void inline autosend() {
		if (autoSendOption) { send(); }
	}

	// Received Data
	volatile uint8_t player;  // Gamepad player #, buffered
	volatile uint8_t rumble[2];  // Rumble motor data in, buffered
	volatile XInputLEDPattern ledPattern;  // LED pattern data in, buffered
	RecvCallbackType recvCallback;  // User-set callback for received data

	void parseLED(uint8_t leds);  // Parse LED data and set pattern/player data

	// Control Input Ranges
	Range rangeTrigLeft, rangeTrigRight, rangeJoyLeft, rangeJoyRight;
	Range * getRangeFromEnum(XInputControl ctrl);
	static int32_t rescaleInput(int32_t val, const Range& in, const Range &out);
	static int16_t invertInput(int16_t val, const Range& range);
};

extern XInputController XInput;

#endif