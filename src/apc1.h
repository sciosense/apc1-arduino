#ifndef SCIOSENSE_APC1_H
#define SCIOSENSE_APC1_H

#include <stdint.h>

#include <Arduino.h>
#include <Stream.h>
#include <SoftwareSerial.h>

#include "utils.h"

namespace ScioSense
{
    class APC1
    {
    public:
        enum class AirQualityIndex_UBA : uint8_t
        {
            Unknown	    = 0,

            Excellent   = 1,
            Good        = 2,
            Moderate    = 3,
            Poor        = 4,
            Unhealthy   = 5,
        };

        enum class ErrorCode : uint8_t
        {
            Default             = 0,        // No Error
            TooManyFanRestarts  = 1 << 0,   // Too many Fan restarts
            FanSpeedTooLow      = 1 << 1,   // Fan-speed low
            Photodiode          = 1 << 2,   // Photodiode
            FanStartUpError     = 1 << 3,   // Fan stopped
            Laser               = 1 << 4,   // Laser
            VOC                 = 1 << 5,   // VOC Sensor
            RHT                 = 1 << 6,   // Temperature and humidity sensor
        };

        // Output data structure adresses for the measurement data struct (64 bytes) and sensor info struct (23 bytes)
        enum class ResultAddress : uint8_t
        {
            // Measurement data struct
            PM_1_0          = 0x04,         // PM1.0 mass concentration
            PM_2_5          = 0x06,         // PM2.5 mass concentration
            PM_10           = 0x08,         // PM10  mass concentration
            PMInAir_1_0     = 0x0A,         // PM1.0 mass concentration in atmospheric environment
            PMInAir_2_5     = 0x0C,         // PM2.5 mass concentration in atmospheric environment
            PMInAir_10      = 0x0E,         // PM10  mass concentration in atmospheric environment
            NoParticles_0_3 = 0x10,         // Number of particles with diameter > 0.3μm in 0.1L of air
            NoParticles_0_5 = 0x12,         // Number of particles with diameter > 0.5μm in 0.1L of air
            NoParticles_1_0 = 0x14,         // Number of particles with diameter > 1.0μm in 0.1L of air
            NoParticles_2_5 = 0x16,         // Number of particles with diameter > 2.5μm in 0.1L of air
            NoParticles_5_0 = 0x18,         // Number of particles with diameter > 5.0μm in 0.1L of air
            NoParticles_10  = 0x1A,         // Number of particles with diameter >  10μm in 0.1L of air
            TVOC            = 0x1C,         // TVOC output
            ECO2            = 0x1E,         // Output in ppm CO2 equivalents
            NO2             = 0x20,         // Reserved
            T_comp          = 0x22,         // Compensated temperature (see datasheet)
            RH_comp         = 0x24,         // Compensated humidity (see datasheet)
            T_raw           = 0x26,         // Uncompensated temperature
            RH_raw          = 0x28,         // Uncompensated humidity
            RS0             = 0x2A,         // Gas sensor 0 raw resistance value
            RS1             = 0x2E,         // Gas sensor 1 raw resistance value
            RS2             = 0x32,         // Gas sensor 2 raw resistance value
            RS3             = 0x36,         // Gas sensor 3 raw resistance value
            AQI             = 0x3A,         // Air Quality Index according to UBA Classification of TVOC value
            FirmwareVersion = 0x3C,         // Firmware version
            ErrorCode       = 0x3D,         // Error codes (see datasheet)
            CheckSumH       = 0x3E,         // Frame (0x00 – 0x3D) checksum High byte
            CheckSumL       = 0x3F,         // Frame (0x00 – 0x3D) checksum Low byte

            // Sensor data struct
            SensorType              = 0x04,
            SensorUID               = 0x0A,
            SensorFirmwareVersion   = 0x13,
        };

        // Measurement Modes
        enum class MeasurementMode : uint16_t
        {
            Passive = 0x0000,   // Device sends 64-byte structure on request (Default).
            Active  = 0x0001,   // Device sends 64-byte structure every second
        };

        // Operating Modes
        enum class OperatingMode : uint16_t
        {
            Idle     = 0x0000,  // Idle mode; No Measurements; Fan Off
            Standard = 0x0001,  // Measurement Mode; Fan On
        };

        typedef const uint8_t Command[7];

        // Command protocol structure values
        enum class CommandAddress : uint8_t
        {
            StartByte1          = 0x42,
            StartByte2          = 0x4D,

            MeasurementMode     = 0xE1, // Toggle measurement result communication mode
            RequestMeasurement  = 0xE2, // Once in passive mode, this command requests the device to send a 64-byte data structure containing the latest measurement results
            OperationMode       = 0xE4, // Toggle between Idle and Measurement mode (Default)
            ReadSensorVersion   = 0xE9, // Read module type, ID & FW version
        };

        // Command response addresses ( answer protocol structure )
        enum class CommandResponse : uint8_t
        {
            DefaultLength            = 8,    // Command response default length
            SensorVersionLength      = 23,   // Command response length for "ReadSensorVersion" command
            MeasurementLength        = 64,   // Command response length for "RequestMeasurement" command
            MeasurementPayloadLength = 60,   // Measurement data struct payload length

            StartByteAddress1        = 0,
            StartByteAddress2        = 1,
            FrameLengthAddressH      = 2,    // Command response length high byte
            FrameLengthAddressL      = 3,    // Command response length low byte
            CommandAddress           = 4,    // Command address
            DataAddress              = 5,    // Data start address
        };

