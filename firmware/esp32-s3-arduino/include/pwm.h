/**  @file pwm.h
 *   @brief The PWM (Pulse Width Modulation) driver for Paradigm's 2023 IGVC bot.
 * 
 *   Reference Documentation: https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/ledc.html#ledc-api-change-pwm-frequency
 *          
 *   Interface for actuating motor controls, i.e. PWM Duty Cycle 50% means the motors will spin at half of max speed.
 *   Makes use of the ledc library, actuating PWM through a led control intensity peripheral.
 */

#ifndef PWM_H
#define PWM_H

#include <Arduino.h>
#include <driver/ledc.h>
#include <stdio.h>
#include <esp_err.h>
#include "twai.h"
#include "linear_interpolate.h"

#define LEDC_OUTPUT_IO_MOTOR_LEFT_DIR   GPIO_NUM_17
#define LEDC_OUTPUT_IO_MOTOR_RIGHT_DIR  GPIO_NUM_40

#define LEDC_MAX_FREQUENCY              (3733)
#define LEDC_MIN_FREQUENCY              (0)

#define LEDC_TIMER_MOTOR_LEFT           LEDC_TIMER_0
#define LEDC_TIMER_MOTOR_RIGHT          LEDC_TIMER_1
#define LEDC_MODE                       LEDC_LOW_SPEED_MODE

// Motor Control IO Outputs
#define LEDC_OUTPUT_IO_MOTOR_LEFT       GPIO_NUM_18
#define LEDC_OUTPUT_IO_MOTOR_RIGHT      GPIO_NUM_41

#define LEDC_CHANNEL_MOTOR_LEFT         LEDC_CHANNEL_0
#define LEDC_CHANNEL_MOTOR_RIGHT        LEDC_CHANNEL_1
#define LEDC_DUTY_RES                   LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY                       (4095) // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
#define LEDC_FREQUENCY                  (3000) // Frequency in Hertz. Set frequency at 3 kHz

/*
 * @brief Configures and initializes PWM/LEDC drivers.
 */
void pwm_init(void);

/*
 * @brief Controls a motor based on the CAN message received.
 */
void pwm_left_motor_control(twai_message_t* message);

/*
 * @brief Controls a motor based on the CAN message received.
 */
void pwm_right_motor_control(twai_message_t* message);

/*
 * @brief Sets the target duty cycle percent output.
 */
void pwm_set_duty_pct(void);

/*
 * @brief Set the target duty cycle percent to 0.
 */
void pwm_clear_duty_pct(void);

/*
 * @brief Set the left motor PWM frequency.
 */
void pwm_set_motor_left_freq(uint16_t freq);

/*
 * @brief Set the right motor PWM frequency.
 */
void pwm_set_motor_right_freq(uint16_t freq);

#endif // PWM_H
