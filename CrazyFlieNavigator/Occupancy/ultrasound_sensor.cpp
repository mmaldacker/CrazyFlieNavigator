#include "ultrasound_sensor.h"
#include <cmath>

SimpleUltrasoundSensorModel::SimpleUltrasoundSensorModel(float dir, float dist) : direction(dir), distance(dist)
{

}

InverseSensorModel::Location rotate_point(float angle, float distance)
{
    InverseSensorModel::Location p;

    p.x = distance * std::cos(angle);
    p.y = distance * std::sin(angle);

    return p;
}

std::vector<InverseSensorModel::Location> SimpleUltrasoundSensorModel::GetLogOdds() const
{
    std::vector<InverseSensorModel::Location> locations;

    for(int i = -10 ; i < 10 ; i++)
    {
        float angle = i * 0.01745329252 + direction;
        auto p = rotate_point(angle, distance);
        p.logOdd = std::log(0.8/0.2);
        locations.push_back(p);
    }

    return locations;
}
