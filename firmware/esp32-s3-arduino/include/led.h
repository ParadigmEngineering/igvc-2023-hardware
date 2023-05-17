/**  @file led.h
 *   @brief LED Drivers that flash periodically based on the state of the bot.
 *          
 *   flash_leds() flashes the LEDs on a set frequency based on the state of the bot.
*/

#define LAMP1_ON 4
#define LAMP2_ON 5
#define LAMP3_ON 2
#define LAMP4_ON 1

#define DEBUG_LED 10

extern long int previous_time;
extern bool     previous_toggle;

/*
 * @brief If the bot is in STANDBY, have the LEDs be on solid. Otherwise if the
 *        bot is in the AUTONOMOUS control state, toggle LEDs every 0.25s,
 *        otherwise if the bot is in the MANUAL control state, toggle LEDs every 2s
 */
void flash_leds(void);
