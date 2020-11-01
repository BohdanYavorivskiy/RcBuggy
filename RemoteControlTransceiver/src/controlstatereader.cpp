#include "controlstatereader.h"

namespace ControlStateReader
{
void setupCtrls()
{
    pinMode(TpSpeedLow, INPUT);
    pinMode(TpSpeedHigh, INPUT);
    pinMode(TpDirectionLow, INPUT);
    pinMode(TpDirectionHigh, INPUT);
}

int measureVoltage(int sample)
{
    double volts = analogRead(A0);
    for (int i = 0; i < sample - 1; i++)
        volts += analogRead(A0);

    if (sample > 0)
        volts /= static_cast<double>(sample);

    return volts;
}

std::pair<int, int> measureContolsVoltages(int sample)
{
    if (sample <= 0)
        sample = 1;

    int speedRaw = 0;
    int directionRaw = 0;

    for (int i = 0; i < sample; ++i)
    {
        pinMode(TpSpeedLow, OUTPUT);
        pinMode(TpSpeedHigh, OUTPUT);
        digitalWrite(TpSpeedLow, LOW);
        digitalWrite(TpSpeedHigh, HIGH);

        delayMicroseconds(50);
        speedRaw += measureVoltage(5);

        pinMode(TpSpeedLow, INPUT);
        pinMode(TpSpeedHigh, INPUT);

        pinMode(TpDirectionLow, OUTPUT);
        pinMode(TpDirectionHigh, OUTPUT);
        digitalWrite(TpDirectionLow, LOW);
        digitalWrite(TpDirectionHigh, HIGH);

        delayMicroseconds(50);
        directionRaw += measureVoltage(5);

        pinMode(TpDirectionLow, INPUT);
        pinMode(TpDirectionHigh, INPUT);
    }

    return { speedRaw / static_cast<double>(sample), directionRaw / static_cast<double>(sample) };
}

ControlsValue getContolsVoltages()
{
    const std::pair<int, int> voltages = measureContolsVoltages(1);
    const int speedRaw = voltages.first;
    const int directionRaw = voltages.second;

    int speed = 0;
    int direction = 0;
    // Serial.println(String(speedRaw) + "\t" + String(directionRaw));

    //////////////////////////////////////////////////////////////
    {
        constexpr double speedMax = 1000;
        constexpr double speedZero = 545;
        constexpr double speedMin = 90;

        constexpr double zeroMargins = 0;

        double diffWithDefault = speedRaw - speedZero;

        if (abs(diffWithDefault) < zeroMargins)
        {
            speed = 0;
        }
        else if (diffWithDefault > 0)
        {
            diffWithDefault -= zeroMargins;
            constexpr double speedMaxDiff = (speedZero - speedMax) + zeroMargins;
            speed = -(diffWithDefault / speedMaxDiff) * 100;
        }
        else if (diffWithDefault < 0)
        {
            diffWithDefault += zeroMargins;
            constexpr double speedMinDiff = (speedZero - speedMin) - zeroMargins;
            speed = (diffWithDefault / speedMinDiff) * 100;
        }
    }

    /////////////////////////////////////////////////////////////////
    {
        constexpr double leftDirection = 90;
        constexpr double zeroDirection = 545;
        constexpr double rightDirection = 1000;

        constexpr double zeroMargins = 0;

        double diffWithDefault = directionRaw - zeroDirection;

        if (abs(diffWithDefault) < zeroMargins)
        {
            direction = 0;
        }
        else if (diffWithDefault > 0)
        {
            diffWithDefault -= zeroMargins;
            constexpr double speedMaxDiff = (zeroDirection - rightDirection) + zeroMargins;
            direction = -(diffWithDefault / speedMaxDiff) * 100;
        }
        else if (diffWithDefault < 0)
        {
            diffWithDefault += zeroMargins;
            constexpr double speedMinDiff = (zeroDirection - leftDirection) - zeroMargins;
            direction = (diffWithDefault / speedMinDiff) * 100;
        }
    }

    return ControlsValue(speed, direction);
}
} // namespace ControlStateReader