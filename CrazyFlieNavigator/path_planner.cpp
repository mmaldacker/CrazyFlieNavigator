#include "path_planner.h"

PathPlanner::PathPlanner(int m, Position p)
    : multiplier(m)
    , position(p)
    , coro(std::bind(&PathPlanner::Loop, this, std::placeholders::_1))
    , startMove(std::chrono::steady_clock::now())
    , duration(std::chrono::milliseconds(0))
{
}

void PathPlanner::Loop(boost::coroutines::asymmetric_coroutine<CommanderMsg>::push_type& sink)
{
    int roll_direction = 0, pitch_direction = 0;
    int distance_multiplier = 1;
    int spiral_orientation = 0;

    int pitch_a[] = {1,0,-1,0};
    int roll_a[] = {0,1,0,-1};

    // found empirically to move the quadrocopter by 5cm
    const float amount = 560.0;

    CommanderMsg rot;
    // found empirically to turn 10 degrees
    rot.yaw = 40.0;

    while(true)
    {
        CommanderMsg msg;
        msg.pitch = pitch_direction * multiplier * distance_multiplier * amount;
        msg.roll = roll_direction * multiplier * distance_multiplier * amount;

        position.x += distance_multiplier * pitch_direction;
        position.y += distance_multiplier * roll_direction;

        distance_multiplier *= 2;

        pitch_direction = pitch_a[spiral_orientation];
        roll_direction = roll_a[spiral_orientation];
        spiral_orientation = (spiral_orientation+1) % 4;

        startMove = std::chrono::steady_clock::now();
        duration = std::chrono::milliseconds(500);
        sink(msg);

        for(int i = 0 ; i < 36 ; i++)
        {
            startMove = std::chrono::steady_clock::now();
            duration = std::chrono::milliseconds(100);
            sink(rot);
        }
    }
}

CommanderMsg PathPlanner::GetNextCommand()
{
    coro();
    return coro.get();
}

bool PathPlanner::IsMoving() const
{
    return std::chrono::steady_clock::now() - startMove < duration;
}

PathPlanner::Position PathPlanner::GetPosition() const
{
    return position;
}
