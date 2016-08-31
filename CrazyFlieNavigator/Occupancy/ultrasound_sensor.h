#ifndef ultrasound_sensor_h
#define ultrasound_sensor_h

#include "inverse_sensor_model.h"

/*
 Very simple model of the ultra sound, this simply sets a probability on the circle arc within 10degrees.
 More complex models can be added and compared, 
 such as one that sets different probabilities given the distance as well
 */
class SimpleUltrasoundSensorModel : public InverseSensorModel
{
public:
    SimpleUltrasoundSensorModel(float direction, float distance);

    std::vector<Location> GetLogOdds() const;

private:
    float direction;
    float distance;
};

#endif