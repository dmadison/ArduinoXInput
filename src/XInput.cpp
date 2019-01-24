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

// Teensy 3.1-3.2:  __MK20DX256__
// Teensy LC:       __MKL26Z64__
// Teensy 3.5:      __MK64FX512__
// Teensy 3.6:      __MK66FX1M0__

#if defined(TEENSYDUINO) && \
	(defined(__MK20DX256__) || defined(__MKL26Z64__) || \
	 defined(__MK64FX512__) || defined(__MK66FX1M0__))

	#ifndef USB_XINPUT
	#error "USB type is not set to XInput in boards menu!"
	#endif

#else
#error "Not a supported board! Must use Teensy 3.1/3.2, LC, 3.5, or 3.6"
#endif

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
static const XInputMap_Button Map_ButtonLogo(3, 2);
static const XInputMap_Button Map_ButtonA(3, 4);
static const XInputMap_Button Map_ButtonB(3, 5);
static const XInputMap_Button Map_ButtonX(3, 6);
static const XInputMap_Button Map_ButtonY(3, 7);

const XInputMap_Button * getButtonFromEnum(XInputControl ctrl) {
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
	case(JOY_LEFT):
	case(BUTTON_L3):    return &Map_ButtonL3;
	case(JOY_RIGHT):
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
	static const XInputGamepad::Range range;
	const uint8_t index;
};

const XInputGamepad::Range XInputMap_Trigger::range = { 0, 255 };  // uint8_t

static const XInputMap_Trigger Map_TriggerLeft(4);
static const XInputMap_Trigger Map_TriggerRight(5);

const XInputMap_Trigger * getTriggerFromEnum(XInputControl ctrl) {
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
	static const XInputGamepad::Range range;
	const uint8_t x_low;
	const uint8_t x_high;
	const uint8_t y_low;
	const uint8_t y_high;
};

const XInputGamepad::Range XInputMap_Joystick::range = { -32768, 32767 };  // int16_t

static const XInputMap_Joystick Map_JoystickLeft(6, 7, 8, 9);
static const XInputMap_Joystick Map_JoystickRight(10, 11, 12, 13);

