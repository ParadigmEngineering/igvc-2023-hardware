/**  @file linear_interpolate.h
 *   @brief Linear interpolate motor frequencies from a CAN message payload.
*/

#ifndef LINEAR_INTERPOLATE_H
#define LINEAR_INTERPOLATE_H

#include <stdint.h>
#include <Arduino.h>

#define MOTOR_FREQUENCY_MAX (3733)
#define MOTOR_FREQUENCY_MIN (0)

#define CAN_MESSAGE_PAYLOAD_MAX (1020)
#define CAN_MESSAGE_PAYLOAD_MIN (0)

/*
 * @brief Linear interpolate the motor outputs based on CAN message payload
 */
uint32_t linear_interpolate(uint32_t xp);

#endif // LINEAR_INTERPOLATE_H
