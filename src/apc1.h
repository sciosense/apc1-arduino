#ifndef SCIOSENSE_APC1_H
#define SCIOSENSE_APC1_H

#include <stdint.h>

#include <Arduino.h>
#include <Stream.h>

#include "lib/apc1/ScioSense_Apc1.h"
#include "lib/io/ScioSense_IOInterface_Arduino_I2C.h"
#include "lib/io/ScioSense_IOInterface_Arduino_Serial.h"

class APC1 : public ScioSense_Apc1
{
public:
    APC1();
    virtual ~APC1();

public:
    inline void begin(TwoWire* wire, const uint8_t address = 0x12);     // Connnects to APC1 using the given TwoWire object and address
    inline void begin(Stream* serial);                                  // Connnects to APC1 using the given Stream(Serial) object
    inline bool init();                                                 // Resets the device to IDLE and reads PartID and FirmwareVersion
    bool isConnected();                                                 // Checks if the read firmware version is plausible; returns true, if so.

public:
    void enableDebugging(Stream& debugStream);                          // Enables the debug log. The output is written to the given debugStream
    void disableDebugging();                                            // Stops the debug log if enabled. Does nothing otherwise.

public:
    inline void clear();                                                // Clears IO buffers of the Stream device
    inline void reset();                                                // Resets the APC1 to default values
    inline Result update();                                             // Reads measurement data; Automaticcaly calls "RequestMeasurement" if in passive mode;
    inline bool setOperatingMode(const Apc1_OperatingMode& mode);       // Toggle between idle and measurement mode
    inline bool setMeasurementMode(const Apc1_MeasurementMode& mode);   // Toggle between active and passive measurement mode

public:
    inline uint16_t getPM_1_0();                                        // returns PM1.0 mass concentration
    inline uint16_t getPM_2_5();                                        // returns PM2.5 mass concentration
    inline uint16_t getPM_10();                                         // returns PM10  mass concentration
    inline uint16_t getPMInAir_1_0();                                   // returns PM1.0 mass concentration in atmospheric environment
    inline uint16_t getPMInAir_2_5();                                   // returns PM2.5 mass concentration in atmospheric environment
    inline uint16_t getPMInAir_10();                                    // returns PM10  mass concentration in atmospheric environment
    inline uint16_t getNoParticles_0_3();                               // returns Number of particles with diameter > 0.3μm in 0.1L of air
    inline uint16_t getNoParticles_0_5();                               // returns Number of particles with diameter > 0.5μm in 0.1L of air.
    inline uint16_t getNoParticles_1_0();                               // returns Number of particles with diameter > 1.0μm in 0.1L of air.
    inline uint16_t getNoParticles_2_5();                               // returns Number of particles with diameter > 2.5μm in 0.1L of air.
    inline uint16_t getNoParticles_5_0();                               // returns Number of particles with diameter > 5.0μm in 0.1L of air.
    inline uint16_t getNoParticles_10();                                // returns Number of particles with diameter >  10μm in 0.1L of air.
    inline uint16_t getTVOC();                                          // returns TVOC output
    inline uint16_t getECO2();                                          // returns Output in ppm CO2 equivalents
    inline uint16_t getNO2();                                           // Reserved
    inline float    getCompT();                                         // returns Compensated temperature (see datasheet)
    inline float    getCompRH();                                        // returns Compensated humidity (see datasheet)
    inline float    getRawT();                                          // returns Uncompensated temperature
    inline float    getRawRH();                                         // returns Uncompensated humidity
    inline uint32_t getRS0();                                           // returns Gas sensor 0 raw resistance value
    inline uint32_t getRS1();                                           // returns Gas sensor 1 raw resistance value
    inline uint32_t getRS2();                                           // returns Gas sensor 2 raw resistance value
    inline uint32_t getRS3();                                           // returns Gas sensor 3 raw resistance value
    inline AirQualityIndex_UBA getAQI();                                // returns Air Quality Index according to UBA Classification of TVOC value
    inline uint16_t getFirmwareVersion();                               // returns Firmware version
    inline Apc1_ErrorCode getError();                                   // returns Error codes (see datasheet)

protected:
    ScioSense_Arduino_I2c_Config        i2cConfig;
    ScioSense_Arduino_Serial_Config     serialConfig;

private:
    Stream* debugStream;
};

#include "apc1.inl.h"

#endif //SCIOSENSE_APC1_H