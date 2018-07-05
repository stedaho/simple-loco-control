/* Copyright 2018 Stefan Daniel Homfeld, www.stedaho.de */

#include <RFTransmitter.h> /* Siehe https://github.com/zeitgeist87/RFTransmitter */

/* Pin-Zuordnungen */
const byte pinButtonA = 2;
const byte pinLedA = 5;
const byte pinButtonB = 3;
const byte pinLedB = 6;
const byte pinTransmitter = 4;

/* Flags zur Abhandlung und dem Sperren der Tasterdrücke */
volatile bool buttonPressedFlagA = false;
volatile bool buttonPressedFlagB = false;
volatile bool buttonLockedFlagA = false;
volatile bool buttonLockedFlagB = false;

/* Zeitstempel zur Sperre der Tasterdrücke und den Blinker */
unsigned long timePressedA = 0;
unsigned long timePressedB = 0;
unsigned long timeLedBlinker = 0;

const long buttonDelay = 5000;
const long blinkerInterval = 500;

bool blinkerState = true;

const byte transmitterId = 28;
RFTransmitter transmitter(pinTransmitter, transmitterId);
const byte buttonPressedMessageA[5] = { 28, 1, 1, 51 };
const byte buttonPressedMessageB[5] = { 28, 2, 1, 51 };


void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Starting up...");

  pinMode(pinLedA, OUTPUT);
  pinMode(pinLedB, OUTPUT);
  pinMode(pinTransmitter, OUTPUT);
  pinMode(pinButtonA, INPUT_PULLUP);
  pinMode(pinButtonB, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(pinButtonA), buttonPressedA, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinButtonB), buttonPressedB, FALLING);

  digitalWrite(pinLedA, HIGH);
  digitalWrite(pinLedB, HIGH);
  digitalWrite(LED_BUILTIN, blinkerState);

  Serial.println("Board started");
}

void loop() {
  unsigned long currentMillis = millis();

  /* Pruefen, ob der A-Timer abgelaufen ist und ggf. den A-Button wieder freigeben */
  if (buttonLockedFlagA && !buttonPressedFlagA) {
    if (currentMillis - timePressedA > buttonDelay) {
      digitalWrite(pinLedA, HIGH);
      buttonLockedFlagA = false;
    }
  }
  /* A-Button wurde gedrueckt (und ist nicht gesperrt) */
  if (buttonLockedFlagA && buttonPressedFlagA) {
    timePressedA = currentMillis;
    digitalWrite(pinLedA, LOW);
    transmitter.send((byte *)buttonPressedMessageA, sizeof(buttonPressedMessageA));
    Serial.println("A gedrueckt");
    buttonPressedFlagA = false;
  }

  /* Pruefen, ob der B-Timer abgelaufen ist und ggf. den B-Button wieder freigeben */
  if (buttonLockedFlagB && !buttonPressedFlagB) {
    if (currentMillis - timePressedB > buttonDelay) {
      digitalWrite(pinLedB, HIGH);
      buttonLockedFlagB = false;
    }
  }
  /* B-Button wurde gedrueckt (und ist nicht gesperrt) */
  if (buttonLockedFlagB && buttonPressedFlagB) {
    timePressedB = currentMillis;
    digitalWrite(pinLedB, LOW);
    transmitter.send((byte *)buttonPressedMessageB, sizeof(buttonPressedMessageB));
    Serial.println("B gedrueckt");
    buttonPressedFlagB = false;
  }

  /* LED-Status-Blinker */
  if (currentMillis - timeLedBlinker >= blinkerInterval) {
    timeLedBlinker = currentMillis;
    blinkerState = !blinkerState;
    digitalWrite(LED_BUILTIN, blinkerState);
  }
}


void buttonPressedA() {
  if (!buttonLockedFlagA) {
    buttonLockedFlagA = true;
    buttonPressedFlagA = true;
  }
}

void buttonPressedB() {
  if (!buttonLockedFlagB) {
    buttonLockedFlagB = true;
    buttonPressedFlagB = true;
  }
}
