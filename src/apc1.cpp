#include <string.h>

#include "apc1.h"

using ScioSense::Utils::endian;

namespace ScioSense
{
        static constexpr APC1::Command SetMeasurementModePassive =
        {
            (uint8_t)   APC1::CommandAddress::StartByte1,
            (uint8_t)   APC1::CommandAddress::StartByte2,
            (uint8_t)   APC1::CommandAddress::MeasurementMode,
            highByte    ((uint16_t)APC1::MeasurementMode::Passive),
            lowByte     ((uint16_t)APC1::MeasurementMode::Passive),
            highByte    ((uint16_t)APC1::CommandAddress::StartByte1 + (uint16_t)APC1::CommandAddress::StartByte2 + (uint16_t)APC1::CommandAddress::MeasurementMode + (uint16_t)APC1::MeasurementMode::Passive),
            lowByte     ((uint16_t)APC1::CommandAddress::StartByte1 + (uint16_t)APC1::CommandAddress::StartByte2 + (uint16_t)APC1::CommandAddress::MeasurementMode + (uint16_t)APC1::MeasurementMode::Passive)
        };

        static constexpr APC1::Command SetMeasurementModeActive =
        {
            (uint8_t)   APC1::CommandAddress::StartByte1,
            (uint8_t)   APC1::CommandAddress::StartByte2,
            (uint8_t)   APC1::CommandAddress::MeasurementMode,
            highByte    ((uint16_t)APC1::MeasurementMode::Active),
            lowByte     ((uint16_t)APC1::MeasurementMode::Active),
            highByte    ((uint16_t)APC1::CommandAddress::StartByte1 + (uint16_t)APC1::CommandAddress::StartByte2 + (uint16_t)APC1::CommandAddress::MeasurementMode + (uint16_t)APC1::MeasurementMode::Active),
            lowByte     ((uint16_t)APC1::CommandAddress::StartByte1 + (uint16_t)APC1::CommandAddress::StartByte2 + (uint16_t)APC1::CommandAddress::MeasurementMode + (uint16_t)APC1::MeasurementMode::Active)
        };

        static constexpr APC1::Command PassiveMeasurement =
        {
            (uint8_t)   APC1::CommandAddress::StartByte1,
            (uint8_t)   APC1::CommandAddress::StartByte2,
            (uint8_t)   APC1::CommandAddress::RequestMeasurement,
                        0,
                        0,
            highByte    ((uint16_t)APC1::CommandAddress::StartByte1 + (uint16_t)APC1::CommandAddress::StartByte2 + (uint16_t)APC1::CommandAddress::RequestMeasurement),
            lowByte     ((uint16_t)APC1::CommandAddress::StartByte1 + (uint16_t)APC1::CommandAddress::StartByte2 + (uint16_t)APC1::CommandAddress::RequestMeasurement)
        };
        static constexpr APC1::Command SetIdle =
        {
            (uint8_t)   APC1::CommandAddress::StartByte1,
            (uint8_t)   APC1::CommandAddress::StartByte2,
            (uint8_t)   APC1::CommandAddress::OperationMode,
            highByte    ((uint16_t)APC1::OperatingMode::Idle),
            lowByte     ((uint16_t)APC1::OperatingMode::Idle),
            highByte    ((uint16_t)APC1::CommandAddress::StartByte1 + (uint16_t)APC1::CommandAddress::StartByte2 + (uint16_t)APC1::CommandAddress::OperationMode + (uint16_t)APC1::OperatingMode::Idle),
            lowByte     ((uint16_t)APC1::CommandAddress::StartByte1 + (uint16_t)APC1::CommandAddress::StartByte2 + (uint16_t)APC1::CommandAddress::OperationMode + (uint16_t)APC1::OperatingMode::Idle)
        };

