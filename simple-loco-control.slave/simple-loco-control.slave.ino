/* Copyright 2018 Stefan Daniel Homfeld, www.stedaho.de */

#include <RFTransmitter.h> /* Siehe https://github.com/zeitgeist87/RFTransmitter */

void buttonPressedA();
void buttonPressedB();

struct BUTTON_CONFIG {  
  /* Pin-Zuordnungen */
  byte pinButton;
  byte pinLed;
  /* Flags zur Abhandlung und dem Sperren der Tasterdrücke */
  volatile bool pressedFlag;
  volatile bool lockedFlag;
  /* Zeitstempel zur Sperre der Tasterdrücke */
  unsigned long timePressed;
  /* Nachricht, die beim Tastendruck übertragen wird */
  byte pressedMessage[4];
  /* Interrupt-Handler, der beim Tastendruck aufgerufen wird */
  void (*interruptHandler)();
};

BUTTON_CONFIG buttons[2] = { 
  {2, 5, false, false, 0, { 28, 0, 1, 82 }, buttonPressedA}, 
  {3, 6, false, false, 0, { 28, 1, 1, 82 }, buttonPressedB}
};
const int buttonCount = sizeof(buttons)/sizeof(BUTTON_CONFIG);

const long buttonDelay = 5000;

/* Zeitstempel für den Blinker */
unsigned long timeLedBlinker = 0;
const long blinkerInterval = 500;
bool blinkerState = true;

/* Transmitter-Einstellungen */
const byte pinTransmitter = 4;
const byte transmitterId = 28;
RFTransmitter transmitter(pinTransmitter, transmitterId);


void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Slave starting up...");
  
  pinMode(pinTransmitter, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  for (int btn=0; btn<buttonCount; btn++) {
    pinMode(buttons[btn].pinLed, OUTPUT);
    pinMode(buttons[btn].pinButton, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(buttons[btn].pinButton), buttons[btn].interruptHandler, FALLING);
    digitalWrite(buttons[btn].pinLed, HIGH);
  }

  digitalWrite(LED_BUILTIN, blinkerState);

  Serial.println("Slave started");
}

void loop() {
  unsigned long currentMillis = millis();

  for (int btn=0; btn<buttonCount; btn++) {
    /* Pruefen, ob der Timer abgelaufen ist und ggf. den Button wieder freigeben */
    if (buttons[btn].lockedFlag && !buttons[btn].pressedFlag) {
      if (currentMillis - buttons[btn].timePressed > buttonDelay) {
        digitalWrite(buttons[btn].pinLed, HIGH);
        buttons[btn].lockedFlag = false;
      }
    }
    /* Button wurde gedrueckt (und ist nicht gesperrt) */
    if (buttons[btn].lockedFlag && buttons[btn].pressedFlag) {
      buttons[btn].timePressed = currentMillis;
      digitalWrite(buttons[btn].pinLed, LOW);
      transmitter.send((byte *)buttons[btn].pressedMessage, sizeof(buttons[btn].pressedMessage));
      Serial.print("Button ");
      Serial.print(btn);
      Serial.println(" gedrueckt");
      buttons[btn].pressedFlag = false;
    }
  }
  
  /* LED-Status-Blinker */
  if (currentMillis - timeLedBlinker >= blinkerInterval) {
    timeLedBlinker = currentMillis;
    blinkerState = !blinkerState;
    digitalWrite(LED_BUILTIN, blinkerState);
  }
}


void buttonPressedA() {
  if (!buttons[0].lockedFlag) {
    buttons[0].lockedFlag = true;
    buttons[0].pressedFlag = true;
  }
}

void buttonPressedB() {
  if (!buttons[1].lockedFlag) {
    buttons[1].lockedFlag = true;
    buttons[1].pressedFlag = true;
  }
}
