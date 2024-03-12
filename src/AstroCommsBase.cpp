#include <AstroCommsBase.h>
#include <SoftwareSerial.h>
#include <config.h>

extern SoftwareSerial SerialBody;

AstroCommsBase::AstroCommsBase()
{
    HeartBeatMillis     = millis();

    memset(SerialDomeBuffer, 0x00, SERIAL_BUFFER_LEN);
    memset(SerialBodyBuffer, 0x00, SERIAL_BUFFER_LEN);
    memset(SerialXBeeBuffer, 0x00, SERIAL_BUFFER_LEN);
    memset(SerialDebugBuffer, 0x00, SERIAL_BUFFER_LEN);
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

    checkEEPROM();

    return false;
}

void AstroCommsBase::loop()
{
    // Heartbeat
    if ((millis() - HeartBeatMillis) > HEARTBEAT_MILLIS)
    {
        toggleHeartBeat();
        HeartBeatMillis = millis();
    }

    checkSerialLEDs();

    if (SerialDome.available())
    {
        char c = SerialDome.read();
        SerialDomeBuffer[DomeBufferIndex++] = c;
        if ((c == '\r') || (DomeBufferIndex == SERIAL_BUFFER_LEN))   // Command complete or buffer full
        {
            SerialDomeBuffer[DomeBufferIndex-1] = 0x00; // ensure proper termination
            dispatchDomeCommand(SerialDomeBuffer);
            memset(SerialDomeBuffer, 0x00, SERIAL_BUFFER_LEN);
            DomeBufferIndex = 0;
        }

        if (digitalRead(P_JUMPER_MONITOR) != LOW)
            digitalWrite(P_LED_DOME_RX, HIGH);
        LED_DOME_RX_Millis = millis();        
    }
    if (SerialBody.available())
    {
        char c = SerialBody.read();
        SerialBodyBuffer[BodyBufferIndex++] = c;
        if ((c == '\r') || (BodyBufferIndex == SERIAL_BUFFER_LEN))   // Command complete or buffer full
        {
            SerialBodyBuffer[BodyBufferIndex-1] = 0x00; // ensure proper termination
            dispatchBodyCommand(SerialBodyBuffer);
            memset(SerialBodyBuffer, 0x00, SERIAL_BUFFER_LEN);
            BodyBufferIndex = 0;
        }
        
        if (digitalRead(P_JUMPER_MONITOR) != LOW)
            digitalWrite(P_LED_BODY_RX, HIGH);
        LED_BODY_RX_Millis = millis();        
    }
    if (SerialXBee.available())
    {
        char c = SerialXBee.read();
        SerialXBeeBuffer[XBeeBufferIndex++] = c;
        if ((c == '\r') || (XBeeBufferIndex == SERIAL_BUFFER_LEN))   // Command complete or buffer full
        {
            SerialXBeeBuffer[XBeeBufferIndex-1] = 0x00; // ensure proper termination
            dispatchXBeeCommand(SerialXBeeBuffer);
            memset(SerialXBeeBuffer, 0x00, SERIAL_BUFFER_LEN);
            XBeeBufferIndex = 0;
        }

        if (digitalRead(P_JUMPER_MONITOR) != LOW)
            digitalWrite(P_LED_XBEE_RX, HIGH);
        LED_XBEE_RX_Millis = millis();
    }
    if (Serial.available()) // Debug Console
    {
        char c = Serial.read();
        SerialDebugBuffer[DebugBufferIndex++] = c;
        if ((c == '\r') || (DebugBufferIndex == SERIAL_BUFFER_LEN))   // Command complete or buffer full
        {
            SerialDebugBuffer[DebugBufferIndex-1] = 0x00; // ensure proper termination
            dispatchDebugCommand(SerialDebugBuffer);
            memset(SerialDebugBuffer, 0x00, SERIAL_BUFFER_LEN);
            DebugBufferIndex = 0;
        }
    }
}

