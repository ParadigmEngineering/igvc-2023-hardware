#include "linear_interpolate.h"

uint32_t linear_interpolate(uint32_t xp)
{
    uint32_t yp = 0;

    float y0 = MOTOR_FREQUENCY_MIN;
    float y1 = MOTOR_FREQUENCY_MAX;

    float x0 = CAN_MESSAGE_PAYLOAD_MIN;
    float x1 = CAN_MESSAGE_PAYLOAD_MAX;

    yp = y0 + (((y1 - y0) / (x1 - x0)) * (xp - x0));

    return yp;
}
