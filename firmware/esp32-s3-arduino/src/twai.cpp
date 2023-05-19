#include "twai.h"

long int last_heartbeat_time_ms = millis();

void setup_twai(void)
{
    //Initialize configuration structures using macro initializers
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_48, GPIO_NUM_47, TWAI_MODE_NORMAL);
    g_config.tx_queue_len = 10;
    g_config.rx_queue_len = 10;
    twai_timing_config_t  t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t  f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    //Install TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
    {
        Serial.println("Driver installed\n");
    }
    else
    {
        Serial.println("Failed to install driver\n");
        return;
    }

    //Start TWAI driver
    if (twai_start() == ESP_OK)
    {
        Serial.println("Driver started\n");
    }
    else
    {
        Serial.println("Failed to start driver\n");
        return;
    }
}

void tx_twai(twai_message_t* message)
{
    (void)twai_transmit(message, pdMS_TO_TICKS(1000));
}

void rx_twai(twai_message_t* message)
{
    (void)twai_receive(message, pdMS_TO_TICKS(1000));
}

void handle_twai_messages(int num_messages)
{
    twai_message_t     twai_message;
    twai_status_info_t twai_status;

    twai_get_status_info(&twai_status);

    for (int i = 0; i < twai_status.msgs_to_rx; i++)
    {
        Serial.println("Number of messages waiting to RX");
        Serial.println(twai_status.msgs_to_rx);

        rx_twai(&twai_message);

        if (twai_status.state == TWAI_STATE_RUNNING)
        {
            if (twai_message.identifier == TWAI_HEARTBEAT_ID)
            {
                Serial.println("Updating last heartbeat time!");
                last_heartbeat_time_ms = millis();

                // Configure new state message to transmit
                twai_message_t message;
                message.identifier = 0x0001;
                message.extd = 1;
                message.data_length_code = 4;
                for (int i = 0; i < 4; i++)
                {
                    message.data[i] = 0;
                }

                tx_twai(&message);
            }

            else if (TWAI_IS_MOTOR_CONTROL(twai_message.identifier))
            {
                if ((twai_message.identifier & TWAI_MOTOR_L_AUTONOMOUS_CONTROL_MASK) ||
                    (twai_message.identifier & TWAI_MOTOR_L_MANUAL_CONTROL_MASK))
                {
                    pwm_left_motor_control(&twai_message);
                }
                else if ((twai_message.identifier & TWAI_MOTOR_R_AUTONOMOUS_CONTROL_MASK) ||
                         (twai_message.identifier & TWAI_MOTOR_R_MANUAL_CONTROL_MASK))
                {
                    pwm_right_motor_control(&twai_message);
                }
            }

            else if (twai_message.identifier & TWAI_STATE_CHANGE_MASK)
            {
                fsm_get_next_state(twai_message.identifier);
            }
        }
    }

    // TODO: Integrate boot cycle on heartbeat timeout
    // if (last_heartbeat_time_ms >= 5000)
    // {
    //     fsm_get_next_state(twai_message.identifier);
    // }
}

void stop_twai(void)
{
    //Stop the TWAI driver
    if (twai_stop() == ESP_OK)
    {
        Serial.println("Driver stopped\n");
    }
    else
    {
        Serial.println("Failed to stop driver\n");
        return;
    }

    //Uninstall the TWAI driver
    if (twai_driver_uninstall() == ESP_OK)
    {
        Serial.println("Driver uninstalled\n");
    }
    else
    {
        Serial.println("Failed to uninstall driver\n");
        return;
    }
}