        enum class Result : uint8_t
        {
            NotAllowed              = 5, // The requested command is not allowed, because the device is in IDLE or DEEP_SLEEP mode.
            IOError                 = 4, // There was an IO communication error, read/write the stream failed.
            ChecksumError           = 3, // The value was read, but the checksum over the payload (valid and data) does not match.
            Invalid                 = 2, // The value was read, but the data is invalid.
            Ok                      = 1  // All OK; The value was read, the checksum matches, and data is valid.
        };

    public:
        APC1();
        virtual ~APC1();

    public:
        bool begin(Stream& serial);                             // Connnects to APC1 using the given Stream(Serial) object and reads Type/UID/Version; returns the result of isConnected()
        bool isConnected();                                     // Checks if the read firmware version is plausible; returns true, if so.

    public:
        void enableDebugging(Stream& debugStream);              // Enables the debug log. The output is written to the given debugStream
        void disableDebugging();                                // Stops the debug log if enabled. Does nothing otherwise.

    public:
        void clear();                                           // Clears IO buffers of the Stream device
        void reset();                                           // Resets the APC1 to default values
        bool valid();                                           // Checks the data integrity of the last measurement (update); returns true if measure data are valid.
        Result update();                                        // Reads measurement data; Automaticcaly calls "RequestMeasurement" if in passive mode;
        bool setOperatingMode(const OperatingMode& mode);       // Toggle between idle and measurement mode
        bool setMeasurementMode(const MeasurementMode& mode);   // Toggle between active and passive measurement mode

    public:
        uint16_t getPM_1_0();                                   // returns PM1.0 mass concentration
        uint16_t getPM_2_5();                                   // returns PM2.5 mass concentration
        uint16_t getPM_10();                                    // returns PM10  mass concentration
        uint16_t getPMInAir_1_0();                              // returns PM1.0 mass concentration in atmospheric environment
        uint16_t getPMInAir_2_5();                              // returns PM2.5 mass concentration in atmospheric environment
        uint16_t getPMInAir_10();                               // returns PM10  mass concentration in atmospheric environment
        uint16_t getNoParticles_0_3();                          // returns Number of particles with diameter > 0.3μm in 0.1L of air
        uint16_t getNoParticles_0_5();                          // returns Number of particles with diameter > 0.5μm in 0.1L of air.
        uint16_t getNoParticles_1_0();                          // returns Number of particles with diameter > 1.0μm in 0.1L of air.
        uint16_t getNoParticles_2_5();                          // returns Number of particles with diameter > 2.5μm in 0.1L of air.
        uint16_t getNoParticles_5_0();                          // returns Number of particles with diameter > 5.0μm in 0.1L of air.
        uint16_t getNoParticles_10();                           // returns Number of particles with diameter >  10μm in 0.1L of air.
        uint16_t getTVOC();                                     // returns TVOC output
        uint16_t getECO2();                                     // returns Output in ppm CO2 equivalents
        uint16_t getNO2();                                      // Reserved
        float    getCompT();                                    // returns Compensated temperature (see datasheet)
        float    getCompRH();                                   // returns Compensated humidity (see datasheet)
        float    getRawT();                                     // returns Uncompensated temperature
        float    getRawRH();                                    // returns Uncompensated humidity
        uint32_t getRS0();                                      // returns Gas sensor 0 raw resistance value
        uint32_t getRS1();                                      // returns Gas sensor 1 raw resistance value
        uint32_t getRS2();                                      // returns Gas sensor 2 raw resistance value
        uint32_t getRS3();                                      // returns Gas sensor 3 raw resistance value
        AirQualityIndex_UBA getAQI();                           // returns Air Quality Index according to UBA Classification of TVOC value
        uint16_t getFirmwareVersion();                          // returns Firmware version
        ErrorCode getError();                                   // returns Error codes (see datasheet)

        const char* getAQIString();

    private:
        Result read(uint8_t* buf, const CommandResponse& size);
        Result invoke(Command& command, uint8_t* buf = nullptr, const CommandResponse& size = CommandResponse::DefaultLength);
        Result readSensorVersion();
        Result checkData(const uint8_t* data, const CommandResponse& size);
        Result checkCommandResponse(Command& command, const uint8_t* data, const CommandResponse& size);
        Result checkMeasurementData(const uint8_t* data);
        template<class T> T getValueOf(const ResultAddress& addr)
        {
            return ScioSense::Utils::endian::bigTo<T>(measurementData + (uint8_t)addr);
        }

    private:
        const char* debugPrefix = "APC1 debug -- ";
        const char* toString(const Result& result);
        const char* toString(const AirQualityIndex_UBA& aqi);
        void debug(const char* msg, const uint8_t* data, const size_t& size = 0);
        void debug(const char* msg, const Result& res);
        void debug(const char* msg);
        template<class T> void debug(const char* msg, T data);

    private:
        bool dataValid;
        uint8_t delimiter;
        uint8_t type[7];
        uint8_t measurementData[(uint8_t)CommandResponse::MeasurementLength];
        uint16_t fwVersion;
        uint64_t serialNumber;

    private:
        Stream* serial;
        bool deleteSerial;
        Stream* debugStream;

    private:
        OperatingMode operatingMode;
        MeasurementMode measurementMode;
    };
}

#endif //SCIOSENSE_APC1_H