const XInputMap_Joystick * getJoyFromEnum(XInputControl ctrl) {
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
	reset();
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
		if (getButton(button) == state) return;  // Button hasn't changed

		if (state) { tx[buttonData->index] |= buttonData->mask; }  // Press
		else { tx[buttonData->index] &= ~(buttonData->mask); }  // Release
		newData = true;
	}
	else {
		Range * triggerRange = getRangeFromEnum(button);
		if (triggerRange == nullptr) return;  // Not a trigger (or joystick, but the trigger function will ignore that)
		setTrigger(button, state ? triggerRange->max : triggerRange->min);  // Treat trigger like a button
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

void XInputGamepad::setTrigger(XInputControl trigger, int32_t val) {
	const XInputMap_Trigger * triggerData = getTriggerFromEnum(trigger);
	if (triggerData == nullptr) return;  // Not a trigger

	val = rescaleInput(val, *getRangeFromEnum(trigger), triggerData->range);
	if (getTrigger(trigger) == val) return;  // Trigger hasn't changed

	tx[triggerData->index] = val;
	newData = true;
}

void XInputGamepad::setJoystick(XInputControl joy, int32_t x, int32_t y) {
	const XInputMap_Joystick * joyData = getJoyFromEnum(joy);
	if (joyData == nullptr) return;  // Not a joystick

	x = rescaleInput(x, *getRangeFromEnum(joy), joyData->range);
	y = rescaleInput(y, *getRangeFromEnum(joy), joyData->range);

	if (getJoystickX(joy) == x && getJoystickY(joy) == y) return;  // Joy hasn't changed

	tx[joyData->x_low]  = lowByte(x);
	tx[joyData->x_high] = highByte(x);

	tx[joyData->y_low]  = lowByte(y);
	tx[joyData->y_high] = highByte(y);

	newData = true;
}

void XInputGamepad::releaseAll() {
	const uint8_t offset = 2;  // Skip message type and packet size
	memset(tx + offset, 0x00, sizeof(tx) - offset);  // Clear TX array
	newData = true;  // Data changed and is unsent
}

boolean XInputGamepad::getButton(XInputControl button) const {
	const XInputMap_Button * buttonData = getButtonFromEnum(button);
	if (buttonData == nullptr) return 0;  // Not a button
	return tx[buttonData->index] & buttonData->mask;
}

boolean XInputGamepad::getDpad(XInputControl dpad) const {
	return getButton(dpad);
}

uint8_t XInputGamepad::getTrigger(XInputControl trigger) const {
	const XInputMap_Trigger * triggerData = getTriggerFromEnum(trigger);
	if (triggerData == nullptr) return 0;  // Not a trigger
	return tx[triggerData->index];
}

int16_t XInputGamepad::getJoystickX(XInputControl joy) const {
	const XInputMap_Joystick * joyData = getJoyFromEnum(joy);
	if (joyData == nullptr) return 0;  // Not a joystick
	return (tx[joyData->x_high] << 8) | tx[joyData->x_low];
}

int16_t XInputGamepad::getJoystickY(XInputControl joy) const {
	const XInputMap_Joystick * joyData = getJoyFromEnum(joy);
	if (joyData == nullptr) return 0;  // Not a joystick
	return (tx[joyData->y_high] << 8) | tx[joyData->y_low];
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
	if (!newData) return;  // TX data hasn't changed
	XInputUSB.send(tx, USB_Timeout);
	newData = false;
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
	switch (ledPattern) {
	case(XInputLEDPattern::Off):
	case(XInputLEDPattern::Blinking):
		player = 0;  // Not connected
		break;
	case(XInputLEDPattern::On1):
	case(XInputLEDPattern::Flash1):
		player = 1;
		break;
	case(XInputLEDPattern::On2):
	case(XInputLEDPattern::Flash2):
		player = 2;
		break;
	case(XInputLEDPattern::On3):
	case(XInputLEDPattern::Flash3):
		player = 3;
		break;
	case(XInputLEDPattern::On4):
	case(XInputLEDPattern::Flash4):
		player = 4;
		break;
	default: return;  // Pattern doesn't affect player #
	}
}

XInputGamepad::Range * XInputGamepad::getRangeFromEnum(XInputControl ctrl) {
	switch (ctrl) {
	case(TRIGGER_LEFT): return &rangeTrigLeft;
	case(TRIGGER_RIGHT): return &rangeTrigRight;
	case(JOY_LEFT): return &rangeJoyLeft;
	case(JOY_RIGHT): return &rangeJoyRight;
	default: return nullptr;
	}
}

int32_t XInputGamepad::rescaleInput(int32_t val, Range in, Range out) {
	if (in.min == out.min && in.max == out.max) return val;  // Ranges identical
	if (val <= in.min) return out.min;  // Out of range -
	if (val >= in.max) return out.max;  // Out of range +
	return map(val, in.min, in.max, out.min, out.max);
}

void XInputGamepad::setTriggerRange(int32_t rangeMin, int32_t rangeMax) {
	setRange(TRIGGER_LEFT, rangeMin, rangeMax);
	setRange(TRIGGER_RIGHT, rangeMin, rangeMax);
}

void XInputGamepad::setJoystickRange(int32_t rangeMin, int32_t rangeMax) {
	setRange(JOY_LEFT, rangeMin, rangeMax);
	setRange(JOY_RIGHT, rangeMin, rangeMax);
}

void XInputGamepad::setRange(XInputControl ctrl, int32_t rangeMin, int32_t rangeMax) {
	if (rangeMin >= rangeMax) return;  // Error: Max < Min

	Range * range = getRangeFromEnum(ctrl);
	if (range == nullptr) return;  // Not an addressable range

	range->min = rangeMin;
	range->max = rangeMax;
}

// Resets class back to initial values
void XInputGamepad::reset() {
	// Reset control data (tx)
	releaseAll();  // Clear TX buffer
	tx[0] = 0x00;  // Set tx message type
	tx[1] = 0x14;  // Set tx packet size (20)

	// Reset received data (rx)
	player = 0;  // Not connected, no player
	memset(rumble, 0x00, sizeof(rumble));  // Clear rumble values
	ledPattern = XInputLEDPattern::Off;  // No LEDs on

	// Reset rescale ranges
	setTriggerRange(XInputMap_Trigger::range.min, XInputMap_Trigger::range.max);
	setJoystickRange(XInputMap_Joystick::range.min, XInputMap_Joystick::range.max);
}

XInputGamepad XInput;