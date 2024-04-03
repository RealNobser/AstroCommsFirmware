#include <Arduino.h>
#include <AstroComms.h>
#include <SoftwareSerial.h>

#include <config.h>

AstroComms* AstroComms_Module = nullptr;
SoftwareSerial SerialBody(P_BODY_RX, P_BODY_TX);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(SERIAL_BAUD);
  while(!Serial);
  #ifdef DEBUG_MSG
  Serial.println("Debug Serial fine.");
  #endif

  SerialDome.begin(SERIAL_DOME_BAUD);
  while(!SerialDome);
  #ifdef DEBUG_MSG
  Serial.println("Dome  Serial fine.");
  SerialDome.println("Dome");
  #endif

  SerialXBee.begin(SERIAL_XBEE_BAUD);
  while (!SerialXBee);
  #ifdef DEBUG_MSG
  Serial.println("XBee  Serial fine.");
  SerialXBee.println("XBee");
  #endif

  SerialFlthy.begin(SERIAL_FLTHY_BAUD);
  while(!SerialFlthy);
  #ifdef DEBUG_MSG
  Serial.println("Flthy Serial fine.");
  SerialFlthy.println("Flthy");
  #endif

  SerialBody.begin(SERIAL_BODY_BAUD);
  while(!SerialBody);
  #ifdef DEBUG_MSG
  Serial.println("Body  Serial fine.");
  SerialBody.println("Body");
  #endif

  AstroComms_Module = new AstroComms();

  if (AstroComms_Module == nullptr)
    return;

  char product[256];
  sprintf(product, "%s\r\nVersion %s\r\n\r\n", AstroComms_Module->getProductName(), VERSION);
  Serial.println();
  Serial.write(product);

  Serial.println("Enter /help for usage.");

  AstroComms_Module->init();  
}

void loop() {
  // put your main code here, to run repeatedly:
  AstroComms_Module->loop();
}
