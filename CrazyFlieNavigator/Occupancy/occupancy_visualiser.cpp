#include "occupancy_visualiser.h"
#include <SFML/Graphics.hpp>

OccupancyVisualiser::OccupancyVisualiser(const OccupancyGrid & g, int s)
    : window(sf::VideoMode(g.GetSize()*s, g.GetSize()*s), "Visualiser")
    , grid(g)
    , scale(s)
{
}

bool OccupancyVisualiser::IsOpen() const
{
    return window.isOpen();
}

void OccupancyVisualiser::Update()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
    }

    window.clear(sf::Color::White);

    sf::RectangleShape rect(sf::Vector2f(scale, scale));
    rect.setFillColor(sf::Color::Black);

    for(int x = 0 ; x < grid.GetSize() ; x++)
    {
        for(int y = 0 ; y < grid.GetSize() ; y++)
        {
            rect.setPosition(x, y);
            window.draw(rect);
        }
    }
}
