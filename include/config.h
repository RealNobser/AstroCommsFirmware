#ifndef __CONFIG_H__
#define __CONFIG_H__


// #define DEBUG_MSG                    // Additional Log Messages to Serial

// DO NOT EDIT BELOW THIS LINE UNLESS YOU REALLY KNOW WHAT YOU ARE DOING
// ------------------------------------------------

//
// Module Specific stuff
//
#define VERSION         "1.1.0"
#define HW_VERSION      "1.6.0"
#define CONFIG_VERSION  10

#define SERIAL_BAUD         9600
#define SERIAL_FLTHY_BAUD   9600
#define SERIAL_DOME_BAUD    9600
#define SERIAL_BODY_BAUD    9600
#define SERIAL_XBEE_BAUD    9600

#define SERIAL_BUFFER_LEN   256

#define HEARTBEAT_MILLIS    1000
#define SERIAL_BLINK_MILLIS 10

#define MAX_FLTHY_CMD_SIZE  6
#define MAX_SEQUENCE_NR     99

// Generic GPIOs
#define P_LED_STATUS    7
#define P_LED_FLTHY_TX  2
#define P_LED_DOME_TX   6
#define P_LED_DOME_RX   5
#define P_LED_BODY_TX   9
#define P_LED_BODY_RX   8
#define P_LED_XBEE_TX   4
#define P_LED_XBEE_RX   3

#define P_I2C_SDA       20
#define P_I2C_SCL       21

// Mega 2560 Rev3;
//
//  0(RX),   1(TX)  // Debug
// 19(RX1), 18(TX1) // Dome
// 17(RX2), 16(TX2) // XBEE
// 15(RX3), 14(TX3) // FLTHY

#define P_FLTHY_TX      14  // Serial 3
#define P_DOME_TX       18  // Serial 1
#define P_DOME_RX       19  // Serial 1
#define P_BODY_TX       11  // Soft Serial
#define P_BODY_RX       10  // Soft Serial
#define P_XBEE_TX       17  // Serial 2
#define P_XBEE_RX       16  // Serial 2

#define P_MP3_SIGNAL    P_DOME_TX

#define SerialDome  Serial1
#define SerialXBee  Serial2
#define SerialFlthy Serial3

#define P_JUMPER_MONITOR    12

#endif // __CONFIG_H__