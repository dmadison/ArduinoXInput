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

#include "XInput.h"

// --------------------------------------------------------
// XInput Button Maps                                     |
// (Matches ID to tx index with bitmask)                  |
// --------------------------------------------------------

struct XInputMap_Button {
	constexpr XInputMap_Button(uint8_t i, uint8_t o)
		: index(i), mask(BuildMask(o)) {}
	const uint8_t index;
	const uint8_t mask;

private:
	constexpr static uint8_t BuildMask(uint8_t offset) {
		return (1 << offset);  // Bitmask of bit to flip
	}
};

static const XInputMap_Button Map_DpadUp(2, 0);
static const XInputMap_Button Map_DpadDown(2, 1);
static const XInputMap_Button Map_DpadLeft(2, 2);
static const XInputMap_Button Map_DpadRight(2, 3);
static const XInputMap_Button Map_ButtonStart(2, 4);
static const XInputMap_Button Map_ButtonBack(2, 5);
static const XInputMap_Button Map_ButtonL3(2, 6);
static const XInputMap_Button Map_ButtonR3(2, 7);

static const XInputMap_Button Map_ButtonLB(3, 0);
static const XInputMap_Button Map_ButtonRB(3, 1);
static const XInputMap_Button Map_ButtonLogo(3, 3);
static const XInputMap_Button Map_ButtonA(3, 4);
static const XInputMap_Button Map_ButtonB(3, 5);
static const XInputMap_Button Map_ButtonX(3, 6);
static const XInputMap_Button Map_ButtonY(3, 7);

constexpr const XInputMap_Button * getButtonFromEnum(XInputControl ctrl) {
	switch (ctrl) {
	case(DPAD_UP):      return &Map_DpadUp;
	case(DPAD_DOWN):    return &Map_DpadDown;
	case(DPAD_LEFT):    return &Map_DpadLeft;
	case(DPAD_RIGHT):   return &Map_DpadRight;
	case(BUTTON_A):     return &Map_ButtonA;
	case(BUTTON_B):     return &Map_ButtonB;
	case(BUTTON_X):     return &Map_ButtonX;
	case(BUTTON_Y):     return &Map_ButtonY;
	case(BUTTON_LB):    return &Map_ButtonLB;
	case(BUTTON_RB):    return &Map_ButtonRB;
	case(BUTTON_L3):    return &Map_ButtonL3;
	case(BUTTON_R3):    return &Map_ButtonR3;
	case(BUTTON_START): return &Map_ButtonStart;
	case(BUTTON_BACK):  return &Map_ButtonBack;
	case(BUTTON_LOGO):  return &Map_ButtonLogo;
	default: return nullptr;
	}
}

// --------------------------------------------------------
// XInput Trigger Maps                                    |
// (Matches ID to tx index)                               |
// --------------------------------------------------------

struct XInputMap_Trigger {
	constexpr XInputMap_Trigger(uint8_t i)
		: index(i) {}
	const uint8_t index;
};

static const XInputMap_Trigger Map_TriggerLeft(4);
static const XInputMap_Trigger Map_TriggerRight(5);

constexpr const XInputMap_Trigger * getTriggerFromEnum(XInputControl ctrl) {
	switch (ctrl) {
	case(TRIGGER_LEFT): return &Map_TriggerLeft;
	case(TRIGGER_RIGHT): return &Map_TriggerRight;
	default: return nullptr;
	}
}

// --------------------------------------------------------
// XInput Joystick Maps                                   |
// (Matches ID to tx x/y high/low indices)                |
// --------------------------------------------------------

struct XInputMap_Joystick {
	constexpr XInputMap_Joystick(uint8_t xl, uint8_t xh, uint8_t yl, uint8_t yh)
		: x_low(xl), x_high(xh), y_low(yl), y_high(yh) {}
	const uint8_t x_low;
	const uint8_t x_high;
	const uint8_t y_low;
	const uint8_t y_high;
};

static const XInputMap_Joystick Map_JoystickLeft(6, 7, 8, 9);
static const XInputMap_Joystick Map_JoystickRight(10, 11, 12, 13);

constexpr const XInputMap_Joystick * getJoyFromEnum(XInputControl ctrl) {
	switch (ctrl) {
	case(JOY_LEFT): return &Map_JoystickLeft;
	case(JOY_RIGHT): return &Map_JoystickRight;
	default: return nullptr;
	}
}

// --------------------------------------------------------
// XInput Rumble Maps                                     |
// (Stores rx index and buffer index for each motor)      |
// --------------------------------------------------------

struct XInputMap_Rumble {
	constexpr XInputMap_Rumble(uint8_t rIndex, uint8_t bIndex)
		: rxIndex(rIndex), bufferIndex(bIndex) {}
	const uint8_t rxIndex;
	const uint8_t bufferIndex;
};

static const XInputMap_Rumble RumbleLeft(3, 0);   // Large motor
static const XInputMap_Rumble RumbleRight(4, 1);  // Small motor


