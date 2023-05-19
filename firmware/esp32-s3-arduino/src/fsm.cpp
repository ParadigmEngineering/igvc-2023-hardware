#include "fsm.h"

enum fsm_state_E current_state = FSM_BOOT;
enum fsm_state_E next_state = FSM_BOOT;

long int fsm_current_time = millis();
long int boot_transition_time = millis();

void fsm_get_next_state(uint32_t id)
{
    uint32_t new_state_identifier = 0;

    switch(current_state)
    {
        case FSM_STANDBY:
        {
            if (id == TWAI_AUTONOMOUS_REQUEST)
            {
                next_state = FSM_AUTONOMOUS;
                new_state_identifier = TWAI_AUTONOMOUS_RESPONSE;
                break;
            }
            if (id == TWAI_MANUAL_REQUEST)
            {
                next_state = FSM_MANUAL;
                new_state_identifier = TWAI_MANUAL_RESPONSE;
                break;
            }
            break;
        }
        case FSM_AUTONOMOUS: // Fallthrough
        case FSM_MANUAL:
        {
            if (id == TWAI_STANDBY_REQUEST)
            {
                next_state = FSM_STANDBY;
                new_state_identifier = TWAI_STANDBY_RESPONSE;
            }
            break;
        }
        case FSM_BOOT:
        {
            // Delay BOOT -> STANDBY transition for 10s
            if ((fsm_current_time - boot_transition_time > 10000) && (digitalRead(ESTOP) == LOW))
            {
                boot_transition_time = millis();
                next_state = FSM_STANDBY;
                new_state_identifier = TWAI_STANDBY_RESPONSE;   
            }

            break;
        }
        case FSM_STATE_COUNT: // Fallthrough
        default:
        {
            next_state = FSM_BOOT;
            new_state_identifier = TWAI_BOOT_RESPONSE;
            break;
        }
    }

    // Configure new state message to transmit
    twai_message_t message;
    message.identifier = new_state_identifier;
    message.extd = 1;
    message.data_length_code = 4;
    for (int i = 0; i < 4; i++)
    {
        message.data[i] = 0;
    }

    tx_twai(&message);

    // Update current state
    current_state = next_state;
}
