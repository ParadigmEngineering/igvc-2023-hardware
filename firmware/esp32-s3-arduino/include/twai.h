/**  @file twai.h
 *   @brief The TWAI (Two-Wire Automotive Interface) driver for Paradigm's 2023 IGVC bot.
 * 
 *   Reference Documentation: https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/twai.html#examples
 *          
 *   Based on control messages received with this protocol, the bot will establish a heartbeat with the
 *   control desktop, the motors will actuate or the bot will change its control state.
 */

#ifndef TWAI_H
#define TWAI_H

#include <Arduino.h>
#include <driver/gpio.h>
#include <driver/twai.h>
#include "twai_msg_defs.h"
#include "pwm.h"
#include "fsm.h"

extern long int last_heartbeat_time_ms;

/*
 * @brief Configures and initializes TWAI drivers.
 */
void setup_twai(void);

/*
 * @brief Send TWAI messages over configured pins.
 */
void tx_twai(twai_message_t* message);

/*
 * @brief Receive TWAI messages over configured pins.
 */
void rx_twai(twai_message_t* message);

/*
 * @brief Handle a given number of TWAI messages per loop.
 */
void handle_twai_messages(int num_messages);

/*
 * @brief Stops and uninstalls the TWAI drivers. 
 */
void stop_twai(void);

#endif // TWAI_H
