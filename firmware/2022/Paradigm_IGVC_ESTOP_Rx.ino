// Paradigm_IGVC_ESTOP_Rx.ino
//
// Code by Jasper Rubin
// Copyright Paradigm Engineering, 2022
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
  digitalWrite(OUTPUT_PIN, LOW);

  // there's likely no one listening to this but set it up in case it helps with debug
  Serial.begin(9600);

  // set up the weird library for this weird Chinese thing y'all went and bought
  // this line is ripped verbatim from the LED control example provided by the manufacturer
  Heltec.begin(false /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  
  // set up the OLED because it's there, might as well use it
  // initialization steps taken from the OLED examples from HelTec
  Heltec.display->init();
  Heltec.display->setFont(ArialMT_Plain_24);
  Heltec.display->drawString(0, 0, "Waiting for ESTOP...");
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

    // check if it's the message we want (should be the only message that will ever be sent)
    if (strncmp(buff, "ESTOP", index) == 0) {
      // drive the output high
      digitalWrite(OUTPUT_PIN, HIGH);
      
      // update the display
      Heltec.display->clear();
      Heltec.display->drawString(0, 0, "ESTOP TRIGGERED!");
      Heltec.display->display();
      
      Serial.println("ESTOP TRIGGERED!");
    } else {
      Serial.println("Packet does not request ESTOP.");
    }
    
    Serial.println("Packet read complete.");
  }
  
  delay(10);
}

