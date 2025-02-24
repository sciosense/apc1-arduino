#ifndef SCIOSENSE_APC1_DEFINES_C_H
#define SCIOSENSE_APC1_DEFINES_C_H

#include <inttypes.h>

//// Output data structure adresses for the measurement data struct (64 bytes) and sensor info struct (23 bytes)
typedef uint8_t ResultAddress;

//// Measurement data struct
#define APC1_RESULT_ADDRESS_FRAME_HEADER                (0x00)
#define APC1_RESULT_ADDRESS_FRAME_LENGTH                (0x02)
#define APC1_RESULT_ADDRESS_PM_1_0                      (0x04)       // PM1.0 mass concentration
#define APC1_RESULT_ADDRESS_PM_2_5                      (0x06)       // PM2.5 mass concentration
#define APC1_RESULT_ADDRESS_PM_10                       (0x08)       // PM10  mass concentration
#define APC1_RESULT_ADDRESS_PMINAIR_1_0                 (0x0A)       // PM1.0 mass concentration in atmospheric environment
#define APC1_RESULT_ADDRESS_PMINAIR_2_5                 (0x0C)       // PM2.5 mass concentration in atmospheric environment
#define APC1_RESULT_ADDRESS_PMINAIR_10                  (0x0E)       // PM10  mass concentration in atmospheric environment
#define APC1_RESULT_ADDRESS_NOPARTICLES_0_3             (0x10)       // Number of particles with diameter > 0.3μm in 0.1L of air
#define APC1_RESULT_ADDRESS_NOPARTICLES_0_5             (0x12)       // Number of particles with diameter > 0.5μm in 0.1L of air
#define APC1_RESULT_ADDRESS_NOPARTICLES_1_0             (0x14)       // Number of particles with diameter > 1.0μm in 0.1L of air
#define APC1_RESULT_ADDRESS_NOPARTICLES_2_5             (0x16)       // Number of particles with diameter > 2.5μm in 0.1L of air
#define APC1_RESULT_ADDRESS_NOPARTICLES_5_0             (0x18)       // Number of particles with diameter > 5.0μm in 0.1L of air
#define APC1_RESULT_ADDRESS_NOPARTICLES_10              (0x1A)       // Number of particles with diameter >  10μm in 0.1L of air
#define APC1_RESULT_ADDRESS_TVOC                        (0x1C)       // TVOC output
#define APC1_RESULT_ADDRESS_ECO2                        (0x1E)       // Output in ppm CO2 equivalents
#define APC1_RESULT_ADDRESS_NO2                         (0x20)       // Reserved
#define APC1_RESULT_ADDRESS_T_COMP                      (0x22)       // Compensated temperature (see datasheet)
#define APC1_RESULT_ADDRESS_RH_COMP                     (0x24)       // Compensated humidity (see datasheet)
#define APC1_RESULT_ADDRESS_T_RAW                       (0x26)       // Uncompensated temperature
#define APC1_RESULT_ADDRESS_RH_RAW                      (0x28)       // Uncompensated humidity
#define APC1_RESULT_ADDRESS_RS0                         (0x2A)       // Gas sensor 0 raw resistance value
#define APC1_RESULT_ADDRESS_RS1                         (0x2E)       // Gas sensor 1 raw resistance value
#define APC1_RESULT_ADDRESS_RS2                         (0x32)       // Gas sensor 2 raw resistance value
#define APC1_RESULT_ADDRESS_RS3                         (0x36)       // Gas sensor 3 raw resistance value
#define APC1_RESULT_ADDRESS_AQI                         (0x3A)       // Air Quality Index according to UBA Classification of TVOC value
#define APC1_RESULT_ADDRESS_FIRMWARE_VERSION            (0x3C)       // Firmware version
#define APC1_RESULT_ADDRESS_ERROR_CODE                  (0x3D)       // Error codes (see datasheet)
#define APC1_RESULT_ADDRESS_CHECKSUM_H                  (0x3E)       // Frame (0x00 – 0x3D) checksum High byte
#define APC1_RESULT_ADDRESS_CHECKSUM_L                  (0x3F)       // Frame (0x00 – 0x3D) checksum Low byte

//// I2C interface register addresses
#define APC1_REGISTER_ADDRESS_COMMAND_WRITE             (0x40)
#define APC1_REGISTER_ADDRESS_COMMAND_RESULT            (0x47)

//// Sensor data struct
#define APC1_RESULT_ADDRESS_SENSOR_TYPE                 (0x04)
#define APC1_RESULT_ADDRESS_SENSOR_UID                  (0x0A)
#define APC1_RESULT_ADDRESS_SENSOR_FIRMWARE_VERSION     (0x13)

//// Error codes
typedef uint8_t Apc1_ErrorCode;
#define APC1_ERROR_CODE_DEFAULT                         (0)         // No Error
#define APC1_ERROR_CODE_TOO_MANY_FAN_RESTARTS           (1 << 0)    // Too many Fan restarts
#define APC1_ERROR_CODE_FAN_SPEED_TOO_LOW               (1 << 1)    // Fan-speed low
#define APC1_ERROR_CODE_PHOTO_DIODE                     (1 << 2)    // Photodiode
#define APC1_ERROR_CODE_FAN_STARTUP_ERROR               (1 << 3)    // Fan stopped
#define APC1_ERROR_CODE_LASER                           (1 << 4)    // Laser
#define APC1_ERROR_CODE_VOC                             (1 << 5)    // VOC Sensor
#define APC1_ERROR_CODE_RHT                             (1 << 6)    // Temperature and humidity sensor