        static constexpr APC1::Command SetWake =
        {
            (uint8_t)   APC1::CommandAddress::StartByte1,
            (uint8_t)   APC1::CommandAddress::StartByte2,
            (uint8_t)   APC1::CommandAddress::OperationMode,
            highByte    ((uint16_t)APC1::OperatingMode::Standard),
            lowByte     ((uint16_t)APC1::OperatingMode::Standard),
            highByte    ((uint16_t)APC1::CommandAddress::StartByte1 + (uint16_t)APC1::CommandAddress::StartByte2 + (uint16_t)APC1::CommandAddress::OperationMode + (uint16_t)APC1::OperatingMode::Standard),
            lowByte     ((uint16_t)APC1::CommandAddress::StartByte1 + (uint16_t)APC1::CommandAddress::StartByte2 + (uint16_t)APC1::CommandAddress::OperationMode + (uint16_t)APC1::OperatingMode::Standard)
        };

        static constexpr APC1::Command ReadSensorVersion =
        {
            (uint8_t)   APC1::CommandAddress::StartByte1,
            (uint8_t)   APC1::CommandAddress::StartByte2,
            (uint8_t)   APC1::CommandAddress::ReadSensorVersion,
                        0,
                        0,
            highByte    ((uint16_t)APC1::CommandAddress::StartByte1 + (uint16_t)APC1::CommandAddress::StartByte2 + (uint16_t)APC1::CommandAddress::ReadSensorVersion),
            lowByte     ((uint16_t)APC1::CommandAddress::StartByte1 + (uint16_t)APC1::CommandAddress::StartByte2 + (uint16_t)APC1::CommandAddress::ReadSensorVersion)
        };

    APC1::~APC1() { }

    APC1::APC1()
    {
        serial      = nullptr;
        debugStream = nullptr;

        reset();
    }

    void APC1::enableDebugging(Stream& debugStream)
    {
        this->debugStream = &debugStream;
    }

    void APC1::disableDebugging()
    {
        debugStream = nullptr;
    }

    bool APC1::begin(Stream& serial)
    {
        this->serial = &serial;
        reset();

        return isConnected();
    }

    bool APC1::isConnected()
    {
        return fwVersion != 0;
    }

    APC1::Result APC1::read(uint8_t* buf, const CommandResponse& size)
    {
        if ((serial->readBytes(buf, (size_t)size) == (size_t)size))
        {
            return Result::Ok;
        }

        return Result::IOError;
    }

    APC1::Result APC1::invoke(Command& command, uint8_t* buf, const CommandResponse& size)
    {
        Result res= Result::IOError;

        if (serial->write(command, sizeof(command)) == sizeof(command))
        {
            if (buf != nullptr)
            {
                res = read(buf, size);
                if (res == Result::Ok)
                {
                    res = checkCommandResponse(command, buf, size);
                }
            }
            else
            {
                res = Result::Ok;
            }
        }

        return res;
    }

    void APC1::clear()
    {
        serial->flush();
        while (serial->available())
        {
            uint8_t buf[8];
            serial->readBytes(buf, 8);
        }
    }

    void APC1::reset()
    {
        debug(__func__);

        memset(type, 0, sizeof(type));
        memset(measurementData , 0, sizeof(measurementData));
        dataValid= false;

        serialNumber = 0;
        delimiter    = 0;
        fwVersion    = 0;

        if (serial)
        {
            clear();

            if (!setOperatingMode(OperatingMode::Standard))
            {
                clear();
                setOperatingMode(OperatingMode::Standard);
            }

            setMeasurementMode(MeasurementMode::Passive);
            readSensorVersion();
        }
    }

    APC1::Result APC1::checkData(const uint8_t* data, const CommandResponse& size)
    {
        if ((uint8_t)size < 3)
        {
            return Result::ChecksumError;
        }

        const uint8_t payloadSize   = ((uint8_t)size) - 2;
        const uint16_t checksum     = endian::bigTo<uint16_t>(data + payloadSize);
        uint16_t calculadedChecksum = 0;

        for (uint8_t i = 0; i<payloadSize; i++)
        {
            calculadedChecksum += data[i];
        }

        return (checksum == calculadedChecksum) ? Result::Ok : Result::ChecksumError;
    }

