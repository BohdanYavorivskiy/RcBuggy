#pragma once

#include "controlsvalue.h"

#include <Arduino.h>

namespace ControlStateReader
{
constexpr uint8_t TpSpeedLow = D5;
constexpr uint8_t TpSpeedHigh = D6;
constexpr uint8_t TpDirectionLow = D1;
constexpr uint8_t TpDirectionHigh = D2;

void setupCtrls();

int measureVoltage(int sample);
std::pair<int, int> measureContolsVoltages(int sample);
ControlsValue getContolsVoltages();
} // namespace ControlStateReader