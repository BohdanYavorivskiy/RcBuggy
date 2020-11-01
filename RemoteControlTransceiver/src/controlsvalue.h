#pragma once

#include <Arduino.h>

struct ControlsValue
{
public:
    ControlsValue(int speed, int direction);

    int speed() const { return _speed; };
    int direction() const { return _direction; };

private:
    int _speed = 0;
    int _direction = 0;
};