    APC1::Result APC1::checkCommandResponse(Command& command, const uint8_t* data, const CommandResponse& size)
    {
        Result res = Result::Invalid;

        if
        (
            command[(uint8_t)CommandResponse::StartByteAddress1]    == data[(uint8_t)CommandResponse::StartByteAddress1]
         && command[(uint8_t)CommandResponse::StartByteAddress2]    == data[(uint8_t)CommandResponse::StartByteAddress2]
         && command[(uint8_t)CommandResponse::CommandAddress]       == data[(uint8_t)CommandResponse::CommandAddress]
         && command[(uint8_t)CommandResponse::DataAddress]          == data[(uint8_t)CommandResponse::DataAddress]
         &&  (uint8_t)size                                          == data[(uint8_t)CommandResponse::FrameLengthAddressL]
        )
        {
            res= checkData(data, size);
        }

        return res;
    }

    APC1::Result APC1::checkMeasurementData(const uint8_t* data)
    {
        Result res = Result::Invalid;

        if
        (
            (uint8_t)CommandAddress::StartByte1                 == data[(uint8_t)CommandResponse::StartByteAddress1]
         && (uint8_t)CommandAddress::StartByte2                 == data[(uint8_t)CommandResponse::StartByteAddress2]
         && (uint8_t) CommandResponse::MeasurementPayloadLength == data[(uint8_t)CommandResponse::FrameLengthAddressL]
        )
        {
            // this is a additional data plausibility check to prevent
            // reads of empty(zero) payloads, while the device switches between opmodes.
            //
            // with exception of ECO2 and AQI, all readings can
            // theoretically be valid zeros. (see datasheet 8.2 output data table).
            // AQI has to be in the 1-5 range. if this is not the case,
            // the measurement is invalid.
            bool dataEmpty= (data[(uint8_t)ResultAddress::AQI] == 0);

            if (!dataEmpty)
            {
                res = checkData(data, CommandResponse::MeasurementLength);
            }
        }

        return res;
    }

    bool APC1::setOperatingMode(const OperatingMode& mode)
    {
        debug(__func__, (uint8_t)mode);

        Result res;
        uint8_t data[(uint8_t)CommandResponse::DefaultLength];

        switch (mode)
        {
            case OperatingMode::Idle:     res = invoke(SetIdle, data, CommandResponse::DefaultLength); break;
            case OperatingMode::Standard: res = invoke(SetWake, data, CommandResponse::DefaultLength); break;
            default:
            {
                debug("Unable to set invalid operating mode.");
                return false;
            }
        }

        operatingMode = mode;

        return res != Result::IOError;
    }

    bool APC1::setMeasurementMode(const MeasurementMode& mode)
    {
        debug(__func__, (uint8_t)mode);

        Result res;
        uint8_t data[(uint8_t)CommandResponse::DefaultLength];

        switch (mode)
        {
            case MeasurementMode::Active:  res = invoke(SetMeasurementModeActive,  data, CommandResponse::DefaultLength); break;
            case MeasurementMode::Passive: res = invoke(SetMeasurementModePassive, data, CommandResponse::DefaultLength); break;
            default:
            {
                debug("Unable to set invalid measurement mode.");
                return false;
            }
        }

        measurementMode = mode;

        return res != Result::IOError;
    }

