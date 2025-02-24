#include "apc1.h"


APC1::~APC1() { }

APC1::APC1()
{
    io              = { 0 };
    moduleName[0]   = 0;
    fwVersion       = 0;
    serialNumber    = 0;
    operatingMode   = APC1_OPERATING_MODE_STANDARD;
    measurementMode = APC1_MEASUREMENT_MODE_PASSIVE;
}

void APC1::begin(Stream* serial)
{
    serialConfig           = { 0 };
    serialConfig.serial    = serial;

    io.read             = ScioSense_Arduino_Serial_Read;
    io.write            = ScioSense_Arduino_Serial_Write;
    io.wait             = ScioSense_Arduino_Serial_Wait;
    io.clear            = ScioSense_Arduino_Serial_Clear;
    io.protocol         = APC1_PROTOCOL_UART;
    io.config           = &serialConfig;
}

void APC1::begin(TwoWire* wire, const uint8_t address)
{
    i2cConfig           = { 0 };
    i2cConfig.wire      = wire;
    i2cConfig.address   = address;

    io.read             = ScioSense_Arduino_I2c_Read;
    io.write            = ScioSense_Arduino_I2c_Write;
    io.wait             = ScioSense_Arduino_I2c_Wait;
    io.protocol         = APC1_PROTOCOL_I2C;
    io.config           = &i2cConfig;

    //there is no passive mode when using the i2c io interface
    measurementMode     = APC1_MEASUREMENT_MODE_ACTIVE;
}

inline bool APC1::init()
{
    return Apc1_Reset(this) == RESULT_OK;
}

void APC1::clear()
{
    this->io.clear(this->io.config);
}

void APC1::reset()
{
    Apc1_Reset(this);
}

bool APC1::isConnected()
{
    return Apc1_IsConnected(this);
}

bool APC1::setOperatingMode(const Apc1_OperatingMode& mode)
{
    return Apc1_SetOperatingMode(this, mode) == RESULT_OK;
}

bool APC1::setMeasurementMode(const Apc1_MeasurementMode& mode)
{
    return Apc1_SetMeasurementMode(this, mode) == RESULT_OK;
}

Result APC1::update()
{
    return Apc1_Update(this);
}

uint16_t APC1::getPM_1_0()
{
    return Apc1_GetPM_1_0(this);
}

uint16_t APC1::getPM_2_5()
{
    return Apc1_GetPM_2_5(this);
}

uint16_t APC1::getPM_10()
{
    return Apc1_GetPM_10(this);
}

uint16_t APC1::getPMInAir_1_0()
{
    return Apc1_GetPMInAir_1_0(this);
}

uint16_t APC1::getPMInAir_2_5()
{
    return Apc1_GetPMInAir_2_5(this);
}

uint16_t APC1::getPMInAir_10()
{
    return Apc1_GetPMInAir_10(this);
}

uint16_t APC1::getNoParticles_0_3()
{
    return Apc1_GetNoParticles_0_3(this);
}

uint16_t APC1::getNoParticles_0_5()
{
    return Apc1_GetNoParticles_0_5(this);
}

uint16_t APC1::getNoParticles_1_0()
{
    return Apc1_GetNoParticles_1_0(this);
}

uint16_t APC1::getNoParticles_2_5()
{
    return Apc1_GetNoParticles_2_5(this);
}

uint16_t APC1::getNoParticles_5_0()
{
    return Apc1_GetNoParticles_5_0(this);
}

uint16_t APC1::getNoParticles_10()
{
    return Apc1_GetNoParticles_10(this);
}

uint16_t APC1::getTVOC()
{
    return Apc1_GetTVOC(this);
}

uint16_t APC1::getECO2()
{
    return Apc1_GetECO2(this);
}

uint16_t APC1::getNO2()
{
    return Apc1_GetNO2(this);
}

float APC1::getCompT()
{
    return Apc1_GetCompT(this);
}

float APC1::getCompRH()
{
    return Apc1_GetCompRH(this);
}

float APC1::getRawT()
{
    return Apc1_GetRawT(this);
}

float APC1::getRawRH()
{
    return Apc1_GetRawRH(this);
}

uint32_t APC1::getRS0()
{
    return Apc1_GetRS0(this);
}

uint32_t APC1::getRS1()
{
    return Apc1_GetRS1(this);
}

uint32_t APC1::getRS2()
{
    return Apc1_GetRS2(this);
}

uint32_t APC1::getRS3()
{
    return Apc1_GetRS3(this);
}

AirQualityIndex_UBA APC1::getAQI()
{
    return Apc1_GetAQI(this);
}

uint16_t APC1::getFirmwareVersion()
{
    return Apc1_GetFirmwareVersion(this);
}

Apc1_ErrorCode APC1::getError()
{
    return Apc1_GetError(this);
}