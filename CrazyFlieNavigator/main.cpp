#include <iostream>
#include "serial.h"
#include "crtp.h"
#include "log.h"
#include "parameter.h"
#include "ultrasound_sensor.h"
#include "occupancy_grid.h"
#include "occupancy_visualiser.h"
#include "path_planner.h"

struct Controller
{
    Controller()
    : serial("/dev/tty.usbserial-AL022GAK", 9600, std::bind(&Controller::Handler, this, std::placeholders::_1, std::placeholders::_2))
    , log(serial)
    , parameter(serial)
    {
    }

    void Process(const ConsoleMsg & msg)
    {
        std::string s(msg.content);
        std::cout << s;
    }

    void Handler(bool success, const std::vector<uint8_t> & buffer)
    {
        if(success && buffer.size() > 0)
        {
            uint8_t channel;
            Port port;
            get_crtp_header(buffer[0], port, channel);

            switch(port)
            {
                case Port::Console:
                    Process(*reinterpret_cast<const ConsoleMsg *>(buffer.data()));
                    break;
                case Port::Log:
                    log.Process(*reinterpret_cast<const LogMsg *>(buffer.data()));
                    break;
                case Port::Parameters:
                    parameter.Process(*reinterpret_cast<const ParameterMsg *>(buffer.data()));
                    break;
                default:
                    std::cout << "Unknown Port/Channel: " << (int)port << "/" << (int)channel << std::endl;
                    break;

            }
        }
    }

    void Heartbeat()
    {
        PingMsg msg;
        serial.send(msg);
    }

    ArduinoSerial serial;
    Log log;
    Parameter parameter;
};

void Rotate(Controller & controller)
{

}

int main(int argc, const char * argv[])
{
    Controller controller;
    OccupancyGrid grid(200); // 10m squared room with 5cm sized grids
    PathPlanner planner(1, {100,100}); // move by 5cm amounts, set initial position at 5m x 5m
    OccupancyVisualiser visualiser(grid);

    controller.log.RequestTableOfContent();
    while(!controller.log.HasTableOfContent())
    {
        controller.Heartbeat();
    }

    controller.parameter.RequestTableOfContent();
    while(!controller.parameter.HasTableOfContent())
    {
        controller.Heartbeat();
    }

    // print to stdout some parameters of the quadrocopter
    controller.parameter.PrintVariable("flightmode.yawMode");
    controller.parameter.PrintVariable("flightmode.althold");
    controller.parameter.PrintVariable("controller.tiltComp");
    controller.parameter.PrintVariable("distance.atlhold");

    controller.log.ResetBlock();
    // get horizontal ultrasound measurements and horizontal compass measurements every 20ms
    controller.log.CreateBlock(1, {"distance.horizontal", "mag.z"});
    controller.log.StartBlock(1, 20);

    // instruct quadroctopter to hover at 20cm from ceiling
    // using onboard PID controller
    controller.parameter.SetVariable("lps.z", 20.0);

    // Yaw is locked to world coordinates thus heading stays the same when yaw rotates
    controller.parameter.SetVariable("flightmode.yawMode", 0);

    while(visualiser.IsOpen())
    {
        float distance = controller.log.GetVariable("distance.horizontal").value;

        // simply stop when too close to a wall
        if(distance < 15.0)
        {
            break;
        }
        
        if(!planner.IsMoving())
        {
            float direction = controller.log.GetVariable("mag.z").value;

            auto p = planner.GetPosition();

            grid.AddReading(p.x, p.y, SimpleUltrasoundSensorModel(direction, distance));

            controller.serial.send(planner.GetNextCommand());
        }
        else
        {
            controller.Heartbeat();
        }

        visualiser.Update();
    }

    return 0;
}