    APC1::Result APC1::readSensorVersion()
    {
        debug(__func__);

        Result res;
        uint8_t data[(uint8_t)CommandResponse::SensorVersionLength] = {0};

        invoke(ReadSensorVersion, data, CommandResponse::SensorVersionLength);
        res = checkData(data, CommandResponse::SensorVersionLength);
        if (res == Result::Ok)
        {
            // module name and type
            memcpy(type, (data + (uint8_t)ResultAddress::SensorType), 6);
            type[6] = 0;

            serialNumber= endian::bigTo<uint64_t>(data + (uint8_t)ResultAddress::SensorUID);
            delimiter   = data[0x12];
            fwVersion   = endian::bigTo<uint16_t>(data + (uint8_t)ResultAddress::SensorFirmwareVersion);

            debug((char*)type);
            debug("Serial Number:", data + (uint8_t)ResultAddress::SensorUID, 8);
            debug("Firmware     :", fwVersion);
        }

        // APC1 devices with firmware < 34 do not respond to ::ReadSensorVersion,
        // but the sensor version can be read from measurement data.
        else
        {
            if
            (
                invoke(PassiveMeasurement)                                  == Result::Ok
             && read(measurementData, CommandResponse::MeasurementLength)   == Result::Ok
             && checkMeasurementData(measurementData)                       == Result::Ok
            )
            {
                res = Result::Ok;
                fwVersion = getValueOf<uint8_t>(ResultAddress::FirmwareVersion);
                debug("Firmware:", fwVersion);
            }
            else
            {
                res = Result::Invalid;
            }
        }

        return res;
    }

    APC1::Result APC1::update()
    {
        Result res;
        if (operatingMode != OperatingMode::Standard)
        {
            debug(__func__);
            debug("Update may only be called in OperatingMode::Standard");
            return Result::NotAllowed;
        }

        if (measurementMode == MeasurementMode::Passive)
        {
            res= invoke(PassiveMeasurement);
            debug("Invoke PassiveMeasurement:", res);
        }

        res = read(measurementData, CommandResponse::MeasurementLength);
        debug("Read measurement data:", res);

        if (res == Result::Ok)
        {
            res = checkMeasurementData(measurementData);
            debug("Validate measurement data:", res);
            dataValid = res == Result::Ok;
        }

        return res;
    }

    uint16_t APC1::getPM_1_0()
    {
        return getValueOf<uint16_t>(ResultAddress::PM_1_0);
    }

    uint16_t APC1::getPM_2_5()
    {
        return getValueOf<uint16_t>(ResultAddress::PM_2_5);
    }

    uint16_t APC1::getPM_10()
    {
        return getValueOf<uint16_t>(ResultAddress::PM_10);
    }

    uint16_t APC1::getPMInAir_1_0()
    {
        return getValueOf<uint16_t>(ResultAddress::PMInAir_1_0);
    }

    uint16_t APC1::getPMInAir_2_5()
    {
        return getValueOf<uint16_t>(ResultAddress::PMInAir_2_5);
    }

    uint16_t APC1::getPMInAir_10()
    {
        return getValueOf<uint16_t>(ResultAddress::PMInAir_10);
    }

    uint16_t APC1::getNoParticles_0_3()
    {
        return getValueOf<uint16_t>(ResultAddress::NoParticles_0_3);
    }

    uint16_t APC1::getNoParticles_0_5()
    {
        return getValueOf<uint16_t>(ResultAddress::NoParticles_0_5);
    }

    uint16_t APC1::getNoParticles_1_0()
    {
        return getValueOf<uint16_t>(ResultAddress::NoParticles_1_0);
    }

    uint16_t APC1::getNoParticles_2_5()
    {
        return getValueOf<uint16_t>(ResultAddress::NoParticles_2_5);
    }

    uint16_t APC1::getNoParticles_5_0()
    {
        return getValueOf<uint16_t>(ResultAddress::NoParticles_5_0);
    }

    uint16_t APC1::getNoParticles_10()
    {
        return getValueOf<uint16_t>(ResultAddress::NoParticles_10);
    }

    uint16_t APC1::getTVOC()
    {
        return getValueOf<uint16_t>(ResultAddress::TVOC);
    }

