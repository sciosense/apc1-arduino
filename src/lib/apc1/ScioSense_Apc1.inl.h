#ifndef SCIOSENSE_APC1_C_INL
#define SCIOSENSE_APC1_C_INL

#include "ScioSense_Apc1.h"

#include <math.h>

#define clear()             if (apc1->io.clear) { apc1->io.clear(apc1->io.config); }
#define wait(ms)            apc1->io.wait(ms)

#define hasAnyFlag(a, b)    (((a) & (b)) != 0)
#define hasFlag(a, b)       (((a) & (b)) == (b))

#define memset(a, b, s)     for(size_t i = 0; i < s; i++) {a[i] = b;}
#define memcpy(a, b, s)     for(size_t i = 0; i < s; i++) {a[i] = b[i];}

static inline uint16_t Apc1_GetValueOf16(const uint8_t* data, const uint16_t resultAddress)
{
    return ((uint16_t)data[resultAddress] << 8) + (uint16_t)data[resultAddress + 1];
}

static inline uint32_t Apc1_GetValueOf32(uint8_t* data, const uint16_t resultAddress)
{
    return  ((uint32_t)data[resultAddress + 0] << 24)
          + ((uint32_t)data[resultAddress + 1] << 16)
          + ((uint32_t)data[resultAddress + 2] <<  8)
          +  (uint32_t)data[resultAddress + 3];
}

static inline uint64_t Apc1_GetValueOf64(uint8_t* data, const uint16_t resultAddress)
{
    return  ((uint64_t)data[resultAddress + 0] << 56)
          + ((uint64_t)data[resultAddress + 1] << 48)
          + ((uint64_t)data[resultAddress + 2] << 40)
          + ((uint64_t)data[resultAddress + 3] << 32)
          + ((uint64_t)data[resultAddress + 4] << 24)
          + ((uint64_t)data[resultAddress + 5] << 16)
          + ((uint64_t)data[resultAddress + 6] << 8)
          +  (uint64_t)data[resultAddress + 7];
}

static inline Result Apc1_Read(ScioSense_Apc1* apc1, const uint16_t address, uint8_t* data, const size_t size)
{
    if (apc1->io.protocol == APC1_PROTOCOL_UART)
    {
        return apc1->io.read(apc1->io.config, 0, data, size);
    }
    else if (apc1->io.protocol == APC1_PROTOCOL_I2C)
    {
        return apc1->io.read(apc1->io.config, address, data, size);
    }

    return RESULT_IO_ERROR;
}

static inline Result Apc1_Write(ScioSense_Apc1* apc1, const uint16_t address, uint8_t* data, const size_t size)
{
    if (apc1->io.protocol == APC1_PROTOCOL_UART)
    {
        return apc1->io.write(apc1->io.config, 0, data, size);
    }
    else if (apc1->io.protocol == APC1_PROTOCOL_I2C)
    {
        return apc1->io.write(apc1->io.config, address, data, size);
    }

    return RESULT_IO_ERROR;
}

static inline Result Apc1_Invoke(ScioSense_Apc1* apc1, Apc1_Command command, uint8_t* resultBuf, const size_t size)
{
    Result result;

    result = Apc1_Write(apc1, APC1_REGISTER_ADDRESS_COMMAND_WRITE, (uint8_t*)command, APC1_COMMAND_LENGTH);

    if (result == RESULT_OK)
    {
        if (apc1->io.protocol == APC1_PROTOCOL_I2C)
        {
            wait(APC1_SYSTEM_TIMING_COMMAND_EXEC);
        }

        if (resultBuf != NULL)
        {
            result = Apc1_Read(apc1, APC1_REGISTER_ADDRESS_COMMAND_RESULT, resultBuf, size);
            if (result == RESULT_OK)
            {
                result = Apc1_CheckCommandResponse(command, resultBuf, (Apc1_CommandResponse)size);
            }
        }
    }

    return result;
}

static inline Result Apc1_InvokePassiveMeasurement(ScioSense_Apc1* apc1)
{
    static const Apc1_Command command = APC1_COMMAND_PASSIVE_MEASUREMENT;

    return Apc1_Invoke(apc1, command, NULL, 0);
}

static inline Result Apc1_InvokeSetIdle(ScioSense_Apc1* apc1)
{
    static const Apc1_Command command = APC1_COMMAND_SET_IDLE;
    uint8_t buf[APC1_COMMAND_RESPONSE_DEFAULT_LENGTH];

    return Apc1_Invoke(apc1, command, buf, APC1_COMMAND_RESPONSE_DEFAULT_LENGTH);
}

static inline Result Apc1_InvokeSetWake(ScioSense_Apc1* apc1)
{
    static const Apc1_Command command = APC1_COMMAND_SET_WAKE;

    return Apc1_Invoke(apc1, command, NULL, 0);
}

