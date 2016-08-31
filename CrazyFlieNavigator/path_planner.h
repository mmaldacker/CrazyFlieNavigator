#ifndef path_planner_h
#define path_planner_h

#include "crtp.h"
#include <boost/coroutine/coroutine.hpp>
#include <chrono>

/*
 Get quadrocopter commands to move in square spiral
 And pause in between to turn 360 degrees
 */
class PathPlanner
{
public:
    struct Position
    {
        int x, y;
    };

    // multiples of 5cm
    PathPlanner(int multipler, Position p);

    // returns the next command which can be move in the spiral or turn on itself
    CommanderMsg GetNextCommand();

    bool IsMoving() const;
    Position GetPosition() const;
    
private:
    void Loop(boost::coroutines::asymmetric_coroutine<CommanderMsg>::push_type& sink);

    int multiplier;
    Position position;

    boost::coroutines::pull_coroutine<CommanderMsg> coro;

    std::chrono::time_point<std::chrono::steady_clock> startMove;
    std::chrono::duration<double> duration;
};

#endif