#include <cstdint>

enum class Direction
{
    Stop,
    Forward,
    Back
};

enum class TurnDirection
{
    Left,
    Right,
    Line,
};

class ControlPacket
{
public:
    static const uint8_t packetSize{ 4 };

    ControlPacket(){};
    ~ControlPacket() = default;
    ControlPacket(const ControlPacket &) = default;
    ControlPacket(int speed, int direction); // +-100

    bool isDefault() const;

    void rawData(char *data) const;

    void setSpeed(int speed);
    void setDirection(int dierction);

    Direction speedState() const { return _speedState; }
    TurnDirection directionState() const { return _directionState; }

    int speed() const { return _speed; }
    int direction() const { return _direction; }

    static bool parsePacket(char *dataBuffer, int size, ControlPacket *packet);

private:
    static const uint8_t BeginPacketByte{ 255 };
    static const uint8_t EndPacketByte{ 204 };
    static const int SpeedIndex{ 1 };
    static const int DirectionIndex{ 2 };

    Direction _speedState{ Direction::Stop };
    int _speed{ 0 };

    TurnDirection _directionState{ TurnDirection::Line };
    int _direction{ 0 };
};