static inline Result Apc1_InvokeSetMeasurementModeActive(ScioSense_Apc1* apc1)
{
    static const Apc1_Command command = APC1_COMMAND_SET_MEASUREMENT_MODE_ACTIVE;
    uint8_t buf[APC1_COMMAND_RESPONSE_DEFAULT_LENGTH];

    return Apc1_Invoke(apc1, command, buf, APC1_COMMAND_RESPONSE_DEFAULT_LENGTH);
}

static inline Result Apc1_InvokeSetMeasurementModePassive(ScioSense_Apc1* apc1)
{
    static const Apc1_Command command = APC1_COMMAND_SET_MEASUREMENT_MODE_PASSIVE;
    uint8_t buf[APC1_COMMAND_RESPONSE_DEFAULT_LENGTH];

    return Apc1_Invoke(apc1, command, buf, APC1_COMMAND_RESPONSE_DEFAULT_LENGTH);
}

static inline Result Apc1_InvokeReadSensorVersion(ScioSense_Apc1* apc1)
{
    Result result;
    static const Apc1_Command command = APC1_COMMAND_READ_SENSOR_VERSION;
    uint8_t buf[APC1_COMMAND_RESPONSE_SENSOR_VERSION_LENGTH];

    result = Apc1_Invoke(apc1, command, buf, APC1_COMMAND_RESPONSE_SENSOR_VERSION_LENGTH);
    if (buf[0] == APC1_COMMAND_ADDRESS_START_BYTE_1)
    {
        result = Apc1_CheckData(buf, APC1_COMMAND_RESPONSE_SENSOR_VERSION_LENGTH);
        if (result == RESULT_OK)
        {
            memcpy(apc1->moduleName, (buf + APC1_RESULT_ADDRESS_SENSOR_TYPE), APC1_COMMAND_RESPONSE_MODULE_NAME_LENGTH);
            apc1->moduleName[APC1_COMMAND_RESPONSE_MODULE_NAME_LENGTH] = 0;
            apc1->serialNumber  = Apc1_GetValueOf64(buf, APC1_RESULT_ADDRESS_SENSOR_UID);
            apc1->fwVersion     = Apc1_GetValueOf16(buf, APC1_RESULT_ADDRESS_SENSOR_FIRMWARE_VERSION);
        }
    }

    return result;
}

static inline Result Apc1_Reset(ScioSense_Apc1* apc1)
{
    Result result;

    memset(apc1->moduleName     , 0, APC1_COMMAND_RESPONSE_MODULE_NAME_LENGTH+1);
    memset(apc1->measurementData, 0, APC1_COMMAND_RESPONSE_MEASUREMENT_LENGTH);

    apc1->serialNumber  = 0;
    apc1->fwVersion     = 0;

    clear();

    if (apc1->io.protocol == APC1_PROTOCOL_UART)
    {
        result = Apc1_SetOperatingMode(apc1, APC1_OPERATING_MODE_STANDARD);
        if (result != RESULT_OK)
        {
            // retry
            clear();
            Apc1_SetOperatingMode(apc1, APC1_OPERATING_MODE_STANDARD);
        }

        Apc1_SetMeasurementMode(apc1, APC1_MEASUREMENT_MODE_PASSIVE);
        result = Apc1_ReadSensorVersion(apc1);
    }
    else
    {
        Apc1_Command reset = APC1_COMMAND_RESET;
        Apc1_Invoke(apc1, reset, NULL, 0);
        wait(APC1_SYSTEM_TIMING_STANDARD_MEASURE);
        Apc1_SetOperatingMode(apc1, APC1_OPERATING_MODE_STANDARD);
        Apc1_SetMeasurementMode(apc1, APC1_MEASUREMENT_MODE_ACTIVE);

        result = Apc1_ReadSensorVersion(apc1);
        if (result != RESULT_OK)
        {
            //retry
            wait(APC1_SYSTEM_TIMING_COMMAND_EXEC);
            result = Apc1_ReadSensorVersion(apc1);
        }
    }

    return result;
}

static inline Result Apc1_Update(ScioSense_Apc1* apc1)
{
    Result result;

    if (apc1->operatingMode != APC1_OPERATING_MODE_STANDARD)
    {
        return RESULT_NOT_ALLOWED;
    }

    if (apc1->measurementMode == APC1_MEASUREMENT_MODE_PASSIVE)
    {
        result = Apc1_InvokePassiveMeasurement(apc1);
        if (result != RESULT_OK)
        {
            return result;
        }
    }

    result = Apc1_Read(apc1, APC1_RESULT_ADDRESS_FRAME_HEADER, apc1->measurementData, APC1_COMMAND_RESPONSE_MEASUREMENT_LENGTH);
    if (result == RESULT_OK)
    {
        result = Apc1_CheckMeasurementData(apc1->measurementData);
    }

    return result;
}

