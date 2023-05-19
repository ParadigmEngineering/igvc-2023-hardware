/**  @file twai_msg_defs.h
 *   @brief The TWAI message ID masks and IDs for Paradigm's 2023 IGVC bot.
 * 
 *   Based on these TWAI message IDs, the bot will either change states or
 *   actuate motors in different modes.
 */

#define TWAI_HEARTBEAT_ID                          0x0001

#define TWAI_STATE_CHANGE_MASK                     0x0010
#define TWAI_STANDBY_REQUEST                       0x0011
#define TWAI_AUTONOMOUS_REQUEST                    0x0012
#define TWAI_MANUAL_REQUEST                        0x0013
#define TWAI_BOOT_RESPONSE                         0x0016
#define TWAI_STANDBY_RESPONSE                      0x0017
#define TWAI_AUTONOMOUS_RESPONSE                   0x0018
#define TWAI_MANUAL_RESPONSE                       0x0019

#define TWAI_MOTOR_L_MANUAL_CONTROL_MASK           0x0020
#define TWAI_MOTOR_L_MANUAL_CONTROL_FORWARD        0x0021
#define TWAI_MOTOR_L_MANUAL_CONTROL_REVERSE        0x0022

#define TWAI_MOTOR_R_MANUAL_CONTROL_MASK           0x0040
#define TWAI_MOTOR_R_MANUAL_CONTROL_FORWARD        0x0041
#define TWAI_MOTOR_R_MANUAL_CONTROL_REVERSE        0x0042

#define TWAI_MOTOR_L_AUTONOMOUS_CONTROL_MASK       0x0080
#define TWAI_MOTOR_L_AUTONOMOUS_CONTROL_FORWARD    0x0081
#define TWAI_MOTOR_L_AUTONOMOUS_CONTROL_REVERSE    0x0082

#define TWAI_MOTOR_R_AUTONOMOUS_CONTROL_MASK       0x0100
#define TWAI_MOTOR_R_AUTONOMOUS_CONTROL_FORWARD    0x0101
#define TWAI_MOTOR_R_AUTONOMOUS_CONTROL_REVERSE    0x0102

#define TWAI_IS_MOTOR_CONTROL(motor) ((motor & TWAI_MOTOR_L_MANUAL_CONTROL_MASK) || (motor & TWAI_MOTOR_R_MANUAL_CONTROL_MASK) || (motor & TWAI_MOTOR_L_AUTONOMOUS_CONTROL_MASK) || (motor & TWAI_MOTOR_R_AUTONOMOUS_CONTROL_MASK))
