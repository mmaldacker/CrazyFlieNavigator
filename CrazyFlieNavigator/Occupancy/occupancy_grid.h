#ifndef OccupancyGrid_h
#define OccupancyGrid_h

#include <vector>
#include "inverse_sensor_model.h"

/*
 Class to represent an occupancy grid in log form
 It is updated using an inverse sensor model and position.\
*/
class OccupancyGrid
{
public:
    // creates a square grid with size*size number of grids
    OccupancyGrid(int size, float logPrior = -6.0);

    void AddReading(int x, int y, const InverseSensorModel & model);

    int GetSize() const;
    bool IsFilled(int x, int y) const;

private:
    std::vector<std::vector<double>> grid;
    int size;
};

#endif