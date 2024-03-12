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

    // Flthy Timeout
    if (FlthyDuration != 0)
    {
        if ((millis() - FlthyMillis) > FlthyDuration)
        {
            clearFlthy();
            FlthyDuration = 0;
        }
    }

    // Check LEDs
    checkSerialLEDs();

    // Check Communication
    if (SerialDome.available())
    {
        char c = SerialDome.read();
        if (c == '\n')
            return;
        SerialDomeBuffer[DomeBufferIndex++] = c;
        if ((c == '\r') || (DomeBufferIndex == SERIAL_BUFFER_LEN))   // Command complete or buffer full
        {
            SerialDomeBuffer[DomeBufferIndex-1] = 0x00; // ensure proper termination
            if(DomeBufferIndex>1)
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
        if (c == '\n')
            return;
        SerialBodyBuffer[BodyBufferIndex++] = c;
        if ((c == '\r') || (BodyBufferIndex == SERIAL_BUFFER_LEN))   // Command complete or buffer full
        {
            SerialBodyBuffer[BodyBufferIndex-1] = 0x00; // ensure proper termination
            if(BodyBufferIndex>1)
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
        if (c == '\n')
            return;
        SerialXBeeBuffer[XBeeBufferIndex++] = c;
        if ((c == '\r') || (XBeeBufferIndex == SERIAL_BUFFER_LEN))   // Command complete or buffer full
        {
            SerialXBeeBuffer[XBeeBufferIndex-1] = 0x00; // ensure proper termination
            if(XBeeBufferIndex>1)
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
        if (c == '\n')
            return;
        SerialDebugBuffer[DebugBufferIndex++] = c;
        if ((c == '\r') || (DebugBufferIndex == SERIAL_BUFFER_LEN))   // Command complete or buffer full
        {
            SerialDebugBuffer[DebugBufferIndex-1] = 0x00; // ensure proper termination
            if(DebugBufferIndex>1)
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

        // Clear each index
        for(uint8_t i=0; i<=MAX_SEQUENCE_NR; i++)
            Storage.freeSequenceMap(i);

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
        Storage.setSequenceMap(11,"A0971",  (uint32_t)0);
        Storage.setSequenceMap(12,"A0971",  (uint32_t)0);
        Storage.setSequenceMap(13,"A0971",  (uint32_t)0);
        Storage.setSequenceMap(14,"A0971",  (uint32_t)0);
        Storage.setSequenceMap(15,"A0051",  (uint32_t)4000);

        Storage.setSequenceMap(51,"A0051",  (uint32_t)4000);
        Storage.setSequenceMap(52,"A0059",  (uint32_t)4000);
        Storage.setSequenceMap(53,"A0059",  (uint32_t)4000);
        Storage.setSequenceMap(54,"A0059",  (uint32_t)6000);
        Storage.setSequenceMap(55,"A00387", (uint32_t)15500);
        Storage.setSequenceMap(56,"A00387", (uint32_t)10000);
        Storage.setSequenceMap(57,"A00387", (uint32_t)40000);

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

void AstroCommsBase::clearFlthy()
{
    #ifdef DEBUG_MSG
    Serial.println("Setting Flthys to default state");
    #endif

    writeFlthy("A0971\r");

    #ifdef DEBUG_MSG
    Serial.println();
    #endif
}

void AstroCommsBase::dispatchDomeCommand(const char* command)
{
    char data[SERIAL_BUFFER_LEN];
    char flthy[MAX_FLTHY_CMD_SIZE+1];

    #ifdef DEBUG_MSG
    char message[256];
    sprintf(message, "Dome Command:  %s", command);
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
        case ':':   // Possible Sequence
            writeDome(data);
            processFlthySequence(command);
        break;
        default:
            writeDome(data);
        break;
    }
    #ifdef DEBUG_MSG
    Serial.println();
    #endif
}

void AstroCommsBase::dispatchBodyCommand(const char* command)
{
    char data[SERIAL_BUFFER_LEN];
    char flthy[MAX_FLTHY_CMD_SIZE+1];

    #ifdef DEBUG_MSG
    char message[SERIAL_BUFFER_LEN];
    sprintf(message, "Body Command:  %s", command);
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
        case ':':   // Possible Sequence
            writeBody(data);
            processFlthySequence(command);
        break;        
        default:
            writeBody(data);
        break;
    }
    #ifdef DEBUG_MSG
    Serial.println();
    #endif
}

void AstroCommsBase::dispatchXBeeCommand(const char* command)
{
    char data[SERIAL_BUFFER_LEN];
    char flthy[MAX_FLTHY_CMD_SIZE+1];

    #ifdef DEBUG_MSG
    char message[256];
    sprintf(message, "XBee Command:  %s", command);
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
        case ':':   // Possible Sequence
            writeDome(data);
            processFlthySequence(command);
        break;        
        default:
            writeDome(data);
        break;
    }
    #ifdef DEBUG_MSG
    Serial.println();
    #endif
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
            writeFlthy(data+1);
        break;
        case ':':   // Possible Sequence
            writeDome(data);
            processFlthySequence(command);
        break;        
        case '/':
            processDebugCommand(command);
        break;
        default:
            writeDome(data);
            break;
    }
    #ifdef DEBUG_MSG
    Serial.println();
    #endif
}

void AstroCommsBase::processFlthySequence(const char* command)
{
    if (strlen(command) == 5)
    {
        if(strncmp(command, ":SE", 3) == 0)
        {
            char s_index[3];
            char flthy_cmd[MAX_FLTHY_CMD_SIZE+1];
            uint32_t flthy_rst_time = 0;

            memset(s_index, 0x00, 0x03);
            memset(flthy_cmd, 0x00, MAX_FLTHY_CMD_SIZE+1);
            strncpy(s_index, command+3, 2);
            Storage.getSequenceMap(atoi(s_index), flthy_cmd, flthy_rst_time);
            if (strlen(flthy_cmd)!= 0)
            {
                writeFlthy(flthy_cmd);
                FlthyMillis = millis();
                FlthyDuration = flthy_rst_time;
            }
        }
    }

}

void AstroCommsBase::printUsage()
{
    Serial.println("\
        /read all   - reads all commands data\r\n\
        /read xx    - reads command data to send to Flthys for a given index from 00 to 99. Example: /read 01\r\n\
        /write xx command milliseconds - writes command data to Flthys for a given index from 00 to 99. Example: /write 01 #A0971 10000\r\n\
        /free xx    - frees command data for a given indes from 99 to 99. Example: /free 01\r\n\
        /reset      - reset command table to factory\r\n\
    ");
}

/*

/help - usage

(unsupported) /commands - display the full list of commands for Marcduino, Holo Projectors servos and leds, Teeces and sounds

/read all - reads all commands data

/read xx - reads command data to send to Flthys for a given index from 00 to 22. Example: /read 01

/write xx command milliseconds - writes command data to Flthys for a given index from 00 to 22. Example: /write 01 #A0971 10000

/free xx - frees command data

/reset - reset command table to factory

*/

void AstroCommsBase::processDebugCommand(const char* command)
{
    char tokens[SERIAL_BUFFER_LEN];
    char* token = NULL;

    #ifdef DEBUG_MSG
    Serial.print("processDebugCommand: ");
    Serial.println(command);
    #endif

    if (strncmp(command, "/read", 5) == 0)
    {
        strcpy(tokens, command);
        token = strtok(tokens, " ");    // Command
        token = strtok(NULL, " ");      // Parameter Index
        if (token != NULL)
        {
            if (strcmp(token, "all") == 0)
            {
                for(uint8_t index = 0; index <= MAX_SEQUENCE_NR; index++)
                    printFlthyCmd(index);
            }
            else
            {
               uint8_t index = atoi(token);
               printFlthyCmd(index);
            }
        }
    }
    else if (strncmp(command, "/write", 6) == 0)
    {
        strcpy(tokens, command);
        token = strtok(tokens, " ");    // Command
        token = strtok(NULL, " ");      // Parameter Index
        if (token != NULL)
        {
            char s_index[SERIAL_BUFFER_LEN];
            strcpy(s_index, token);
            token = strtok(NULL, " ");      // Parameter Command
            if (token != NULL)
            {
                char s_command[SERIAL_BUFFER_LEN];
                strcpy(s_command, token);
                if (strlen(s_command)> MAX_FLTHY_CMD_SIZE)
                    return;
                token = strtok(NULL, " ");  // Parameter Millis
                if (token != NULL)
                {
                    char s_millis[SERIAL_BUFFER_LEN];
                    strcpy(s_millis, token);
                    Storage.setSequenceMap(atoi(s_index), s_command, atol(s_millis));
                    printFlthyCmd(atoi(s_index));
                }
            }
        }
    }
    else if (strncmp(command, "/free", 5) == 0)
    {
        strcpy(tokens, command);
        token = strtok(tokens, " ");    // Command
        token = strtok(NULL, " ");      // Parameter

        if (token != NULL)
        {
            uint8_t index = atoi(token);
            Storage.freeSequenceMap(index);
            printFlthyCmd(index);
        }
    }
    else if (strcmp(command, "/reset") == 0)
    {
        checkEEPROM(true);
    }
    else if (strcmp(command, "/help") == 0)
    {
        printUsage();
    }
}

void AstroCommsBase::printFlthyCmd(const uint8_t index)
{
    char message[SERIAL_BUFFER_LEN];
    char flthy_cmd[MAX_FLTHY_CMD_SIZE+1];
    uint32_t flthy_rst_time = 0;

    if (index > MAX_SEQUENCE_NR)
    {
        Serial.println("Invalid Index.");
        return;
    }

    memset(flthy_cmd, 0x00, MAX_FLTHY_CMD_SIZE+1);
    Storage.getSequenceMap(index, flthy_cmd, flthy_rst_time);

    if (strlen(flthy_cmd) != 0)
        sprintf(message, "Command number: %d\r\nMarcduino command >>> :SE%02d\r\nFlthy command >>> %s\r\nFlthy reset time >>> %lu\r\n", index, index, flthy_cmd, flthy_rst_time );
    else
        sprintf(message, "Command number: %d\r\nMarcduino command >>> :SE%02d\r\nFlthy command >>> %s\r\nFlthy reset time >>> %lu\r\n", index, index, "<empty>", flthy_rst_time );

    Serial.println(message);
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
    #ifdef DEBUG_MSG
    Serial.print("Sent to Dome:  ");
    Serial.println(data);
    #endif

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
    #ifdef DEBUG_MSG
    Serial.print("Sent to Body:  ");
    Serial.println(data);
    #endif

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
    #ifdef DEBUG_MSG
    Serial.print("Sent to Flthy: ");
    Serial.println(data);
    #endif

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
    #ifdef DEBUG_MSG
    Serial.print("Sent to XBee:  ");
    Serial.println(data);
    #endif

    SerialXBee.print(data);
    if (digitalRead(P_JUMPER_MONITOR) != LOW)
    {
        digitalWrite(P_LED_XBEE_TX, HIGH);
        LED_XBEE_TX_Millis = millis();
    }
    SerialXBee.flush();        
}
