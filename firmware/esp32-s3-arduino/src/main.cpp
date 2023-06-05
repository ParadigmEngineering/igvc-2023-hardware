#include <Arduino.h>
#include "twai.h"
#include "pwm.h"
#include "fsm.h"
#include "led.h"

// Override LED_BUILTIN defined by Arduino framework
#define LED_BUILTIN 10

long int startup_time = 0;
long int kick_count_1 = 0;
long int kick_count_2 = 0;
long int kick_time_1 = millis();
long int kick_time_2 = millis();

void estop_routine()
{
  current_state = FSM_BOOT;
  next_state = FSM_STANDBY;

  pwm_set_motor_left_freq(0);
  pwm_set_motor_right_freq(0);

  ledc_timer_pause(LEDC_MODE, LEDC_TIMER_MOTOR_LEFT);
  ledc_timer_pause(LEDC_MODE, LEDC_TIMER_MOTOR_RIGHT);

  // pwm_clear_duty_pct();

  digitalWrite(DEBUG_LED, LOW);
  digitalWrite(LAMP1_ON, LOW);
  digitalWrite(LAMP2_ON, LOW);
  digitalWrite(LAMP3_ON, LOW);
  digitalWrite(LAMP4_ON, LOW);

  digitalWrite(GPIO_NUM_39, LOW);
  digitalWrite(GPIO_NUM_16, LOW);

  twai_clear_receive_queue();
}

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
  pinMode(GPIO_NUM_39, OUTPUT);
  pinMode(GPIO_NUM_18, OUTPUT);
  pinMode(GPIO_NUM_17, OUTPUT);
  pinMode(GPIO_NUM_16, OUTPUT);

  // Enable motor pins
  digitalWrite(GPIO_NUM_39, HIGH);
  digitalWrite(GPIO_NUM_16, HIGH);

  // Initial Direction is Forward
  digitalWrite(GPIO_NUM_40, HIGH);
  digitalWrite(GPIO_NUM_17, HIGH);

  // E-stop
  pinMode(GPIO_NUM_9, INPUT);

  // Setup Serial Monitor
  Serial.begin(9600);

  // Initialize TWAI driver
  setup_twai();

  // Initialize PWM driver
  pwm_init();

  startup_time = millis();

  attachInterrupt(ESTOP, estop_routine, RISING);

  // pwm_set_motor_left_freq(2000);
  // pwm_set_motor_right_freq(2000);
}

void loop()
{
  long int current_time = millis();
  fsm_current_time = millis();

  handle_twai_messages(10);

  flash_leds();

  // Gradual PWM Decrease test
  // if ((current_time - startup_time) > 30000)
  // {
  //   pwm_estop();
  // }

  // TODO: Validate Estop Functionality
  if (digitalRead(GPIO_NUM_7) == LOW)
  {
    digitalWrite(GPIO_NUM_16, LOW);
    delay(100);
    digitalWrite(GPIO_NUM_16, HIGH);
    if (kick_count_1 == 0)
    {
      kick_time_1 = millis();
    }
    kick_count_1++;
  }
  else if (digitalRead(GPIO_NUM_37) == LOW)
  {
    digitalWrite(GPIO_NUM_39, LOW);
    delay(100);
    digitalWrite(GPIO_NUM_39, HIGH);
    if (kick_count_2 == 0)
    {
      kick_time_2 = millis();
    }
    kick_count_2++;
  }
  else if ((kick_count_1 >= 2) || (kick_count_2 >= 2) )
  {
    estop_routine();
  }
  else if ((millis() - kick_time_1) > 60000)
  {
    kick_count_1 = 0;
  }
  else if ((millis() - kick_time_2) > 60000)
  {
    kick_count_2 = 0;
  }
  else
  {
    digitalWrite(GPIO_NUM_39, HIGH);
    digitalWrite(GPIO_NUM_16, HIGH);
  }





  // // TODO: Validate Estop Functionality
  // if (digitalRead(ESTOP) == HIGH)
  // {
  //   // current_state = FSM_BOOT;
  //   // next_state = FSM_STANDBY;

  //   pwm_set_motor_left_freq(0);
  //   pwm_set_motor_right_freq(0);

  //   ledc_timer_pause(LEDC_MODE, LEDC_TIMER_MOTOR_LEFT);
  //   ledc_timer_pause(LEDC_MODE, LEDC_TIMER_MOTOR_RIGHT);

  //   // pwm_clear_duty_pct();

  //   digitalWrite(DEBUG_LED, LOW);
  //   digitalWrite(LAMP1_ON, LOW);
  //   digitalWrite(LAMP2_ON, LOW);
  //   digitalWrite(LAMP3_ON, LOW);
  //   digitalWrite(LAMP4_ON, LOW);

  //   digitalWrite(GPIO_NUM_39, LOW);
  //   digitalWrite(GPIO_NUM_16, LOW);

  //   // twai_clear_receive_queue();
  // }
  // else
  // {
  //   delay(5000);

  //   Serial.println("Turning on motors again");
  //   digitalWrite(GPIO_NUM_39, HIGH);
  //   digitalWrite(GPIO_NUM_16, HIGH);

  //   ledc_timer_resume(LEDC_MODE, LEDC_TIMER_MOTOR_LEFT);
  //   ledc_timer_resume(LEDC_MODE, LEDC_TIMER_MOTOR_RIGHT);

  //   pwm_set_motor_left_freq(2000);
  //   pwm_set_motor_right_freq(2000);
  // }
}
