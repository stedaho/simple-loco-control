/* Copyright 2018 Stefan Daniel Homfeld, www.stedaho.de */

#include <RFTransmitter.h> /* Siehe https://github.com/zeitgeist87/RFTransmitter */

/* Pin-Zuordnungen */
const byte pinButtonStart = 2;
const byte pinLedStart = 5;
const byte pinButtonStop = 3;
const byte pinLedStop = 6;
const byte pinTransmitter = 4;

/* Flags zur Abhandlung und dem Sperren der Tasterdrücke */
volatile bool buttonPressedFlagStart = false;
volatile bool buttonPressedFlagStop = false;
volatile bool buttonLockedFlagStart = false;
volatile bool buttonLockedFlagStop = false;

/* Zeitstempel zur Sperre der Tasterdrücke und den Blinker */
unsigned long timePressedStart = 0;
unsigned long timePressedStop = 0;
unsigned long timeLedBlinker = 0;

const long buttonDelay = 5000;
const long blinkerInterval = 500;

bool blinkerState = true;

const byte transmitterId = 28;
RFTransmitter transmitter(pinTransmitter, transmitterId);
const byte buttonPressedMessageStart[5] = { 28, 1, 1, 51 };
const byte buttonPressedMessageStop[5] = { 28, 2, 1, 51 };


void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Starting up...");

  pinMode(pinLedStart, OUTPUT);
  pinMode(pinLedStop, OUTPUT);
  pinMode(pinTransmitter, OUTPUT);
  pinMode(pinButtonStart, INPUT_PULLUP);
  pinMode(pinButtonStop, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(pinButtonStart), buttonPressedStart, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinButtonStop), buttonPressedStop, FALLING);

  digitalWrite(pinLedStart, HIGH);
  digitalWrite(pinLedStop, HIGH);
  digitalWrite(LED_BUILTIN, blinkerState);

  Serial.println("Board started");
}

void loop() {
  unsigned long currentMillis = millis();

  /* Pruefen, ob der Start-Timer abgelaufen ist und ggf. den Button wieder freigeben */
  if (buttonLockedFlagStart && !buttonPressedFlagStart) {
    if (currentMillis - timePressedStart > buttonDelay) {
      digitalWrite(pinLedStart, HIGH);
      buttonLockedFlagStart = false;
    }
  }
  /* Start-Button wurde gedrueckt (und ist nicht gesperrt) */
  if (buttonLockedFlagStart && buttonPressedFlagStart) {
    timePressedStart = currentMillis;
    digitalWrite(pinLedStart, LOW);
    transmitter.send((byte *)buttonPressedMessageStart, sizeof(buttonPressedMessageStart));
    Serial.println("Start gedrueckt");
    buttonPressedFlagStart = false;
  }

  /* Pruefen, ob der Stop-Timer abgelaufen ist und ggf. den Button wieder freigeben */
  if (buttonLockedFlagStop && !buttonPressedFlagStop) {
    if (currentMillis - timePressedStop > buttonDelay) {
      digitalWrite(pinLedStop, HIGH);
      buttonLockedFlagStop = false;
    }
  }
  /* Stop-Button wurde gedrueckt (und ist nicht gesperrt) */
  if (buttonLockedFlagStop && buttonPressedFlagStop) {
    timePressedStop = currentMillis;
    digitalWrite(pinLedStop, LOW);
    transmitter.send((byte *)buttonPressedMessageStop, sizeof(buttonPressedMessageStop));
    Serial.println("Stop gedrueckt");
    buttonPressedFlagStop = false;
  }

  /* LED-Status-Blinker */
  if (currentMillis - timeLedBlinker >= blinkerInterval) {
    timeLedBlinker = currentMillis;
    blinkerState = !blinkerState;
    digitalWrite(LED_BUILTIN, blinkerState);
  }
}


void buttonPressedStart() {
  if (!buttonLockedFlagStart) {
    buttonLockedFlagStart = true;
    buttonPressedFlagStart = true;
  }
}

void buttonPressedStop() {
  if (!buttonLockedFlagStop) {
    buttonLockedFlagStop = true;
    buttonPressedFlagStop = true;
  }
}
