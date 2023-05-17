#include "led.h"
#include "fsm.h"

long int previous_time = millis();
bool previous_toggle = false;

void flash_leds(void)
{
    switch (current_state)
    {
        //case FSM_BOOT: // Fallthrough
        case FSM_STANDBY:
            digitalWrite(DEBUG_LED, HIGH);
            digitalWrite(LAMP1_ON, HIGH);
            digitalWrite(LAMP2_ON, HIGH);
            digitalWrite(LAMP3_ON, HIGH);
            digitalWrite(LAMP4_ON, HIGH);
            break;
        case FSM_AUTONOMOUS:
            // Toggle LEDs once every 250ms
            if ((millis() - previous_time) > 250)
            {
                digitalWrite(DEBUG_LED, previous_toggle);
                digitalWrite(LAMP1_ON, previous_toggle);
                digitalWrite(LAMP2_ON, previous_toggle);
                digitalWrite(LAMP3_ON, previous_toggle);
                digitalWrite(LAMP4_ON, previous_toggle);
                previous_toggle = !previous_toggle;
                previous_time = millis();
            }
            break;
        case FSM_MANUAL:
            // Toggle LEDs once every 2s
            if ((millis() - previous_time) > 2000)
            {
                digitalWrite(DEBUG_LED, previous_toggle);
                digitalWrite(LAMP1_ON, previous_toggle);
                digitalWrite(LAMP2_ON, previous_toggle);
                digitalWrite(LAMP3_ON, previous_toggle);
                digitalWrite(LAMP4_ON, previous_toggle);
                previous_toggle = !previous_toggle;
                previous_time = millis(); 
            }
            break;
        default:
            digitalWrite(DEBUG_LED, LOW);
            digitalWrite(LAMP1_ON, LOW);
            digitalWrite(LAMP2_ON, LOW);
            digitalWrite(LAMP3_ON, LOW);
            digitalWrite(LAMP4_ON, LOW);
            break;
    }
}