void AstroCommsBase::checkEEPROM(const bool factoryReset /*= false*/)
{
    byte ConfigVersion = Storage.getConfigVersion();
    if ((ConfigVersion != CONFIG_VERSION) || (factoryReset == true))
    {
        #ifdef DEBUG_MSG
        Serial.println(F("Invalid Config Version. Storing defaults in EEPROM and restart."));
        #endif

        //
        // Init EEPROM values
        //
        char flthy[MAX_FLTHY_CMD_SIZE+1];
        memset(flthy, 0x00, MAX_FLTHY_CMD_SIZE + 1);
        uint16_t flthy_rst_time = 0;

        // Clear each index
        for(uint8_t i=0; i<0xff; i++)
            Storage.setSequenceMap(i, flthy, flthy_rst_time);

        // Init Presets
        Storage.setSequenceMap(0, "A0971",  (uint32_t)0);
        Storage.setSequenceMap(1, "A00359", (uint32_t)4000);
        Storage.setSequenceMap(2, "A0059",  (uint32_t)4000);
        Storage.setSequenceMap(3, "A00389", (uint32_t)4000);
        Storage.setSequenceMap(4, "A0059",  (uint32_t)5000);
        Storage.setSequenceMap(5, "A00358", (uint32_t)155000);
        Storage.setSequenceMap(6, "A0051",  (uint32_t)10000);
        Storage.setSequenceMap(7, "A00387", (uint32_t)40500);
        Storage.setSequenceMap(8, "A001",   (uint32_t)35500);
        Storage.setSequenceMap(9, "A00387", (uint32_t)245000);
        Storage.setSequenceMap(10,"A0971",  (uint32_t)0);


        Storage.setConfigVersion(CONFIG_VERSION);   // Final step before restart
        delay(500);
        resetFunc();
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


void AstroCommsBase::dispatchDomeCommand(const char* command)
{
    char data[SERIAL_BUFFER_LEN];
    char flthy[MAX_FLTHY_CMD_SIZE+1];

    #ifdef DEBUG_MSG
    char message[256];
    sprintf(message, "Dome Command: %s", command);
    Serial.println(message);
    #endif

    sprintf(data, "%s\r", command);
    memset(flthy, 0x00, MAX_FLTHY_CMD_SIZE + 1);

    switch (data[0])
    {
        case ';':   // Body prefix
            data[0] = ':';
            writeBody(data);
        break;
        case '+':   // Flthy prefix
            writeFlthy(data+1);
        break;
        default:
            writeDome(data);
            break;
    }
}

void AstroCommsBase::dispatchBodyCommand(const char* command)
{
    char data[SERIAL_BUFFER_LEN];
    char flthy[MAX_FLTHY_CMD_SIZE+1];

    #ifdef DEBUG_MSG
    char message[SERIAL_BUFFER_LEN];
    sprintf(message, "Body Command: %s", command);
    Serial.println(message);
    #endif

    sprintf(data, "%s\r", command);
    memset(flthy, 0x00, MAX_FLTHY_CMD_SIZE + 1);

    switch (data[0])
    {
        case ';':   // Body prefix
            data[0] = ':';
            writeBody(data);
        break;
        case '+':   // Flthy prefix
            writeFlthy(data+1);
        break;
        default:
            writeBody(data);
            break;
    }
}

void AstroCommsBase::dispatchXBeeCommand(const char* command)
{
    char data[SERIAL_BUFFER_LEN];
    char flthy[MAX_FLTHY_CMD_SIZE+1];

    #ifdef DEBUG_MSG
    char message[256];
    sprintf(message, "XBee Command: %s", command);
    Serial.println(message);
    #endif

    sprintf(data, "%s\r", command);
    memset(flthy, 0x00, MAX_FLTHY_CMD_SIZE + 1);

    switch (data[0])
    {
        case ';':   // Body prefix
            data[0] = ':';
            writeBody(data);
        break;
        case '+':   // Flthy prefix
            writeFlthy(data+1);
        break;
        default:
            writeDome(data);
            break;
    }

}

void AstroCommsBase::dispatchDebugCommand(const char* command)
{
    char data[SERIAL_BUFFER_LEN];
    char flthy[MAX_FLTHY_CMD_SIZE+1];

    #ifdef DEBUG_MSG
    char message[256];
    sprintf(message, "Debug Command: %s", command);
    Serial.println(message);
    #endif

    sprintf(data, "%s\r", command);
    memset(flthy, 0x00, MAX_FLTHY_CMD_SIZE + 1);

    switch (data[0])
    {
        case ';':   // Body prefix
            data[0] = ':';
            writeBody(data);
        break;
        case '+':   // Flthy prefix
            writeBody(data+1);
        break;
        default:
            writeDome(data);
            break;
    }

}

void AstroCommsBase::writeDome(const uint8_t* data, const size_t data_len)
{
    SerialDome.write(data, data_len);
    if (digitalRead(P_JUMPER_MONITOR) != LOW)
    {
        digitalWrite(P_LED_DOME_TX, HIGH);
        LED_DOME_TX_Millis = millis();
    }
    SerialDome.flush();
}

void AstroCommsBase::writeDome(const char* data)
{
    SerialDome.print(data);
    if (digitalRead(P_JUMPER_MONITOR) != LOW)
    {
        digitalWrite(P_LED_DOME_TX, HIGH);
        LED_DOME_TX_Millis = millis();
    }
    SerialDome.flush();
}

void AstroCommsBase::writeBody(const uint8_t* data, const size_t data_len)
{
    SerialBody.write(data, data_len);
    if (digitalRead(P_JUMPER_MONITOR) != LOW)
    {
        digitalWrite(P_LED_BODY_TX, HIGH);
        LED_BODY_TX_Millis = millis();
    }
    SerialBody.flush();
}

void AstroCommsBase::writeBody(const char* data)
{
    SerialBody.print(data);
    if (digitalRead(P_JUMPER_MONITOR) != LOW)
    {
        digitalWrite(P_LED_BODY_TX, HIGH);
        LED_BODY_TX_Millis = millis();
    }
    SerialBody.flush();
}

void AstroCommsBase::writeFlthy(const uint8_t* data, const size_t data_len)
{
    SerialFlthy.write(data, data_len);
    if (digitalRead(P_JUMPER_MONITOR) != LOW)
    {
        digitalWrite(P_LED_FLTHY_TX, HIGH);
        LED_FLTHY_TX_Millis = millis();
    }
    SerialFlthy.flush();        
}

void AstroCommsBase::writeFlthy(const char* data)
{
    SerialFlthy.print(data);
    if (digitalRead(P_JUMPER_MONITOR) != LOW)
    {
        digitalWrite(P_LED_FLTHY_TX, HIGH);
        LED_FLTHY_TX_Millis = millis();
    }
    SerialFlthy.flush();        
}

void AstroCommsBase::writeXBee(const uint8_t* data, const size_t data_len)
{
    SerialXBee.write(data, data_len);
    if (digitalRead(P_JUMPER_MONITOR) != LOW)
    {
        digitalWrite(P_LED_XBEE_TX, HIGH);
        LED_XBEE_TX_Millis = millis();
    }
    SerialXBee.flush();        
}

void AstroCommsBase::writeXBee(const char* data)
{
    SerialXBee.print(data);
    if (digitalRead(P_JUMPER_MONITOR) != LOW)
    {
        digitalWrite(P_LED_XBEE_TX, HIGH);
        LED_XBEE_TX_Millis = millis();
    }
    SerialXBee.flush();        
}
