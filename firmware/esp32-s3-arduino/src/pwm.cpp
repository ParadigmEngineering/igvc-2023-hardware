#include "pwm.h"

void pwm_init(void)
{
    // Prepare and apply LEDC PWM timer for left motor
    ledc_timer_config_t ledc_timer_motor_left = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER_MOTOR_LEFT,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 3.733 kHz
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
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 3.733 kHz
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
}

void pwm_left_motor_control(twai_message_t* message)
{
    Serial.println(message->identifier);
}

void pwm_right_motor_control(twai_message_t* message)
{
    Serial.println(message->identifier);
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

void pwm_set_freq(void)
{
    // Set frequency to 2kHz
    ESP_ERROR_CHECK(ledc_set_freq(LEDC_MODE, LEDC_TIMER_MOTOR_LEFT, 2000));
    ESP_ERROR_CHECK(ledc_set_freq(LEDC_MODE, LEDC_TIMER_MOTOR_RIGHT, 2000));
}
