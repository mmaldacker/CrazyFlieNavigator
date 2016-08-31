# CrazyFlieNavigator

Using the [CrazyFlie](https://www.bitcraze.io) quadrocopter, this projects aims to try to map an indoor environment autonomously. 

This is a work in progress.

## Introduction

Since the CrazyFlie is expendable, I've soldered on two cheap and lightweight ultrasound sensors (SFR02). One is mounted horizontally and one is mounted vertically (on top) of the quadrocopter. 

The vertical sensor is used to maintain height, by using the CrazyFlie onboard PID controller. The horizontal sensor is used to map the environment.

The quadrocopter uses bluetooth and radio for communication, I've created a simple usb radio contraption by putting together a nRF24L01+ module and an arduino nano. The arduino nano then simply copies the data from serial to the RF24 and back. Commands can then be send and received on a computer with serial communication over USB.

The quadrocopter is then controller by a computer that will issue commands to explore a room, and using the returned ultra sound sensor data, create a map of said room.

## Radio

The radio section implements the [CRTP](https://wiki.bitcraze.io/projects:crazyflie:crtp) protocol used to communicate with the quadrocopter. Only the following is implemented:

* Console: used to receive debug messages from the quadrocopter
* Parameter: used to update internal parameter of the quadrocopter
* Log: used to retrieve sensor data
* Commander: used to send move commands to the quadrocopter

## Occupancy Mapping

This is a simple algorithm used to map an environment. The algorithm represents the environment as a grid with each cell representing a probability of being occupied or empty. The ultrasound sensors gives a probability of there being an object with a circle arc (due to the imprecision of the sensor). By moving the quadrocopter around we can get several measurements of the environment from different positions and get a somewhat accurate map of the environment

By setting a cutoff, say 0.9, we can then graphically represent a map of the environment with the obstacles shown.

### Known position 

The occupancy mapping algorithm requires precise position of the quadrocopter. However this is difficult to achieve with a quadrocopter as small air perturbation modify the position of the quadrocopter and the onboard sensors (gyroscope and accelerometer) are not precise enough to compensate. 

Bitcraze is currently developing a more precise [positioning system module](https://www.bitcraze.io/loco-pos-system/) which will greatly improve this.

### Navigation heuristic

Currently, the navigation is extremely simple and dumb. The quadrocopter is placed in the middle of the room and it will fly in an outwards spiral, pausing at regular interval to do a 360 degree turn on itself and measuring the ultra sound output. It then stops when it starts flying to close to a wall. 

Known position is simply extrapolated by measuring how much the quadrocopter moves given the same amount of power.
