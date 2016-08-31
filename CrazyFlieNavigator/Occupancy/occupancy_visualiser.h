#ifndef occupancy_visualiser_h
#define occupancy_visualiser_h

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "occupancy_grid.h"

class OccupancyVisualiser
{
public:
    OccupancyVisualiser(const OccupancyGrid & grid, int scale = 3);

    bool IsOpen() const;
    void Update();

private:
    sf::RenderWindow window;
    const OccupancyGrid & grid;
    int scale;
};

#endif
