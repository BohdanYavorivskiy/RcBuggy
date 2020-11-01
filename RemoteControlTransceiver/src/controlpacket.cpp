#include "controlpacket.h"
#include <Arduino.h>

static int rountToHundred(int val)
{
    if (val >= 100)
        return 100;
    if (val <= 0)
        return 0;
    return val;
};

bool ControlPacket::parsePacket(char *dataBuffer, int size, ControlPacket *packet)
{
    // convert raw bytes into EngineControlData
    // raw data: 0b XYYY YYYY   0b KNNN NNNN
    // X: '1' - Direction::Forward; '0' - Direction::Back;
    // YYY YYYY: - engine power [0; 127]
    // K: '1' - TurnDirection::Right; '0' - TurnDirection::Left;
    // NNN NNNN: - turn wheels

    if (size < ControlPacket::packetSize)
        return false;

    uint8_t rawData[ControlPacket::packetSize];
    bool packetFound = false;

    for (int i = 0; i <= size - ControlPacket::packetSize; ++i)
    {
        if (dataBuffer[i] == ControlPacket::BeginPacketByte
            && dataBuffer[i + 3] == ControlPacket::EndPacketByte)
        {
            memcpy(rawData, dataBuffer + i, ControlPacket::packetSize);
            packetFound = true;
            break;
        }
    }

    if (!packetFound)
        return false;

    int speed = rawData[1] & 0x7F;
    speed *= (rawData[1] & 0x80) ? 1 : -1;

    int direction = rawData[2] & 0x7F;
    direction *= (rawData[2] & 0x80) ? 1 : -1;

    *packet = ControlPacket(speed, direction);
    return true;
}

ControlPacket::ControlPacket(int speed, int direction)
{
    setSpeed(speed);
    setDirection(direction);
}

void ControlPacket::setSpeed(int speed)
{
    _speed = rountToHundred(abs(speed));
    if (speed == 0)
        _speedState = Direction::Stop;
    else
        _speedState = speed > 0 ? Direction::Forward : Direction::Back;
}
void ControlPacket::setDirection(int direction)
{
    _direction = rountToHundred(abs(direction));
    if (direction == 0)
        _directionState = TurnDirection::Line;
    else
        _directionState = direction > 0 ? TurnDirection::Right : TurnDirection::Left;
}

bool ControlPacket::isDefault() const
{
    ControlPacket defaultObj;
    return defaultObj.speedState() == speedState()
           && defaultObj.directionState() == directionState() && defaultObj.speed() == speed()
           && defaultObj.direction() == direction();
}

void ControlPacket::rawData(char *data) const
{
    data[0] = BeginPacketByte;
    data[3] = EndPacketByte;

    data[1] = static_cast<uint8_t>(_speed);
    data[1] |= _speedState == Direction::Forward ? 0x80 : 0x00;

    data[2] = static_cast<uint8_t>(_direction);
    data[2] |= _directionState == TurnDirection::Right ? 0x80 : 0x00;
}
