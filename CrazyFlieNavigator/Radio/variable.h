#ifndef variable_h
#define variable_h

#include <cstdint>
#include <string>
#include <ostream>

struct Variable
{
    Variable(const uint8_t content[31])
    {
        index = content[1];
        type = content[2];

        uint8_t pos = 3;
        while(content[pos]) group += content[pos++];
        pos++;
        while(content[pos]) variable += content[pos++];
    }

    Variable() : index(255), type(0)
    {}

    std::string Name() const
    {
        return group + "." + variable;
    }

    uint8_t index;
    uint8_t type;
    float value;
    std::string group;
    std::string variable;
};

inline std::ostream& operator <<(std::ostream& o, const Variable & v)
{
    o << "Index: " << (int)v.index << " Type: " << (int)v.type << " Group: " << v.group << " Variable: " << v.variable;
    return o;
}


template<typename T>
inline const uint8_t * Get(const uint8_t * location, float & result)
{
    T value;
    memcpy(&value, location, sizeof(T));
    result = value;
    return location + sizeof(T);
}

inline const uint8_t * GetLogValue(uint8_t type, const uint8_t * location, float & value)
{
    switch(type)
    {
        case 1:
        {
            return Get<uint8_t>(location, value);
        }
        case 2:
        {
            return Get<uint16_t>(location, value);
        }
        case 3:
        {
            return Get<uint32_t>(location, value);
        }
        case 4:
        {
            return Get<int8_t>(location, value);
        }
        case 5:
        {
            return Get<int16_t>(location, value);
        }
        case 6:
        {
            return Get<int32_t>(location, value);
        }
        case 7:
        {
            return Get<float>(location, value);
        }
        default:
        {
            value = 0.0;
            return location;
        }
    }
}

inline float GetParamValue(uint8_t type, const uint8_t * location)
{
    float value = 0.0;
    switch(type)
    {
        case 8:
        {
            Get<uint8_t>(location, value);
            break;
        }
        case 9:
        {
            Get<uint16_t>(location, value);
            break;
        }
        case 10:
        {
            Get<uint32_t>(location, value);
            break;
        }
        case 0:
        {
            Get<int8_t>(location, value);
            break;
        }
        case 1:
        {
            Get<int16_t>(location, value);
            break;
        }
        case 2:
        {
            Get<int32_t>(location, value);
            break;
        }
        case 6:
        {
            Get<float>(location, value);
            break;
        }
    }

    return value;
}

template<typename T>
inline uint8_t Set(uint8_t * location, float input)
{
    T value = input;
    memcpy(location, &value, sizeof(T));
    return sizeof(T);
}

inline uint8_t SetParamValue(uint8_t type, float value, uint8_t * location)
{
    switch(type)
    {
        case 8:
        {
            return Set<uint8_t>(location, value);
        }
        case 9:
        {
            return Set<uint16_t>(location, value);
        }
        case 10:
        {
            return Set<uint32_t>(location, value);
        }
        case 0:
        {
            return Set<int8_t>(location, value);
        }
        case 1:
        {
            return Set<int16_t>(location, value);
        }
        case 2:
        {
            return Set<int32_t>(location, value);
        }
        case 6:
        {
            return Set<float>(location, value);
        }
    }

    return 0;
}


#endif
