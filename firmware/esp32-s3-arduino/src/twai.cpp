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

void tx_twai(void)
{
    //Configure message to transmit
    twai_message_t message;
    message.identifier = 0xAAAA;
    message.extd = 1;
    message.data_length_code = 4;
    for (int i = 0; i < 4; i++)
    {
        message.data[i] = 0;
    }

    //Queue message for transmission
    if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK)
    {
        Serial.println("Message queued for transmission\n");
    }
    else
    {
        Serial.println("Failed to queue message for transmission\n");
    }
}

void rx_twai(twai_message_t* message)
{
    //Wait for message to be received
    esp_err_t err_type = twai_receive(message, pdMS_TO_TICKS(10000));

    Serial.println("Error Type: ");
    Serial.println(err_type);

    if (err_type == ESP_OK)
    {
        Serial.println("Message received\n");

        for (int i = 0; i < 8; i++)
        {
            Serial.println(message->data[i]);
        }
    }
    else
    {
        Serial.println("Failed to receive message\n");
        for (int i = 0; i < 8; i++)
        {
            Serial.println(message->data[i]);
        }
        return;
    }

    //Process received message
    if (message->extd)
    {
        Serial.println("Message is in Extended Format\n");
    }
    else
    {
        Serial.println("Message is in Standard Format\n");
    }

    String debug_str = String("ID is ") + String(message->identifier) + String("\n");

    Serial.println(debug_str);

    if (!(message->rtr))
    {
        for (int i = 0; i < message->data_length_code; i++)
        {
            debug_str = String("Data byte ") + String(i) + String(" = ") + String(message->data[i]) + String("\n");
            Serial.println(debug_str);
        }
    }
}

void handle_twai_messages(int num_messages)
{
    twai_message_t     twai_message;
    twai_status_info_t twai_status;

    for (int i = 0; i < num_messages; i++)
    {
        rx_twai(&twai_message);
        twai_get_status_info(&twai_status);

        if (twai_status.state == TWAI_STATE_RUNNING)
        {
            if (twai_message.identifier == TWAI_HEARTBEAT_ID)
            {
                last_heartbeat_time_ms = millis();
            }

            else if (TWAI_IS_MOTOR_CONTROL(twai_message.identifier))
            {
                if ((twai_message.identifier & TWAI_MOTOR_L_CONTROL_MANUAL) ||
                    (twai_message.identifier & TWAI_MOTOR_L_CONTROL_AUTONOMOUS))
                {
                    pwm_left_motor_control(&twai_message);
                }
                else if ((twai_message.identifier & TWAI_MOTOR_R_CONTROL_MANUAL) ||
                         (twai_message.identifier & TWAI_MOTOR_R_CONTROL_AUTONOMOUS))
                {
                    pwm_right_motor_control(&twai_message);
                }
            }

            else if (twai_message.identifier & TWAI_STATE_CHANGE_MASK)
            {
                fsm_get_next_state(twai_message.identifier, false);
            }
        }

        // Stop receiving if there aren't any messages in the queue
        else if (twai_status.msgs_to_rx == 0)
        {
            break;
        }
    }
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
