// Paradigm_IGVC_ESTOP_Rx.ino
//
// Code by Jasper Rubin
// Modified by Dylan Matthews
// Copyright Paradigm Engineering, 2023
// All rights reserved
//
// Wait for a LoRa packet from the board connected to the ESTOP button
// When we receive it, drive a pin high
//
// Requires Heltec ESP32 library (see https://heltec-automation-docs.readthedocs.io/en/latest/esp32/quick_start.html for installation guide)

#include "heltec.h"
#include "string.h"

// Pin to drive high when ESTOP is pressed
// GPIO12 comes out on the 16th pin of JP3
#define OUTPUT_PIN 12

// Band for the LoRa, this must be the same for both modules or you're gonna have a bad time
#define BAND 868E6

void setup () {
  pinMode(OUTPUT_PIN, OUTPUT);
  digitalWrite(OUTPUT_PIN, HIGH); // have estop be active on boot to cover case where receiver is turned on while transmitter button is already latched

  // there's likely no one listening to this but set it up in case it helps with debug
  Serial.begin(9600);

  // set up the weird library for this weird Chinese thing y'all went and bought
  // this line is ripped verbatim from the LED control example provided by the manufacturer
  Heltec.begin(false /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  
  // set up the OLED because it's there, might as well use it
  // initialization steps taken from the OLED examples from HelTec
  Heltec.display->init();
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 0, "ESTOP active");
  Heltec.display->drawString(0, 10, "Clear with remote to begin");
  Heltec.display->display();
}

void loop () {
  // try to parse a LoRa packet, if we succeed we received one
  // this framework is lifted from the Rx script for the LED control example by HelTec
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    // we got one
    Serial.println("Packet received, reading...");

    // ah, the pains of C
    // Arduino can do some more C++ish stringy things
    // but I don't trust them
    char buff[50];
    int index = 0;
    while (LoRa.available()) {
      buff[index++] = (char)LoRa.read();
    }

    // check message contents and set/clear pin
    if (strncmp(buff, "ESTOP", index) == 0) {
      // drive the output high
      digitalWrite(OUTPUT_PIN, HIGH);
      
      // update the display
      Heltec.display->clear();
      Heltec.display->drawString(0, 0, "ESTOP TRIGGERED!");
      Heltec.display->display();
      
      Serial.println("ESTOP TRIGGERED!");
    } 
    else if (strncmp(buff, "CLEAR", index) == 0) {
      // drive output low
      digitalWrite(OUTPUT_PIN, LOW);

      // update the display
      Heltec.display->clear();
      Heltec.display->drawString(0, 0, "ESTOP cleared");
      Heltec.display->drawString(0, 10, "Waiting for ESTOP...");
      Heltec.display->display();
    } 
    else {
      Serial.println("Packet does not request ESTOP.");
    }
    
    Serial.println("Packet read complete.");
  }
  
  delay(10);
}