static inline Result Apc1_ReadSensorVersion(ScioSense_Apc1* apc1)
{
    Result result;

    result = Apc1_InvokeReadSensorVersion(apc1);
    if (result != RESULT_OK && apc1->io.protocol == APC1_PROTOCOL_UART)
    {
        // APC1 devices with firmware < 34 do not respond to ::ReadSensorVersion,
        // but the firmware version can be read from measurement data.
        if
        (
            Apc1_InvokePassiveMeasurement(apc1)                                                                                 == RESULT_OK
         && Apc1_Read(apc1, APC1_RESULT_ADDRESS_FRAME_HEADER, apc1->measurementData, APC1_COMMAND_RESPONSE_MEASUREMENT_LENGTH)  == RESULT_OK
         && Apc1_CheckMeasurementData(apc1->measurementData)                                                                    == RESULT_OK
        )
        {
            result          = RESULT_OK;
            apc1->fwVersion = apc1->measurementData[APC1_RESULT_ADDRESS_FIRMWARE_VERSION];
        }
    }

    return result;
}

static inline Result Apc1_SetOperatingMode(ScioSense_Apc1* apc1, const Apc1_OperatingMode mode)
{
    Result result;

    switch (mode)
    {
        case APC1_OPERATING_MODE_IDLE        : result = Apc1_InvokeSetIdle(apc1);    break;
        case APC1_OPERATING_MODE_STANDARD    : result = Apc1_InvokeSetWake(apc1);    break;
        default                              : result = RESULT_NOT_ALLOWED;          break;
    }

    apc1->operatingMode = mode;

    return result;
}

static inline Result Apc1_SetMeasurementMode(ScioSense_Apc1* apc1, const Apc1_MeasurementMode mode)
{
    Result result;

    if (apc1->io.protocol == APC1_PROTOCOL_UART)
    {
        switch (mode)
        {
            case APC1_MEASUREMENT_MODE_ACTIVE    : result = Apc1_InvokeSetMeasurementModeActive(apc1);  break;
            case APC1_MEASUREMENT_MODE_PASSIVE   : result = Apc1_InvokeSetMeasurementModePassive(apc1); break;
            default                              : result = RESULT_NOT_ALLOWED;                         break;
        }

        apc1->measurementMode = mode;
    }
    else if (apc1->io.protocol == APC1_PROTOCOL_I2C)
    {
        apc1->measurementMode = APC1_MEASUREMENT_MODE_ACTIVE;
        result = (mode == APC1_MEASUREMENT_MODE_ACTIVE) ? RESULT_OK : RESULT_NOT_ALLOWED;
    }
    else
    {
        result = RESULT_IO_ERROR;
    }

    return result;
}

static inline bool Apc1_IsConnected(ScioSense_Apc1* apc1)
{
    return apc1->fwVersion != 0;
}

static inline uint16_t Apc1_GetPM_1_0(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_PM_1_0);
}

static inline uint16_t Apc1_GetPM_2_5(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_PM_2_5);
}

static inline uint16_t Apc1_GetPM_10(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_PM_10);
}

static inline uint16_t Apc1_GetPMInAir_1_0(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_PMINAIR_1_0);
}

static inline uint16_t Apc1_GetPMInAir_2_5(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_PMINAIR_2_5);
}

static inline uint16_t Apc1_GetPMInAir_10(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_PMINAIR_10);
}

static inline uint16_t Apc1_GetNoParticles_0_3(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_NOPARTICLES_0_3);
}

static inline uint16_t Apc1_GetNoParticles_0_5(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_NOPARTICLES_0_5);
}

static inline uint16_t Apc1_GetNoParticles_1_0(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_NOPARTICLES_1_0);
}

static inline uint16_t Apc1_GetNoParticles_2_5(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_NOPARTICLES_2_5);
}

static inline uint16_t Apc1_GetNoParticles_5_0(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_NOPARTICLES_5_0);
}

static inline uint16_t Apc1_GetNoParticles_10(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_NOPARTICLES_10);
}

static inline uint16_t Apc1_GetTVOC(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_TVOC);
}

static inline uint16_t Apc1_GetECO2(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_ECO2);
}

static inline uint16_t Apc1_GetNO2(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_NO2);
}

static inline float Apc1_GetCompT(ScioSense_Apc1* apc1)
{
    return (float)Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_T_COMP) * 0.1f;
}

static inline float Apc1_GetCompRH(ScioSense_Apc1* apc1)
{
    return (float)Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_RH_COMP) * 0.1f;
}

