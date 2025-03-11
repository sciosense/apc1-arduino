#ifndef SCIOSENSE_APC1_C_H
#define SCIOSENSE_APC1_C_H

#include "ScioSense_Apc1_defines.h"

#include <stdbool.h>
#include <inttypes.h>

typedef struct ScioSense_Apc1_IO
{
    Result  (*read)     (void* config, const uint16_t address, uint8_t* data, const size_t size);
    Result  (*write)    (void* config, const uint16_t address, uint8_t* data, const size_t size);
    Result  (*clear)    (void* config);
    void    (*wait)     (const uint32_t ms);
    Apc1_Protocol protocol;
    void* config;
} ScioSense_Apc1_IO;

typedef struct ScioSense_Apc1
{
    ScioSense_Apc1_IO       io;
    uint8_t                 measurementData[APC1_COMMAND_RESPONSE_MEASUREMENT_LENGTH];
    uint8_t                 moduleName[APC1_COMMAND_RESPONSE_MODULE_NAME_LENGTH+1];
    uint16_t                fwVersion;
    uint64_t                serialNumber;
    Apc1_OperatingMode      operatingMode;
    Apc1_MeasurementMode    measurementMode;

} ScioSense_Apc1;

static inline Result              Apc1_Reset                  (ScioSense_Apc1* apc1);                             // Resets the APC1 to default values
static inline Result              Apc1_Update                 (ScioSense_Apc1* apc1);                             // Reads measurement data; Automaticcaly calls "RequestMeasurement" if in passive mode;
static inline Result              Apc1_ReadSensorVersion      (ScioSense_Apc1* apc1);
static inline Result              Apc1_SetOperatingMode       (ScioSense_Apc1* apc1, const Apc1_OperatingMode mode);   // Toggle between idle and measurement mode
static inline Result              Apc1_SetMeasurementMode     (ScioSense_Apc1* apc1, const Apc1_MeasurementMode mode); // Toggle between active and passive measurement mode

static inline bool                Apc1_IsConnected            (ScioSense_Apc1* apc1);                             // Checks if the read firmware version is plausible; returns true, if so.
static inline uint16_t            Apc1_GetPM_1_0              (ScioSense_Apc1* apc1);                             // returns PM1.0 mass concentration
static inline uint16_t            Apc1_GetPM_2_5              (ScioSense_Apc1* apc1);                             // returns PM2.5 mass concentration
static inline uint16_t            Apc1_GetPM_10               (ScioSense_Apc1* apc1);                             // returns PM10  mass concentration
static inline uint16_t            Apc1_GetPMInAir_1_0         (ScioSense_Apc1* apc1);                             // returns PM1.0 mass concentration in atmospheric environment
static inline uint16_t            Apc1_GetPMInAir_2_5         (ScioSense_Apc1* apc1);                             // returns PM2.5 mass concentration in atmospheric environment
static inline uint16_t            Apc1_GetPMInAir_10          (ScioSense_Apc1* apc1);                             // returns PM10  mass concentration in atmospheric environment
static inline uint16_t            Apc1_GetNoParticles_0_3     (ScioSense_Apc1* apc1);                             // returns Number of particles with diameter > 0.3μm in 0.1L of air
static inline uint16_t            Apc1_GetNoParticles_0_5     (ScioSense_Apc1* apc1);                             // returns Number of particles with diameter > 0.5μm in 0.1L of air.
static inline uint16_t            Apc1_GetNoParticles_1_0     (ScioSense_Apc1* apc1);                             // returns Number of particles with diameter > 1.0μm in 0.1L of air.
static inline uint16_t            Apc1_GetNoParticles_2_5     (ScioSense_Apc1* apc1);                             // returns Number of particles with diameter > 2.5μm in 0.1L of air.
static inline uint16_t            Apc1_GetNoParticles_5_0     (ScioSense_Apc1* apc1);                             // returns Number of particles with diameter > 5.0μm in 0.1L of air.
static inline uint16_t            Apc1_GetNoParticles_10      (ScioSense_Apc1* apc1);                             // returns Number of particles with diameter >  10μm in 0.1L of air.
static inline uint16_t            Apc1_GetTVOC                (ScioSense_Apc1* apc1);                             // returns TVOC output
static inline uint16_t            Apc1_GetECO2                (ScioSense_Apc1* apc1);                             // returns Output in ppm CO2 equivalents
static inline uint16_t            Apc1_GetNO2                 (ScioSense_Apc1* apc1);                             // Reserved
static inline float               Apc1_GetCompT               (ScioSense_Apc1* apc1);                             // returns Compensated temperature (see datasheet)
static inline float               Apc1_GetCompRH              (ScioSense_Apc1* apc1);                             // returns Compensated humidity (see datasheet)
static inline float               Apc1_GetRawT                (ScioSense_Apc1* apc1);                             // returns Uncompensated temperature
static inline float               Apc1_GetRawRH               (ScioSense_Apc1* apc1);                             // returns Uncompensated humidity
static inline uint32_t            Apc1_GetRS0                 (ScioSense_Apc1* apc1);                             // returns Gas sensor 0 raw resistance value
static inline uint32_t            Apc1_GetRS1                 (ScioSense_Apc1* apc1);                             // returns Gas sensor 1 raw resistance value
static inline uint32_t            Apc1_GetRS2                 (ScioSense_Apc1* apc1);                             // returns Gas sensor 2 raw resistance value
static inline uint32_t            Apc1_GetRS3                 (ScioSense_Apc1* apc1);                             // returns Gas sensor 3 raw resistance value
static inline AirQualityIndex_UBA Apc1_GetAQI                 (ScioSense_Apc1* apc1);                             // returns Air Quality Index according to UBA Classification of TVOC value
static inline uint16_t            Apc1_GetFirmwareVersion     (ScioSense_Apc1* apc1);                             // returns Firmware version
static inline Apc1_ErrorCode      Apc1_GetError               (ScioSense_Apc1* apc1);                             // returns Error codes (see datasheet)

static inline Result              Apc1_CheckData              (const uint8_t* data, const Apc1_CommandResponse size);                               // calculates the checksum of the data and compares it with the last 2 byte; returns RESULT_CHECKSUM_ERROR on failure
static inline Result              Apc1_CheckCommandResponse   (const Apc1_Command command, const uint8_t* data, const Apc1_CommandResponse size);   // checks if the data corresponds to the command result protocol and calculates the checksum thereafter; returns RESULT_INVALID if the protocol does not match
static inline Result              Apc1_CheckMeasurementData   (const uint8_t* data);                                                                // checks measurement date checksum and data plausability


#include "ScioSense_Apc1.inl.h"
#endif // SCIOSENSE_APC1_C_H