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

    pinMode(P_JUMPER_MONITOR, INPUT_PULLUP);

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
    uint8_t SerialBuffer[SERIAL_BUFFER_LEN];
    size_t DataLen = 0;

    // Heartbeat
    if ((millis() - HeartBeatMillis) > HEARTBEAT_MILLIS)
    {
        toggleHeartBeat();
        HeartBeatMillis = millis();
    }

    checkSerialLEDs();

    if (SerialDome.available())
    {
        DataLen =  SerialDome.available();
        SerialDome.readBytes(SerialBuffer, DataLen);
        if (digitalRead(P_JUMPER_MONITOR) != LOW)
            digitalWrite(P_LED_DOME_RX, HIGH);
        LED_DOME_RX_Millis = millis();

        writeDome(SerialBuffer, DataLen);
        writeFlthy(SerialBuffer, DataLen);
    }
    if (SerialBody.available())
    {
        DataLen =  SerialBody.available();
        SerialBody.readBytes(SerialBuffer, DataLen);
        if (digitalRead(P_JUMPER_MONITOR) != LOW)
            digitalWrite(P_LED_BODY_RX, HIGH);
        LED_BODY_RX_Millis = millis();

        writeBody(SerialBuffer, DataLen);
        writeFlthy(SerialBuffer, DataLen);
    }
    if (SerialXBee.available())
    {
        DataLen = SerialXBee.available();
        SerialXBee.readBytes(SerialBuffer, DataLen);
        if (digitalRead(P_JUMPER_MONITOR) != LOW)
            digitalWrite(P_LED_XBEE_RX, HIGH);
        LED_XBEE_RX_Millis = millis();

        writeDome(SerialBuffer, DataLen);
        writeFlthy(SerialBuffer, DataLen);
    }

    // Debug
    if (Serial.available())
    {
        DataLen = Serial.available();
        Serial.readBytes(SerialBuffer, DataLen);

        writeDome(SerialBuffer, DataLen);
        writeFlthy(SerialBuffer, DataLen);
    }
}

void AstroCommsBase::toggleHeartBeat()
{
    if (HeartBeatStatus == LOW)
      HeartBeatStatus = HIGH;
    else
      HeartBeatStatus = LOW;

    if (digitalRead(P_JUMPER_MONITOR) != LOW)
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

void AstroCommsBase::writeDome(uint8_t* data, size_t data_len)
{
    SerialDome.write(data, data_len);
    if (digitalRead(P_JUMPER_MONITOR) != LOW)
    {
        digitalWrite(P_LED_DOME_TX, HIGH);
        LED_DOME_TX_Millis = millis();
    }
    SerialDome.flush();
}

void AstroCommsBase::writeBody(uint8_t* data, size_t data_len)
{
    SerialBody.write(data, data_len);
    if (digitalRead(P_JUMPER_MONITOR) != LOW)
    {
        digitalWrite(P_LED_BODY_TX, HIGH);
        LED_BODY_TX_Millis = millis();
    }
    SerialBody.flush();
}

void AstroCommsBase::writeFlthy(uint8_t* data, size_t data_len)
{
    SerialFlthy.write(data, data_len);
    if (digitalRead(P_JUMPER_MONITOR) != LOW)
    {
        digitalWrite(P_LED_FLTHY_TX, HIGH);
        LED_FLTHY_TX_Millis = millis();
    }
    SerialFlthy.flush();        
}

void AstroCommsBase::writeXBee(uint8_t* data, size_t data_len)
{
    SerialXBee.write(data, data_len);
    if (digitalRead(P_JUMPER_MONITOR) != LOW)
    {
        digitalWrite(P_LED_XBEE_TX, HIGH);
        LED_XBEE_TX_Millis = millis();
    }
    SerialXBee.flush();        
}


