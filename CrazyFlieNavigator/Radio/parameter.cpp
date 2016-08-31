#include "parameter.h"
#include <iostream>

Parameter::Parameter(ArduinoSerial & serial) : serial(serial), numVariables(0)
{
}

void Parameter::RequestTableOfContent()
{
    ParameterMsg msg(ParameterChannel::TableOfContent);
    msg.content[0] = 0x01; //CMD_GET_INFO

    serial.send(msg, 2);
}

void Parameter::RequestVariableNames()
{
    ParameterMsg msg(ParameterChannel::TableOfContent);
    msg.content[0] = 0x00; //CMD_GET_ITEM

    for(uint8_t i = 0 ; i < numVariables ; i++)
    {
        msg.content[1] = i;
        
        serial.send(msg, 3);
    }
}

bool Parameter::HasTableOfContent()
{
    return numVariables == variables.size() && !variables.empty();
}

void Parameter::PrintVariable(const std::string & name)
{
    ParameterMsg msg(ParameterChannel::Read);
    const auto & var = GetVariable(name);
    msg.content[0] = var.index;

    serial.send(msg, 2);
}

void Parameter::SetVariable(const std::string & name, float value)
{
    ParameterMsg msg(ParameterChannel::Write);
    const auto & var = GetVariable(name);
    msg.content[0] = var.index;
    uint8_t length = SetParamValue(var.type, value, &msg.content[1]);

    serial.send(msg, 2 + length);
}

void Parameter::Process(const ParameterMsg & msg)
{
    Port port;
    ParameterChannel channel;
    get_crtp_header(msg.header, port, channel);
    switch(channel)
    {
        case ParameterChannel::TableOfContent:
        {
            if(msg.content[0] == 0x00) // CMD_GET_ITEM
            {
                Variable v(msg.content);
                variables[v.index] = v;
                v.type &= 0b00001111;
                std::cout << "[PARAM] " << v << std::endl;
            }
            else if(msg.content[0] == 0x01) // CMD_GET_INFO
            {
                numVariables = msg.content[1];
                std::cout << "[PARAM] Total number of variable " << (int)numVariables << std::endl;
                RequestVariableNames();
            }
            break;
        }
        case ParameterChannel::Read:
        {
            const auto & var = variables[msg.content[0]];
            std::cout << "[PARAM] " << var.Name() << ": " << GetParamValue(var.type, &msg.content[1]) << std::endl;
            break;
        }
        case ParameterChannel::Write:
        {
            break;
        }
    }
}

const Variable & Parameter::GetVariable(const std::string & name)
{
    for(const auto & variable : variables)
    {
        if(variable.second.Name() == name)
        {
            return variable.second;
        }
    }

    throw std::runtime_error("Variable not found: " + name);
}
