#include <Arduino.h>
#include "twai.h"
#include "pwm.h"
#include "fsm.h"
#include "led.h"

// Set LED_BUILTIN if it is not defined by Arduino framework
#define LED_BUILTIN 10

void setup()
{
  // Lamps and Debug LED
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LAMP1_ON, OUTPUT);
  pinMode(LAMP2_ON, OUTPUT);
  pinMode(LAMP3_ON, OUTPUT);
  pinMode(LAMP4_ON, OUTPUT);

  // TWAI TX
  pinMode(GPIO_NUM_48, OUTPUT);

  // TWAI RX
  pinMode(GPIO_NUM_47, INPUT);

  // Motor Outputs
  pinMode(GPIO_NUM_41, OUTPUT);
  pinMode(GPIO_NUM_40, OUTPUT);
  pinMode(GPIO_NUM_18, OUTPUT);
  pinMode(GPIO_NUM_17, OUTPUT);

  // Setup Serial Monitor
  Serial.begin(9600);

  // Initialize TWAI driver
  setup_twai();

  // Initialize PWM driver
  pwm_init();
}

void loop()
{
  // fsm_get_next_state(2, false);

  flash_leds();

  twai_message_t msg;
  rx_twai(&msg);

  digitalWrite(GPIO_NUM_17, HIGH);

  pwm_set_duty_pct();
}
