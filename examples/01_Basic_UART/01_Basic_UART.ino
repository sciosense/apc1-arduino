/* **************************************************
*   
*   Example Code for running ScioSense APC1 on UART
*       tested with Arduino UNO and ESP32
*
*  **************************************************
*/

#include <Arduino.h>

#include <apc1.h>

using namespace ScioSense;

#define rxPin 16
#define txPin 17

APC1 apc1;

// If your board only has one hardware serial bus, use SoftwareSerial
#ifndef ESP32
  #include <SoftwareSerial.h>
  SoftwareSerial softwareSerial = SoftwareSerial(rxPin, txPin);
#endif

void setup()
{
    Serial.begin(9600);
    Serial.println("");

    // Enable Debug if needed
    //apc1.enableDebugging(Serial);

    // If your board supports a second hardware serial bus (like the ESP32)...
    #ifdef ESP32
      // ...then use Serial1 or Serial2
      Serial2.begin(9600, SERIAL_8N1, rxPin, txPin);
      apc1.begin(Serial2);
    #else
      // ...otherwise go for SoftwareSerial
      softwareSerial.begin(9600);
      apc1.begin(softwareSerial);
    #endif

    if (apc1.isConnected() == false)
    {
        Serial.println("Error -- The APC1 is not connected.");
        while(1);
    }

    Serial.print("APC1 FW version: ");
    Serial.println(apc1.getFirmwareVersion());
}

void loop()
{
    if (apc1.update() == APC1::Result::Ok)
    {
        Serial.print("PM1.0: ");
        Serial.println(apc1.getPM_1_0());

        Serial.print("PM2.5: ");
        Serial.println(apc1.getPM_2_5());

        Serial.print("PM10: ");
        Serial.println(apc1.getPM_10());

        Serial.print("PM1.0 in air: ");
        Serial.println(apc1.getPMInAir_1_0());

        Serial.print("PM2.5 in air: ");
        Serial.println(apc1.getPMInAir_2_5());

        Serial.print("PM10 in air: ");
        Serial.println(apc1.getPMInAir_10());

        Serial.print("# particles >0.3μm: ");
        Serial.println(apc1.getNoParticles_0_3());

        Serial.print("# particles >0.5μm: ");
        Serial.println(apc1.getNoParticles_0_5());

        Serial.print("# particles >1.0μm: ");
        Serial.println(apc1.getNoParticles_1_0());

        Serial.print("# particles >2.5μm: ");
        Serial.println(apc1.getNoParticles_2_5());

        Serial.print("# particles >5.0μm: ");
        Serial.println(apc1.getNoParticles_5_0());

        Serial.print("# particles >10μm: ");
        Serial.println(apc1.getNoParticles_10());

        Serial.print("TVOC: ");
        Serial.println(apc1.getTVOC());

        Serial.print("ECO2: ");
        Serial.println(apc1.getECO2());

        Serial.print("T-comp.: ");
        Serial.println(apc1.getCompT());

        Serial.print("RH-comp.: ");
        Serial.println(apc1.getCompRH());

        Serial.print("T-raw: ");
        Serial.println(apc1.getRawT());

        Serial.print("RH-raw: ");
        Serial.println(apc1.getRawRH());

        Serial.print("RS0: ");
        Serial.println(apc1.getRS0());

        Serial.print("RS1: ");
        Serial.println(apc1.getRS1());

        Serial.print("RS2: ");
        Serial.println(apc1.getRS2());

        Serial.print("RS3: ");
        Serial.println(apc1.getRS3());

        Serial.print("AQI: ");
        Serial.print(apc1.getAQIString());
        Serial.print(" ");
        Serial.println((uint8_t)apc1.getAQI());

        Serial.print("Error code: ");
        Serial.println((uint8_t)apc1.getError(), BIN);

        Serial.println("-----------------------");
    }

    delay(1000);
}