#include "pwm.h"

void pwm_init(void)
{
    // Prepare and apply LEDC PWM timer for left motor
    ledc_timer_config_t ledc_timer_motor_left = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER_MOTOR_LEFT,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 3 kHz
        .clk_cfg          = LEDC_USE_APB_CLK
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
        .clk_cfg          = LEDC_USE_APB_CLK
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

    pwm_set_motor_left_freq(0);
    pwm_set_motor_right_freq(0);

    ledc_timer_pause(LEDC_MODE, LEDC_TIMER_MOTOR_LEFT);
    ledc_timer_pause(LEDC_MODE, LEDC_TIMER_MOTOR_RIGHT);
}

void pwm_left_motor_control(twai_message_t* message)
{
    if (current_state == FSM_AUTONOMOUS ||
        current_state == FSM_MANUAL)
    {
        Serial.println("Handling Left motor controls!");
        // Serial.println(message->identifier);

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

            // String debug_str = "Message Data Byte" + String(i) + ": \n";
            // Serial.println(debug_str);
            // Serial.println(message->data[i]); 
        }

        if (sum == 0)
        {
            
            ledc_timer_pause(LEDC_MODE, LEDC_TIMER_MOTOR_LEFT);
        }
        else
        {
            ledc_timer_resume(LEDC_MODE, LEDC_TIMER_MOTOR_LEFT);
        }

        Serial.println("Sum of payload: ");
        Serial.println(sum);

        uint32_t left_motor_freq = linear_interpolate(sum);

        Serial.println("Setting left motor frequency to:");
        Serial.println(left_motor_freq);
        pwm_set_motor_left_freq(left_motor_freq);

        Serial.println("Actual frequency Set to:");
        Serial.println(ledc_get_freq(LEDC_MODE, LEDC_TIMER_MOTOR_LEFT));
    }

    // Otherwise stop all motor actuations
    else
    {
        ledc_timer_pause(LEDC_MODE, LEDC_TIMER_MOTOR_LEFT);
    }
}

void pwm_right_motor_control(twai_message_t* message)
{
    if (current_state == FSM_AUTONOMOUS ||
        current_state == FSM_MANUAL)
    {
        Serial.println("Handling Right motor controls!");
        // Serial.println(message->identifier);

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

            // String debug_str = "Message Data Byte" + String(i) + ": \n";
            // Serial.println(debug_str);
            // Serial.println(message->data[i]);
        }

        if (sum == 0)
        {
            ledc_timer_pause(LEDC_MODE, LEDC_TIMER_MOTOR_RIGHT);
        }
        else
        {
            ledc_timer_resume(LEDC_MODE, LEDC_TIMER_MOTOR_RIGHT);
        }

        Serial.println("Sum of payload: ");
        Serial.println(sum);

        uint32_t right_motor_freq = linear_interpolate(sum);

        Serial.println("Setting right motor frequency to:");
        Serial.println(right_motor_freq);
        pwm_set_motor_right_freq(right_motor_freq);

        Serial.println("Actual frequency Set to:");
        Serial.println(ledc_get_freq(LEDC_MODE, LEDC_TIMER_MOTOR_RIGHT));
    }

    // Otherwise stop all motor actuations
    else
    {
        ledc_timer_pause(LEDC_MODE, LEDC_TIMER_MOTOR_RIGHT);
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

void pwm_estop(void)
{
    uint32_t motor_left_freq = ledc_get_freq(LEDC_MODE, LEDC_TIMER_MOTOR_LEFT);
    uint32_t motor_right_freq = ledc_get_freq(LEDC_MODE, LEDC_TIMER_MOTOR_RIGHT);

    Serial.println("Motor Left freq:");
    Serial.println(motor_left_freq);

    Serial.println("Motor Right freq:");
    Serial.println(motor_right_freq);

    if (motor_left_freq <= 250 && motor_right_freq <= 250)
    {
        Serial.println("Pausing both motors!");
        ledc_timer_pause(LEDC_MODE, LEDC_TIMER_MOTOR_LEFT);
        ledc_timer_pause(LEDC_MODE, LEDC_TIMER_MOTOR_RIGHT);
        return;
    }

    uint16_t time_to_stop = 12; // 12 time iterations, i.e. 250ms intervals over 3s total

    uint32_t motor_left_decrease = motor_left_freq / time_to_stop;
    uint32_t motor_right_decrease = motor_right_freq / time_to_stop;

    bool slow_left = false;
    bool slow_right = false;

    if (motor_left_freq == motor_right_freq)
    {
        slow_left = true;
        slow_right = true;
    }
    else if (motor_left_freq > motor_right_freq)
    {
        slow_left = true;
    }
    else
    {
        slow_right = true;
    }

    for (uint16_t t = 0; t < time_to_stop; t++)
    {
        // Slow down motor actuations
        if (slow_left && slow_right)
        {
            motor_left_freq -= motor_left_decrease;
            motor_right_freq -= motor_right_decrease;
        }
        else if (slow_left)
        {
            motor_left_freq -= motor_left_decrease;
        }
        else if (slow_right)
        {
            motor_right_freq -= motor_right_decrease;
        }

        if (motor_left_freq == 250)
        {
            motor_left_freq = 0;
            slow_left = false;
            motor_left_decrease = 0;

            Serial.println("Pausing left motor!");
            ledc_timer_pause(LEDC_MODE, LEDC_TIMER_MOTOR_LEFT);
        }

        if (motor_right_freq == 250)
        {
            motor_right_freq = 0;
            slow_right = false;
            motor_right_decrease = 0;

            Serial.println("Pausing right motor!");
            ledc_timer_pause(LEDC_MODE, LEDC_TIMER_MOTOR_RIGHT);
        }

        if (motor_left_freq < 500)
        {
            motor_left_freq = 250;
            slow_left = false;
            motor_left_decrease = 0;

            ledc_timer_pause(LEDC_MODE, LEDC_TIMER_MOTOR_LEFT);
        }

        if (motor_right_freq < 500)
        {
            motor_right_freq = 250;
            slow_right = false;
            motor_right_decrease = 0;

            ledc_timer_pause(LEDC_MODE, LEDC_TIMER_MOTOR_RIGHT);
        }

        Serial.println("Setting Left Freq to:");
        Serial.println(motor_left_freq);

        Serial.println("Setting Right Freq to:");
        Serial.println(motor_right_freq);

        pwm_set_motor_left_freq(motor_left_freq);
        pwm_set_motor_right_freq(motor_right_freq);

        uint16_t delayed_time_to_stop = time_to_stop - t;

        // Update what motors to slow down and decrease rates
        if (slow_left && (motor_right_freq > motor_left_freq))
        {
            slow_right = true;
            motor_right_decrease = motor_right_freq / delayed_time_to_stop;
        }

        else if (slow_right && (motor_left_freq > motor_right_freq))
        {
            slow_left = true;
            motor_left_decrease = motor_left_freq / delayed_time_to_stop;
        }
    }
}