//// Measurement Modes
typedef uint16_t Apc1_MeasurementMode;
#define APC1_MEASUREMENT_MODE_PASSIVE                   (0x0000)    // Device sends 64-byte structure on request (Default).
#define APC1_MEASUREMENT_MODE_ACTIVE                    (0x0001)    // Device sends 64-byte structure every second

//// Operating Modes
typedef  uint16_t Apc1_OperatingMode;
#define APC1_OPERATING_MODE_IDLE                        (0x0000)    // Idle mode; No Measurements; Fan Off
#define APC1_OPERATING_MODE_STANDARD                    (0x0001)    // Measurement Mode; Fan On

//// Command protocol structure values
#define APC1_COMMAND_LENGTH                             (7)
typedef const uint8_t Apc1_Command[APC1_COMMAND_LENGTH];
#define APC1_COMMAND_ADDRESS_START_BYTE_1               (0x42)
#define APC1_COMMAND_ADDRESS_START_BYTE_2               (0x4D)

#define APC1_COMMAND_ADDRESS_MEASUREMENT_MODE           (0xE1)          // Toggle measurement result communication mode
#define APC1_COMMAND_ADDRESS_REQUEST_MEASUREMENT        (0xE2)          // Once in passive mode, this command requests the device to send a 64-byte data structure containing the latest measurement results
#define APC1_COMMAND_ADDRESS_OPERATION_MODE             (0xE4)          // Toggle between Idle and Measurement mode (Default)
#define APC1_COMMAND_ADDRESS_READSENSOR_VERSION         (0xE9)          // Read module type, ID & FW version

#define APC1_COMMAND_SET_MEASUREMENT_MODE_PASSIVE       {0x42, 0x4D, 0xE1, 0x00, 0x00, 0x01, 0x70}
#define APC1_COMMAND_SET_MEASUREMENT_MODE_ACTIVE        {0x42, 0x4D, 0xE1, 0x00, 0x01, 0x01, 0x71}
#define APC1_COMMAND_PASSIVE_MEASUREMENT                {0x42, 0x4D, 0xE2, 0x00, 0x00, 0x01, 0x71}
#define APC1_COMMAND_SET_IDLE                           {0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73}
#define APC1_COMMAND_SET_WAKE                           {0x42, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74}
#define APC1_COMMAND_RESET                              {0x42, 0x4D, 0xE4, 0x00, 0x0F, 0x01, 0x82}
#define APC1_COMMAND_READ_SENSOR_VERSION                {0x42, 0x4D, 0xE9, 0x00, 0x00, 0x01, 0x78}

//// Command response addresses ( answer protocol structure )
typedef uint8_t Apc1_CommandResponse;
#define APC1_COMMAND_RESPONSE_DEFAULT_LENGTH                (8)     // Command response default length
#define APC1_COMMAND_RESPONSE_SENSOR_VERSION_LENGTH         (23)    // Command response length for "ReadSensorVersion" command
#define APC1_COMMAND_RESPONSE_MODULE_NAME_LENGTH            (6)    // Command response length for the module name part of the "ReadSensorVersion" command
#define APC1_COMMAND_RESPONSE_MEASUREMENT_LENGTH            (64)    // Command response length for "RequestMeasurement" command
#define APC1_COMMAND_RESPONSE_MEASUREMENT_PAYLOAD_LENGTH    (60)    // Measurement data struct payload length

#define APC1_COMMAND_RESPONSE_START_BYTE_ADDRESS_1          (0)
#define APC1_COMMAND_RESPONSE_START_BYTE_ADDRESS_2          (1)
#define APC1_COMMAND_RESPONSE_FRAME_LENGTH_ADDRESS_H        (2)    // Command response length high byte
#define APC1_COMMAND_RESPONSE_FRAME_LENGTH_ADDRESS_L        (3)    // Command response length low byte
#define APC1_COMMAND_RESPONSE_COMMAND_ADDRESS               (4)    // Command address
#define APC1_COMMAND_RESPONSE_DATA_ADDRESS                  (5)    // Data start address

//// IO Protocol
typedef uint8_t Apc1_Protocol;
#define APC1_PROTOCOL_UART      (0)
#define APC1_PROTOCOL_I2C       (1)

//// SystemTiming in ms
#define APC1_SYSTEM_TIMING_STANDARD_MEASURE     (1000)
#define APC1_SYSTEM_TIMING_COMMAND_EXEC         (200)

//// UBA Air Quality Index
#ifndef SCIOSENSE_AQI_UBA_CODES
#define SCIOSENSE_AQI_UBA_CODES
typedef uint8_t AirQualityIndex_UBA;
#define AQI_UBA_UNKNOWN         (0)
#define AQI_UBA_EXCELLENT       (1)
#define AQI_UBA_GOOD            (2)
#define AQI_UBA_MODERATE        (3)
#define AQI_UBA_POOR            (4)
#define AQI_UBA_UNHEALTHY       (5)
#endif

//// Result and Errors
#ifndef SCIOSENSE_RESULT_CODES
#define SCIOSENSE_RESULT_CODES
typedef int8_t Result;
#define RESULT_NOT_ALLOWED      (4)     // The requested command is not allowed.
#define RESULT_CHECKSUM_ERROR   (3)     // The value was read, but the checksum over the payload (valid and data) does not match.
#define RESULT_INVALID          (2)     // The value was read, but the data is invalid.
#define RESULT_IO_ERROR         (1)     // There was an IO communication error, read/write the stream failed.
#define RESULT_OK               (0)     // All OK; The value was read, the checksum matches, and data is valid.
#endif

#endif // SCIOSENSE_APC1_DEFINES_C_H