#include "controlsvalue.h"

ControlsValue::ControlsValue(int speed, int direction)
{
    const auto rountToHundred = [this](int val) -> int {
        if (val <= -100)
            return -100;
        if (val >= 100)
            return 100;
        return val;
    };
    _speed = rountToHundred(speed);
    _direction = rountToHundred(direction);
}