static inline float Apc1_GetRawT(ScioSense_Apc1* apc1)
{
    return (float)Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_T_RAW) * 0.1f;
}

static inline float Apc1_GetRawRH(ScioSense_Apc1* apc1)
{
    return (float)Apc1_GetValueOf16(apc1->measurementData, APC1_RESULT_ADDRESS_RH_RAW) * 0.1f;
}

static inline uint32_t Apc1_GetRS0(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf32(apc1->measurementData, APC1_RESULT_ADDRESS_RS0);
}

static inline uint32_t Apc1_GetRS1(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf32(apc1->measurementData, APC1_RESULT_ADDRESS_RS1);
}

static inline uint32_t Apc1_GetRS2(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf32(apc1->measurementData, APC1_RESULT_ADDRESS_RS2);
}

static inline uint32_t Apc1_GetRS3(ScioSense_Apc1* apc1)
{
    return Apc1_GetValueOf32(apc1->measurementData, APC1_RESULT_ADDRESS_RS3);
}

static inline AirQualityIndex_UBA Apc1_GetAQI(ScioSense_Apc1* apc1)
{
    return apc1->measurementData[APC1_RESULT_ADDRESS_AQI];
}

static inline uint16_t Apc1_GetFirmwareVersion(ScioSense_Apc1* apc1)
{
    return apc1->fwVersion;
}

static inline Apc1_ErrorCode Apc1_GetError(ScioSense_Apc1* apc1)
{
    return apc1->measurementData[APC1_RESULT_ADDRESS_ERROR_CODE];
}

static inline Result Apc1_CheckData(const uint8_t* data, const Apc1_CommandResponse size)
{
    if (size < 3)
    {
        return RESULT_CHECKSUM_ERROR;
    }


    const uint8_t payloadSize   = ((uint8_t)size) - 2;
    const uint16_t checksum     = Apc1_GetValueOf16(data, payloadSize);
    uint16_t calculatedChecksum = 0;

    for (uint8_t i = 0; i<payloadSize; i++)
    {
        calculatedChecksum += data[i];
    }

    return (checksum == calculatedChecksum) ? RESULT_OK : RESULT_CHECKSUM_ERROR;
}

static inline Result Apc1_CheckCommandResponse(const Apc1_Command command, const uint8_t* data, const Apc1_CommandResponse size)
{
    Result result = RESULT_INVALID;

    if
    (
        command[APC1_COMMAND_RESPONSE_START_BYTE_ADDRESS_1] == data[APC1_COMMAND_RESPONSE_START_BYTE_ADDRESS_1]
     && command[APC1_COMMAND_RESPONSE_START_BYTE_ADDRESS_2] == data[APC1_COMMAND_RESPONSE_START_BYTE_ADDRESS_2]
     && command[APC1_COMMAND_RESPONSE_COMMAND_ADDRESS]      == data[APC1_COMMAND_RESPONSE_COMMAND_ADDRESS]
     && command[APC1_COMMAND_RESPONSE_DATA_ADDRESS]         == data[APC1_COMMAND_RESPONSE_DATA_ADDRESS]
     &&  (uint8_t)size                                      == data[APC1_COMMAND_RESPONSE_FRAME_LENGTH_ADDRESS_L]
    )
    {
        result = Apc1_CheckData(data, size);
    }

    return result;
}

static inline Result Apc1_CheckMeasurementData(const uint8_t* data)
{
    Result result = RESULT_INVALID;

    if
    (
        APC1_COMMAND_ADDRESS_START_BYTE_1                == data[APC1_COMMAND_RESPONSE_START_BYTE_ADDRESS_1]
     && APC1_COMMAND_ADDRESS_START_BYTE_2                == data[APC1_COMMAND_RESPONSE_START_BYTE_ADDRESS_2]
     && APC1_COMMAND_RESPONSE_MEASUREMENT_PAYLOAD_LENGTH == data[APC1_COMMAND_RESPONSE_FRAME_LENGTH_ADDRESS_L]
    )
    {
        // this is a additional data plausibility check to prevent
        // reads of empty(zero) payloads, while the device switches between opmodes.
        //
        // with exception of ECO2 and AQI, all readings can
        // theoretically be valid zeros. (see datasheet 8.2 output data table).
        // AQI has to be in the 1-5 range. if this is not the case,
        // the measurement is invalid.
        bool dataEmpty= (data[APC1_RESULT_ADDRESS_AQI] == 0);

        if (!dataEmpty)
        {
            result = Apc1_CheckData(data, APC1_COMMAND_RESPONSE_MEASUREMENT_LENGTH);
        }
    }

    return result;
}


#undef wait
#undef clear
#undef hasAnyFlag
#undef hasFlag
#undef memset
#undef memcpy

#endif // SCIOSENSE_APC1_C_INL