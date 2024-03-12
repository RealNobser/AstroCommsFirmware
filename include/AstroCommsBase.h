#ifndef __ASTROCOMMS_H__
#define __ASTROCOMMS_H__

#include<Arduino.h>

#include"config.h"

class AstroCommsBase
{
    public:
        AstroCommsBase();
        bool init();
        void loop();

        const char* getProductName() { return "AstroComms Mega Shield Pro"; }

    protected:
        unsigned long HeartBeatMillis       = 0;
        byte HeartBeatStatus = LOW;

        unsigned long LED_STATUS_Millis     = 0;
        unsigned long LED_FLTHY_TX_Millis   = 0;
        unsigned long LED_DOME_TX_Millis    = 0;
        unsigned long LED_DOME_RX_Millis    = 0;
        unsigned long LED_BODY_TX_Millis    = 0;
        unsigned long LED_BODY_RX_Millis    = 0;
        unsigned long LED_XBEE_TX_Millis    = 0;
        unsigned long LED_XBEE_RX_Millis    = 0;

    protected:
        char SerialDomeBuffer[SERIAL_BUFFER_LEN];
        char SerialBodyBuffer[SERIAL_BUFFER_LEN];
        char SerialXBeeBuffer[SERIAL_BUFFER_LEN];
        char SerialDebugBuffer[SERIAL_BUFFER_LEN];

        unsigned int DomeBufferIndex = 0;
        unsigned int BodyBufferIndex = 0;
        unsigned int XBeeBufferIndex = 0;
        unsigned int DebugBufferIndex = 0;
 
    protected:
        void toggleHeartBeat();
        void checkSerialLEDs();
        void checkSerialLED(const uint8_t pin, unsigned long & ulMillis);


/*
 * Dispatch Methods:
 *
 * Dome RX  Dome TX
 * Body RX  Body TX
 * XBee RX  Dome TX
 * Debug RX Dome TX
 * 
 * Prefix   Route
 * ;        replace ; by : and send to body
 * #        sent to Flthy (must be replaced, collision with MarcDuino configuration commands!)
 * :SExx    sent to target, add Flthy command regarding command map (triggers Flthy on body sequences, too!)
 * 
*/

        void dispatchDomeCommand(const char* command);
        void dispatchBodyCommand(const char* command);
        void dispatchXBeeCommand(const char* command);
        void dispatchDebugCommand(const char* command);

        void writeDome(const uint8_t* data, const size_t data_len);
        void writeDome(const char* data);

        void writeBody(const uint8_t* data, const size_t data_len);
        void writeBody(const char* data);

        void writeFlthy(const uint8_t* data, const size_t data_len);
        void writeFlthy(const char* data);

        void writeXBee(const uint8_t* data, const size_t data_len);
        void writeXBee(const char* data);
};

#endif