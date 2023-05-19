#include "pwm.h"

void pwm_init(void)
{
    // Prepare and apply LEDC PWM timer for left motor
    ledc_timer_config_t ledc_timer_motor_left = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER_MOTOR_LEFT,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 3 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer_motor_left));

    // Prepare and apply LEDC PWM channel for left motor
    ledc_channel_config_t ledc_channel_motor_left = {
        .gpio_num       = LEDC_OUTPUT_IO_MOTOR_LEFT,
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_MOTOR_LEFT,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER_MOTOR_LEFT,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_motor_left));

    // Prepare and apply LEDC PWM timer for right motor
    ledc_timer_config_t ledc_timer_motor_right = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER_MOTOR_RIGHT,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 3 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer_motor_right));

    // Prepare and apply LEDC PWM channel for right motor
    ledc_channel_config_t ledc_channel_motor_right = {
        .gpio_num       = LEDC_OUTPUT_IO_MOTOR_RIGHT,
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_MOTOR_RIGHT,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER_MOTOR_RIGHT,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_motor_right));

    // Set duty pct default to 50%
    pwm_set_duty_pct();
}

void pwm_left_motor_control(twai_message_t* message)
{
    if (current_state == FSM_AUTONOMOUS ||
        current_state == FSM_MANUAL)
    {
        Serial.println("Handling Left motor controls!");
        Serial.println(message->identifier);

        if (message->identifier == TWAI_MOTOR_L_MANUAL_CONTROL_FORWARD ||
            message->identifier == TWAI_MOTOR_L_AUTONOMOUS_CONTROL_FORWARD)
        {
            Serial.println("Setting Left Dir Pin High to move forward!");
            digitalWrite(LEDC_OUTPUT_IO_MOTOR_LEFT_DIR, HIGH);
        }
        else if (message->identifier == TWAI_MOTOR_L_MANUAL_CONTROL_REVERSE ||
                 message->identifier == TWAI_MOTOR_L_AUTONOMOUS_CONTROL_REVERSE)
        {
            Serial.println("Setting Left Dir Pin Low to move backward!");
            digitalWrite(LEDC_OUTPUT_IO_MOTOR_LEFT_DIR, LOW);
        }

        uint64_t sum = 0;
        for (uint8_t i = 0; i < 4; i++)
        {
            sum += message->data[i];

            String debug_str = "Message Data Byte" + String(i) + ": \n";
            Serial.println(debug_str);
            Serial.println(message->data[i]); 
        }

        Serial.println("Sum of payload: ");
        Serial.println(sum);

        uint32_t left_motor_freq = linear_interpolate(sum);

        Serial.println("Setting left motor frequency to:");
        Serial.println(left_motor_freq);
        pwm_set_motor_left_freq(left_motor_freq);
    }
}

void pwm_right_motor_control(twai_message_t* message)
{
    if (current_state == FSM_AUTONOMOUS ||
        current_state == FSM_MANUAL)
    {
        Serial.println("Handling Right motor controls!");
        Serial.println(message->identifier);

        if (message->identifier == TWAI_MOTOR_R_MANUAL_CONTROL_FORWARD ||
            message->identifier == TWAI_MOTOR_R_AUTONOMOUS_CONTROL_FORWARD)
        {
            Serial.println("Setting Right Dir Pin High to move forward!");
            digitalWrite(LEDC_OUTPUT_IO_MOTOR_RIGHT_DIR, HIGH);
        }
        else if (message->identifier == TWAI_MOTOR_R_MANUAL_CONTROL_REVERSE ||
                 message->identifier == TWAI_MOTOR_R_AUTONOMOUS_CONTROL_REVERSE)
        {
            Serial.println("Setting Right Dir Pin Low to move backward!");
            digitalWrite(LEDC_OUTPUT_IO_MOTOR_RIGHT_DIR, LOW);
        }

        uint64_t sum = 0;
        for (uint8_t i = 0; i < 4; i++)
        {
            sum += message->data[i];

            String debug_str = "Message Data Byte" + String(i) + ": \n";
            Serial.println(debug_str);
            Serial.println(message->data[i]);
        }

        Serial.println("Sum of payload: ");
        Serial.println(sum);

        uint32_t right_motor_freq = linear_interpolate(sum);

        Serial.println("Setting right motor frequency to:");
        Serial.println(right_motor_freq);
        pwm_set_motor_left_freq(right_motor_freq);
    }
}

void pwm_set_duty_pct(void)
{
    // Set duty to 50%
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_MOTOR_LEFT, LEDC_DUTY));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_MOTOR_RIGHT, LEDC_DUTY));

    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_MOTOR_LEFT));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_MOTOR_RIGHT));
}

void pwm_clear_duty_pct(void)
{
    // Set duty to 50%
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_MOTOR_LEFT, 0));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_MOTOR_RIGHT, 0));

    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_MOTOR_LEFT));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_MOTOR_RIGHT));
}

void pwm_set_motor_left_freq(uint16_t freq)
{
    if (freq < LEDC_MAX_FREQUENCY &&
        freq > LEDC_MIN_FREQUENCY)
    {
        ESP_ERROR_CHECK(ledc_set_freq(LEDC_MODE, LEDC_TIMER_MOTOR_LEFT, freq));
    }
}

void pwm_set_motor_right_freq(uint16_t freq)
{
    if (freq < LEDC_MAX_FREQUENCY &&
        freq > LEDC_MIN_FREQUENCY)
    {
        ESP_ERROR_CHECK(ledc_set_freq(LEDC_MODE, LEDC_TIMER_MOTOR_RIGHT, freq));
    }
}
