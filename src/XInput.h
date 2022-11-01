#include <XInput.h>

const int StartButtonPin = 0;
const int SelectButtonPin = 2;

const int AButtonPin = 3;
const int XButtonPin = 4;
const int YButtonPin = 5;
const int BButtonPin = 6;

const int RBButtonPin = 7;
const int LBButtonPin = 8;

const int LeftJoyXPin = A3;
const int LeftJoyYPin = A4;
//const int LeftJoyPushPin = A5;

const int RightJoyXPin = A0;
const int RightJoyYPin = A1;
const int led1 = 9;
const int led2 = 10;
const int led3 = 11;
const int led4 = 12;
const int led5 = 13;
const int led6 = 1;
const int led7 = 17;
const int led8 = 16;

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);
  pinMode(led7, OUTPUT);
  pinMode(led8, OUTPUT);
  
  pinMode(StartButtonPin, INPUT_PULLUP);
  pinMode(SelectButtonPin, INPUT_PULLUP);

  pinMode(AButtonPin, INPUT_PULLUP);
  pinMode(XButtonPin, INPUT_PULLUP);
  pinMode(YButtonPin, INPUT_PULLUP);
  pinMode(BButtonPin, INPUT_PULLUP);

  pinMode(RBButtonPin, INPUT_PULLUP);
  pinMode(LBButtonPin, INPUT_PULLUP);

  XInput.setRange(JOY_LEFT, 0, 1023);  // using analogRead (10-bit ADC)
  XInput.setRange(JOY_RIGHT, 0, 1023);  // using analogRead (10-bit ADC)
  //pinMode(LeftJoyPushPin, INPUT_PULLUP);
	XInput.begin();
}

void loop() {
  boolean StartState = digitalRead(StartButtonPin);
	XInput.setButton(BUTTON_START, StartState);   // press Start conditionally
  digitalWrite(led1, StartState);

  boolean SelectState = digitalRead(SelectButtonPin);
	XInput.setButton(BUTTON_BACK, SelectState);   // press Select conditionally
  digitalWrite(led2, SelectState);

	boolean AState = digitalRead(AButtonPin);
	XInput.setButton(BUTTON_A, AState);   // press A conditionally
  digitalWrite(led3, AState);

  boolean XState = digitalRead(XButtonPin);
	XInput.setButton(BUTTON_X, XState);   // press X conditionally
  digitalWrite(led4, XState);

  boolean YState = digitalRead(YButtonPin);
	XInput.setButton(BUTTON_Y, YState);   // press Y conditionally
  digitalWrite(led5, YState);

  boolean BState = digitalRead(BButtonPin);
	XInput.setButton(BUTTON_B, BState);   // press B conditionally
  digitalWrite(led6, BState);

  boolean RBState = digitalRead(RBButtonPin);
	XInput.setButton(BUTTON_RB, RBState);   // press RB conditionally
  digitalWrite(led7, RBState);

  boolean LBState = digitalRead(LBButtonPin);
	XInput.setButton(BUTTON_LB, LBState);   // press LB conditionally
  digitalWrite(led8, LBState);

  int LeftJoyXState = analogRead(LeftJoyXPin);
  int LeftJoyYState = analogRead(LeftJoyYPin);
  XInput.setJoystickX(JOY_LEFT, LeftJoyXState, false);
  XInput.setJoystickY(JOY_LEFT, LeftJoyYState, true); //Y axis needed to be inverted for correct positioning

  // boolean LeftJoyPushState = !digitalRead(LeftJoyPushPin);
  // XInput.setButton(BUTTON_L3, LeftJoyPushState);   // press L3 Right conditionally

  int RightJoyXState = analogRead(RightJoyXPin);
  int RightJoyYState = analogRead(RightJoyYPin);
  XInput.setJoystickX(JOY_RIGHT, RightJoyXState, false);
  XInput.setJoystickY(JOY_RIGHT, RightJoyYState, true); //Y axis needed to be inverted for correct positioning
}