    uint16_t APC1::getECO2()
    {
        return getValueOf<uint16_t>(ResultAddress::ECO2);
    }

    uint16_t APC1::getNO2()
    {
        return getValueOf<uint16_t>(ResultAddress::NO2);
    }

    float APC1::getCompT()
    {
        return ((float)getValueOf<uint16_t>(ResultAddress::T_comp)) * 0.1f;
    }

    float APC1::getCompRH()
    {
        return ((float)getValueOf<uint16_t>(ResultAddress::RH_comp)) * 0.1f;
    }

    float APC1::getRawT()
    {
        return ((float)getValueOf<uint16_t>(ResultAddress::T_raw)) * 0.1f;
    }

    float APC1::getRawRH()
    {
        return ((float)getValueOf<uint16_t>(ResultAddress::RH_raw)) * 0.1f;
    }

    uint32_t APC1::getRS0()
    {
        return getValueOf<uint32_t>(ResultAddress::RS0);
    }

    uint32_t APC1::getRS1()
    {
        return getValueOf<uint32_t>(ResultAddress::RS1);
    }

    uint32_t APC1::getRS2()
    {
        return getValueOf<uint32_t>(ResultAddress::RS2);
    }

    uint32_t APC1::getRS3()
    {
        return getValueOf<uint32_t>(ResultAddress::RS3);
    }

    APC1::AirQualityIndex_UBA APC1::getAQI()
    {
        return static_cast<AirQualityIndex_UBA>(measurementData[(uint8_t)ResultAddress::AQI]);
    }

    uint16_t APC1::getFirmwareVersion()
    {
        return fwVersion;
    }

    APC1::ErrorCode APC1::getError()
    {
        return static_cast<ErrorCode>(measurementData[(uint8_t)ResultAddress::ErrorCode]);
    }

    const char* APC1::getAQIString()
    {
        AirQualityIndex_UBA aqi = getAQI();
        return toString(aqi);
    }

    const char* APC1::toString(const APC1::Result& result)
    {
        switch (result)
        {
            case Result::NotAllowed     : return "not-allowed";
            case Result::IOError        : return "io-error";
            case Result::ChecksumError  : return "checksum-error";
            case Result::Invalid        : return "data-invalid";
            case Result::Ok             : return "ok";
            default                     : return "unknown-status";
        }
    }

    const char* APC1::toString(const AirQualityIndex_UBA& aqi)
    {
        switch (aqi)
        {
            case AirQualityIndex_UBA::Excellent :     return "Excellent";
            case AirQualityIndex_UBA::Good      :     return "Good";
            case AirQualityIndex_UBA::Moderate  :     return "Moderate";
            case AirQualityIndex_UBA::Poor      :     return "Poor";
            case AirQualityIndex_UBA::Unhealthy :     return "Unhealthy";
            default                             :     return "Unknown";
        }
    }

    void APC1::debug(const char* msg, const uint8_t* data, const size_t& size)
    {
        if (debugStream)
        {
            debugStream->print(debugPrefix);

            if (data && size != 0)
            {
                debugStream->print(msg);
                for (size_t i = 0; i < size; i++)
                {
                    debugStream->print(" 0x");
                    debugStream->print(data[i], HEX);
                }
                debugStream->println();
            }
            else
            {
                debugStream->println(msg);
            }
        }
    }

    void APC1::debug(const char* msg)
    {
        debug(msg, (uint8_t*)nullptr, 0);
    }

    void APC1::debug(const char* msg, const Result& res)
    {
        if (debugStream)
        {
            debugStream->print(debugPrefix);
            debugStream->print(msg);
            debugStream->print(" ");
            debugStream->println(toString(res));
        }
    }

    template<class T> void APC1::debug(const char* msg, T data)
    {
        if (debugStream)
        {
            debugStream->print(debugPrefix);
            debugStream->print(msg);
            debugStream->print(" ");
            debugStream->println(data);
        }
    }

} //namespace ScioSense