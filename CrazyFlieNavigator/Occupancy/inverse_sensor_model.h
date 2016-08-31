#ifndef inverse_sensor_model_h
#define inverse_sensor_model_h

#include <vector>

struct InverseSensorModel
{
    struct Location
    {
        int x,y;
        double logOdd;
    };

    virtual std::vector<Location> GetLogOdds() const = 0;
};


#endif
