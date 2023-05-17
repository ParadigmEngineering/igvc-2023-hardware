#include "linear_interpolate.h"

uint32_t linear_interpolate(uint32_t xp)
{
    uint32_t yp = 0;

    uint32_t y0 = MOTOR_FREQUENCY_MIN;
    uint32_t y1 = MOTOR_FREQUENCY_MAX;

    uint32_t x0 = CAN_MESSAGE_PAYLOAD_MIN;
    uint32_t x1 = CAN_MESSAGE_PAYLOAD_MAX;

    yp = y0 + ((y1 - y0) / (x1 - x0)) * (xp - x0);

    return yp;
}