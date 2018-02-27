/* Copyright 2018 Stefan Daniel Homfeld, www.stedaho.de */

const int pinButtonStart = 2;
const int pinLedStart = 5;
const int pinButtonStop = 3;
const int pinLedStop = 6;
const int pinTransmitter = 4;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Starting up...");

  pinMode(pinLedStart, OUTPUT);
  pinMode(pinLedStop, OUTPUT);
  pinMode(pinTransmitter, OUTPUT);
  pinMode(pinButtonStart, INPUT_PULLUP);
  pinMode(pinButtonStop, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinButtonStart), buttonPressedStart, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinButtonStop), buttonPressedStop, FALLING);

  Serial.println("Board started");
}

void loop() {

}


void buttonPressedStart() {
}

void buttonPressedStop() {
}
