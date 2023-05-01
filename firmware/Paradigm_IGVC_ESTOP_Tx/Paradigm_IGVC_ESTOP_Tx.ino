// Paradigm_IGVC_ESTOP_Tx.ino
//
// It's been a while since I wrote C, sorry in advance
//
// Code by Jasper Rubin
// Modified by Dylan Matthews
// Copyright Paradigm Engineering, 2023
// All rights reserved
//
// Wait for an interrupt from the ESTOP button (pulled low externally), send a LoRa packet
//
// Requires Heltec ESP32 library (see https://heltec-automation-docs.readthedocs.io/en/latest/esp32/quick_start.html for installation guide)

#include "heltec.h"

// On ESP32 we *should* be able to run an interrupt off any pin
// Picking GPIO12 mostly at random, was gonna use 38 but 36-39 are coupled with 270pF caps for some reason and the board mod seemed unnecessary
// GPIO12 comes out on the 16th pin of JP3
#define ESTOP_PIN 12

// Band for the LoRa, this must be the same for both modules or you're gonna have a bad time
#define BAND 868E6

// This should never matter because the ESTOP will only be triggered once per power cycle
// but I put in software debouncing because I have some amount of pride in my work
#define DEBOUNCE_TIME 100

// Used for debouncing
unsigned long lastInterruptTime = 0;

// Flag to send packet, flipped by ISR
bool interruptFlag = true;  // set true to send estop button status on boot

// get IO state when interrupt triggered
int estopStatus = 0;

void displayText(String text, int y = 0) {
  Heltec.display->clear();
  Heltec.display->drawString(0, y, text);
  Heltec.display->display();
}

// ISR to run when ESTOP is pressed
// Flagged as IRAM_ATTR, to tell the compiler
// to put this in the internal RAM, not flash (makes it faster)
void IRAM_ATTR estopPressedISR () {
  // quick check for debouncing purposes, feel free
  // to scrap this if it's causing issues (will just open you
  // up to switch bounce, but that shouldn't be an issue anyways
  if (millis() - lastInterruptTime > DEBOUNCE_TIME) {
    interruptFlag = true;
  }
  lastInterruptTime = millis();
}


void setup () {
  pinMode(ESTOP_PIN, INPUT);

  // there's likely no one listening to this but for debug it may be useful
  Serial.begin(9600);

  // according to https://lastminuteengineers.com/handling-esp32-gpio-interrupts-tutorial/ we don't
  // need to use the Arduino digitalPinToInterrupt() function here, just give the GPIO number
  attachInterrupt(ESTOP_PIN, estopPressedISR, CHANGE);

  // set up the weird library for this weird Chinese thing y'all went and bought
  // this line is ripped verbatim from the LED control example provided by the manufacturer
  // also, their code calls the second arguemtn "LoRa Disable," but in the library source code it's called "LoRa Enable" which makes more sense
  // I will assume the latter is correct
  Heltec.begin(false /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  LoRa.setTxPower(14, RF_PACONFIG_PASELECT_PABOOST); // also taken verbatim, I have no clue what this does

  // set up the OLED because it's there, might as well use it
  // initialization steps taken from the OLED examples from HelTec
  Heltec.display->init();
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Waiting for ESTOP");
  Heltec.display->display();
}

void loop () {
  if (interruptFlag) {
    estopStatus = digitalRead(ESTOP_PIN);

    if (estopStatus == 1) {
//      Serial.println("ESTOP TRIGGERED! Sending packet...");
    
      // send a packet to the other LoRa
      LoRa.beginPacket();
      LoRa.print("ESTOP");
      LoRa.endPacket();
    
      // reset the flag
      interruptFlag = false;

      // update the display
      Heltec.display->clear();
      Heltec.display->drawString(0, 0, "ESTOP TRIGGERED!");
      Heltec.display->display();
      
      Serial.println("Packet sent.");
    }
    else if (estopStatus == 0) {
//      Serial.println("Clearing ESTOP...");
    
      // send a packet to the other LoRa
      LoRa.beginPacket();
      LoRa.print("CLEAR");
      LoRa.endPacket();
    
      // reset the flag
      interruptFlag = false;

      // update the display
      Heltec.display->clear();
      Heltec.display->drawString(0, 0, "ESTOP cleared");
      Heltec.display->drawString(0, 10, "Waiting for ESTOP...");
      Heltec.display->display();

      Serial.println("Packet sent.");
    }
  }

  // idle while we wait for ESTOP to be triggered
  delay(10);
}
