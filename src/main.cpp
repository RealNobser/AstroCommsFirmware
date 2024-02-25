#include <Arduino.h>
#include <AstroCommsBase.h>
#include <SoftwareSerial.h>

#include <config.h>

AstroCommsBase* AstroComms = nullptr;
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

  AstroComms = new AstroCommsBase();

  if (AstroComms == nullptr)
    return;

  char product[256];
  sprintf(product, "%s\r\nVersion %s\r\n\r\n", AstroComms->getProductName(), VERSION);
  Serial.println();
  Serial.write(product);

  AstroComms->init();  
}

void loop() {
  // put your main code here, to run repeatedly:
  AstroComms->loop();
}
