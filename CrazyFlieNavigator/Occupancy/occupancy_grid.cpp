#include "occupancy_grid.h"
#include <cmath>

OccupancyGrid::OccupancyGrid(int s, float logPrior) : size(s)
{
    grid.resize(size);
    for(auto && g : grid) g.resize(size, logPrior);
}

void OccupancyGrid::AddReading(int x, int y, const InverseSensorModel & model)
{
    auto locations = model.GetLogOdds();
    for(auto && location : locations)
    {
        int x_sensor = x + location.x;
        int y_sensor = y + location.y;
        if(x_sensor >= 0 && x_sensor < size && y_sensor > 0 && y_sensor < size)
        {
            grid[x_sensor][y_sensor] += location.logOdd;
        }
    }
}

int OccupancyGrid::GetSize() const
{
    return size;
}

bool OccupancyGrid::IsFilled(int x, int y) const
{
    double p = 1.0 - 1.0/(1.0 - std::exp(grid[x][y]));
    return p > 0.9;
}
