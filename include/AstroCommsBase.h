#ifndef __ASTROCOMMS_H__
#define __ASTROCOMMS_H__

#include<Arduino.h>

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
        void toggleHeartBeat();
        void checkSerialLEDs();
        void checkSerialLED(const uint8_t pin, unsigned long & ulMillis);
};

#endif