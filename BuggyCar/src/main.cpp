#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Servo.h>
#include <WiFiUdp.h>

#include "../../Common/WiFiConfig.h"
#include "controlpacket.h"

const IPAddress localIp(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);
WiFiUDP Udp;

Servo servoDirection;
Servo servoSpeed;
constexpr uint8_t PinServoTurnDirection{ D1 };
constexpr uint8_t PinSpeedDriver{ D2 };

void setup()
{
    Serial.begin(9600);

    WiFi.begin(ssid, pw);
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(localIp, localIp, subnet);
    WiFi.softAP(ssid, pw);
    Udp.begin(port);
    pinMode(D4, OUTPUT);

    servoSpeed.attach(PinSpeedDriver);
    servoDirection.attach(PinServoTurnDirection);
}

bool readPacket(ControlPacket *packet)
{
    constexpr int BufferSize = 255;
    static char BufferRaw[BufferSize];

    const bool hasPacketData = Udp.parsePacket() > 0;
    digitalWrite(D4, hasPacketData ? HIGH : LOW);

    if (!hasPacketData)
        return false;
    
    const int len = Udp.read(BufferRaw, BufferSize);
    Serial.println(String(len) + String((int)(BufferRaw[1])) + "     " + String((int)(BufferRaw[2])));
    return ControlPacket::parsePacket(BufferRaw, len, packet);
}

void updateCarState(const ControlPacket &packet)
{
    ///////////////////////////////////////////////////////////
    constexpr int ServoCalibrationLeft = 880;
    constexpr int ServoCalibrationLine = 1140;
    constexpr int ServoCalibrationRight = 1400;

    constexpr int ServoCalibrationRightDiff = ServoCalibrationRight - ServoCalibrationLine;
    constexpr int ServoCalibrationLeftDiff = ServoCalibrationLine - ServoCalibrationLeft;

    int turnValue = ServoCalibrationLine;

    if (packet.directionState() == TurnDirection::Left)
        turnValue -= ServoCalibrationRightDiff * (packet.direction() / 100.0);

    if (packet.directionState() == TurnDirection::Right)
        turnValue += ServoCalibrationLeftDiff * (packet.direction() / 100.0);

    servoDirection.writeMicroseconds(turnValue);

    ///////////////////////////////////////////////////////////////////////////////

    constexpr int ServoCalibrationStop = 1500;
    constexpr int ServoCalibrationMinForward = 1530;
    constexpr int ServoCalibrationMaxForward = 2200;
    constexpr int ServoCalibrationForwardRange = ServoCalibrationMaxForward
                                                 - ServoCalibrationMinForward;

    constexpr int ServoCalibrationMinBack = 1470;
    constexpr int ServoCalibrationMaxBack = 800;
    constexpr int ServoCalibrationBackRange = ServoCalibrationMaxBack - ServoCalibrationMinBack;

    constexpr double SpeedMultipler = 0.5;

    int speedValue = ServoCalibrationStop;

    if (packet.speedState() == Direction::Forward)
    {
        int speed = ServoCalibrationForwardRange * (packet.speed() / 100.0);
        speed = static_cast<int>(speed * SpeedMultipler);
        speedValue = ServoCalibrationMinForward + speed;
    }
    if (packet.speedState() == Direction::Back)
    {
        int speed = (ServoCalibrationBackRange) * (packet.speed() / 100.0);
        speed = static_cast<int>(speed * SpeedMultipler);
        speedValue = ServoCalibrationMinBack + speed;
    }

    servoSpeed.writeMicroseconds(speedValue);
}

void loop()
{
    static unsigned long priviousMilis = millis();
    static ControlPacket lastAppliedPacket;

    ControlPacket packet;
    if (readPacket(&packet))
    {
        priviousMilis = millis();
        updateCarState(packet);
        lastAppliedPacket = packet;
    }

    if (millis() - priviousMilis > 250)
    {
        ControlPacket packet = lastAppliedPacket;
        packet.setSpeed(0);
        updateCarState(packet);
        priviousMilis = millis();
    }
}
