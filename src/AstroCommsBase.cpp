#include <AstroCommsBase.h>
#include <SoftwareSerial.h>
#include <config.h>

extern SoftwareSerial SerialBody;

AstroCommsBase::AstroCommsBase()
{
    HeartBeatMillis     = millis();
}

bool AstroCommsBase::init()
{
    pinMode(P_LED_STATUS, OUTPUT);
    pinMode(P_LED_FLTHY_TX, OUTPUT);
    pinMode(P_LED_DOME_TX, OUTPUT);
    pinMode(P_LED_DOME_RX, OUTPUT);
    pinMode(P_LED_BODY_TX, OUTPUT);
    pinMode(P_LED_BODY_RX, OUTPUT);
    pinMode(P_LED_XBEE_TX, OUTPUT);
    pinMode(P_LED_XBEE_RX, OUTPUT);

    digitalWrite(P_LED_STATUS, LOW);
    digitalWrite(P_LED_FLTHY_TX, LOW);
    digitalWrite(P_LED_DOME_TX, LOW);
    digitalWrite(P_LED_DOME_RX, LOW);
    digitalWrite(P_LED_BODY_TX, LOW);
    digitalWrite(P_LED_BODY_RX, LOW);
    digitalWrite(P_LED_XBEE_TX, LOW);
    digitalWrite(P_LED_XBEE_RX, LOW);

    // LED-Check

    digitalWrite(P_LED_STATUS, HIGH);
    delay(100);
    digitalWrite(P_LED_FLTHY_TX, HIGH);
    delay(100);
    digitalWrite(P_LED_DOME_TX, HIGH);
    delay(100);
    digitalWrite(P_LED_DOME_RX, HIGH);
    delay(100);
    digitalWrite(P_LED_BODY_TX, HIGH);
    delay(100);
    digitalWrite(P_LED_BODY_RX, HIGH);
    delay(100);
    digitalWrite(P_LED_XBEE_TX, HIGH);
    delay(100);
    digitalWrite(P_LED_XBEE_RX, HIGH);
    delay(100);

    digitalWrite(P_LED_STATUS, LOW);
    delay(100);
    digitalWrite(P_LED_FLTHY_TX, LOW);
    delay(100);
    digitalWrite(P_LED_DOME_TX, LOW);
    delay(100);
    digitalWrite(P_LED_DOME_RX, LOW);
    delay(100);
    digitalWrite(P_LED_BODY_TX, LOW);
    delay(100);
    digitalWrite(P_LED_BODY_RX, LOW);
    delay(100);
    digitalWrite(P_LED_XBEE_TX, LOW);
    delay(100);
    digitalWrite(P_LED_XBEE_RX, LOW);
    delay(100);

    return false;
}

void AstroCommsBase::loop()
{
    char SerialBuffer[256];

    // Heartbeat
    if ((millis() - HeartBeatMillis) > HEARTBEAT_MILLIS)
    {
        toggleHeartBeat();
        HeartBeatMillis = millis();
    }

    checkSerialLEDs();

    if (SerialDome.available())
    {
        SerialDome.readBytes(SerialBuffer, SerialDome.available());
        digitalWrite(P_LED_DOME_RX, HIGH);
        LED_DOME_RX_Millis = millis();
        Serial.print("D");
    }
    if (SerialBody.available())
    {
        SerialBody.readBytes(SerialBuffer, SerialBody.available());
        digitalWrite(P_LED_BODY_RX, HIGH);
        LED_BODY_RX_Millis = millis();
        Serial.print("B");
    }
    if (SerialXBee.available())
    {
        SerialXBee.readBytes(SerialBuffer, SerialXBee.available());
        digitalWrite(P_LED_XBEE_RX, HIGH);
        LED_XBEE_RX_Millis = millis();
        Serial.print("X");
    }
}

void AstroCommsBase::toggleHeartBeat()
{
    if (HeartBeatStatus == LOW)
      HeartBeatStatus = HIGH;
    else
      HeartBeatStatus = LOW;

    digitalWrite(P_LED_STATUS, HeartBeatStatus);
}

void AstroCommsBase::checkSerialLEDs()
{
    checkSerialLED(P_LED_FLTHY_TX,  LED_FLTHY_TX_Millis);
    checkSerialLED(P_LED_DOME_TX,   LED_DOME_TX_Millis);
    checkSerialLED(P_LED_DOME_RX,   LED_DOME_RX_Millis);
    checkSerialLED(P_LED_BODY_TX,   LED_BODY_TX_Millis);
    checkSerialLED(P_LED_BODY_RX,   LED_BODY_RX_Millis);
    checkSerialLED(P_LED_XBEE_TX,   LED_XBEE_TX_Millis);
    checkSerialLED(P_LED_XBEE_RX,   LED_XBEE_RX_Millis);    
}

void AstroCommsBase::checkSerialLED(const uint8_t pin, unsigned long & ulMillis)
{
    if (ulMillis > 0)
    {
        if ((millis() - ulMillis) > SERIAL_BLINK_MILLIS)
        {
            digitalWrite(pin, LOW);
            ulMillis = 0;
        }        
    }
}


