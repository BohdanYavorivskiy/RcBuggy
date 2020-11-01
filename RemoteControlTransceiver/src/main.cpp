#include <Arduino.h>

#include "../../Common/WiFiConfig.h"
#include "controlstatereader.h"

#include "controlpacket.h"

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

WiFiUDP Udp;

void setup()
{
    Serial.begin(9600);

    WiFi.begin(ssid, pw);
    pinMode(D4, OUTPUT);

    ControlStateReader::setupCtrls();
}

void loop()
{
    const bool isConnectedClient = WiFi.isConnected();
    digitalWrite(D4, isConnectedClient ? HIGH : LOW);

    if (!isConnectedClient)
        return;

    const ControlsValue values = ControlStateReader::getContolsVoltages();
    ControlPacket packet(values.speed(), values.direction());
    delay(3); // fix something ptoblem after measure voltage. Resceiver dont ger signal.
    char data[ControlPacket::packetSize];
    packet.rawData(data);

    Udp.beginPacket(WiFi.gatewayIP(), port);
    Udp.write(data, ControlPacket::packetSize);
    Udp.endPacket();

    // digitalWrite(D4, LOW);
    // Serial.println(String((int)(data[1])) + "     " + String((int)(data[2])));
}