#ifndef crtp_h
#define crtp_h

#include <cstdint>

enum class Port : uint8_t
{
    Console = 0,
    Parameters = 2,
    Commander = 3,
    Log = 5,
    Link = 15
};

enum class LogChannel : uint8_t
{
    TableOfContent = 0,
    Settings = 1,
    Data = 2
};

enum class ParameterChannel : uint8_t
{
    TableOfContent = 0,
    Read = 1,
    Write = 2
};

inline uint8_t set_crtp_header(Port port, uint8_t channel)
{
    return ((uint8_t)port << 4) | 0b00001100 | (channel & 0x03);
}

template<typename T>
inline void get_crtp_header(uint8_t header, Port & port, T & channel)
{
    port = (Port)(header >> 4);
    channel = (T)(header & 0x03);
}

#pragma pack(1)
struct CommanderMsg
{
    CommanderMsg() : header(set_crtp_header(Port::Commander,0)) {}
    uint8_t header;
    float roll;
    float pitch;
    float yaw;
    uint16_t  thrust;
};

struct PingMsg
{
    const uint8_t header = 0xFF;
};

struct ConsoleMsg
{
    uint8_t header;
    char content[31];
};

struct LogMsg
{
    LogMsg(LogChannel channel) : header(set_crtp_header(Port::Log, (uint8_t)channel)) {}
    uint8_t header;
    uint8_t content[31];
};

struct ParameterMsg
{
    ParameterMsg(ParameterChannel channel) : header(set_crtp_header(Port::Parameters, (uint8_t)channel)) {}
    uint8_t header;
    uint8_t content[31];
};

#pragma pack()

#endif