// --------------------------------------------------------
// XInputGamepad Class (API)                              |
// --------------------------------------------------------

XInputGamepad::XInputGamepad() :
	tx(), rumble() // Zero initialize arrays
{
	tx[0] = 0x00;  // Message type
	tx[1] = 0x14;  // Packet size
}

void XInputGamepad::press(XInputControl button) {
	setButton(button, true);
}

void XInputGamepad::release(XInputControl button) {
	setButton(button, false);
}

void XInputGamepad::setButton(XInputControl button, boolean state) {
	const XInputMap_Button * buttonData = getButtonFromEnum(button);
	if (buttonData != nullptr) {
		if (state) { tx[buttonData->index] |= buttonData->mask; }  // Press
		else { tx[buttonData->index] &= ~(buttonData->mask); }  // Release
	}
	else {
		setTrigger(button, state ? 255 : 0);  // Treat trigger like a button
	}
}

void XInputGamepad::setDpad(XInputControl pad, boolean state) {
	setButton(pad, state);
}

void XInputGamepad::setDpad(boolean up, boolean down, boolean left, boolean right) {
	// Simultaneous Opposite Cardinal Directions (SOCD) Cleaner
	if (up && down) { down = false; }  // Up + Down = Up
	if (left && right) { left = false; right = false; }  // Left + Right = Neutral

	setDpad(DPAD_UP, up);
	setDpad(DPAD_DOWN, down);
	setDpad(DPAD_LEFT, left);
	setDpad(DPAD_RIGHT, right);
}

void XInputGamepad::setTrigger(XInputControl trigger, uint8_t val) {
	const XInputMap_Trigger * triggerData = getTriggerFromEnum(trigger);
	if (triggerData == nullptr) return;  // Not a trigger
	tx[triggerData->index] = val;
}

void XInputGamepad::setJoystick(XInputControl joy, int16_t x, int16_t y) {
	const XInputMap_Joystick * joyData = getJoyFromEnum(joy);
	if (joyData == nullptr) return;  // Not a joystick

	tx[joyData->x_low]  = lowByte(x);
	tx[joyData->x_high] = highByte(x);

	tx[joyData->y_low]  = lowByte(y);
	tx[joyData->y_high] = highByte(y);
}

void XInputGamepad::releaseAll() {
	const uint8_t offset = 2;  // Skip message type and packet size
	memset(tx + offset, 0x00, sizeof(tx) - offset);  // Clear TX array
}

uint8_t XInputGamepad::getPlayer() const {
	return player;
}

uint16_t XInputGamepad::getRumble() const {
	return rumble[RumbleLeft.bufferIndex] << 8 | rumble[RumbleRight.bufferIndex];
}

uint8_t XInputGamepad::getRumbleLeft() const {
	return rumble[RumbleLeft.bufferIndex];
}

uint8_t XInputGamepad::getRumbleRight() const {
	return rumble[RumbleRight.bufferIndex];
}

XInputLEDPattern XInputGamepad::getLEDPattern() const {
	return ledPattern;
}

uint8_t XInputGamepad::getLEDPatternID() const {
	return (uint8_t)ledPattern;
}

//Send an update packet to the PC
void XInputGamepad::send() {
	XInputUSB.send(tx, USB_Timeout);
}

void XInputGamepad::receive() {
	if (XInputUSB.available() == 0) {
		return;  // No packet available
	}

	// Grab packet and store it in rx array
	uint8_t rx[8];
	XInputUSB.recv(rx, USB_Timeout);
	
	// Rumble Packet
	if ((rx[0] == 0x00) & (rx[1] == 0x08)) {
		rumble[RumbleLeft.bufferIndex]  = rx[RumbleLeft.rxIndex];   // Big weight (Left grip)
		rumble[RumbleRight.bufferIndex] = rx[RumbleRight.rxIndex];  // Small weight (Right grip)
	}
	// LED Packet
	else if (rx[0] == 0x01) {
		parseLED(rx[2]);
	}
}

void XInputGamepad::parseLED(uint8_t leds) {
	if (leds > 0x0D) return;  // Not a known pattern

	ledPattern = (XInputLEDPattern) leds;  // Save pattern
	if (ledPattern == XInputLEDPattern::Off || ledPattern == XInputLEDPattern::Blinking) {
		player = 0;  // Not connected
	}
	else if (ledPattern == XInputLEDPattern::On1 || ledPattern == XInputLEDPattern::Flash1) {
		player = 1;
	}
	else if (ledPattern == XInputLEDPattern::On2 || ledPattern == XInputLEDPattern::Flash2) {
		player = 2;
	}
	else if (ledPattern == XInputLEDPattern::On3 || ledPattern == XInputLEDPattern::Flash3) {
		player = 3;
	}
	else if (ledPattern == XInputLEDPattern::On4 || ledPattern == XInputLEDPattern::Flash4) {
		player = 4;
	}
	else {
		return;  // Pattern doesn't affect player #
	}
}

XInputGamepad XInput;
