#include "log.h"
#include <iostream>

Log::Log(ArduinoSerial & serial) : serial(serial), numVariables(0)
{
    
}

void Log::RequestTableOfContent()
{
    LogMsg msg(LogChannel::TableOfContent);
    msg.content[0] = 0x01; //CMD_GET_INFO
    
    serial.send(msg, 2);
}

void Log::RequestVariableNames()
{
    LogMsg msg(LogChannel::TableOfContent);
    msg.content[0] = 0x00; //CMD_GET_ITEM

    for(uint8_t i = 0 ; i < numVariables ; i++)
    {
        msg.content[1] = i;

        serial.send(msg, 3);
    }
}

bool Log::HasTableOfContent()
{
    return numVariables == variables.size() && !variables.empty();
}

void Log::CreateBlock(uint8_t id, const std::vector<std::string> & names)
{
    LogMsg msg(LogChannel::Settings);
    msg.content[0] = 0x00; // CONTROL_CREATE_BLOCK
    msg.content[1] = id;

    std::vector<Variable> blockVariables;

    uint8_t i = 2;
    for(auto && name : names)
    {
        auto variable = GetVariable(name);
        msg.content[i++] = variable.type;
        msg.content[i++] = variable.index;

        blockVariables.push_back(variable);
    }

    blocks[id] = blockVariables;

    serial.send(msg, i+1);
}

void Log::StartBlock(uint8_t id, uint8_t period)
{
    LogMsg msg(LogChannel::Settings);
    msg.content[0] = 0x03; // CONTROL_START_BLOCK
    msg.content[1] = id;
    msg.content[2] = period;

    serial.send(msg, 4);
}

void Log::ResetBlock()
{
    LogMsg msg(LogChannel::Settings);
    msg.content[0] = 0x05; // CONTROL_RESET

    serial.send(msg, 2);
}

void Log::Process(const LogMsg & msg)
{
    Port port;
    LogChannel channel;
    get_crtp_header(msg.header, port, channel);
    switch(channel)
    {
        case LogChannel::TableOfContent:
        {
            if(msg.content[0] == 0x00) // CMD_GET_ITEM
            {
                Variable v(msg.content);
                variables[v.index] = v;
                std::cout << "[LOG] " << v << std::endl;
            }
            else if(msg.content[0] == 0x01) // CMD_GET_INFO
            {
                numVariables = msg.content[1];
                std::cout << "[LOG] Total number of variable " << (int)numVariables << std::endl;
                RequestVariableNames();
            }
            break;
        }
        case LogChannel::Settings:
        {
            std::cout << "[LOG] Command " << (int)msg.content[0] << " BlockID " << (int)msg.content[1] << " Status " << (int)msg.content[2] << std::endl;
            break;
        }
        case LogChannel::Data:
        {
            auto & blockVariables = blocks[msg.content[0]];
            std::cout << "[LOG] Block ID " << (int)msg.content[0] << std::endl;

            const uint8_t * location = msg.content + 4;
            for(auto && variable : blockVariables)
            {
                location = GetLogValue(variable.type, location, variable.value);
                std::cout << "\t" << variable.Name() << ": " << variable.value << std::endl;
            }
            break;
        }
    }
}

const Variable & Log::GetVariable(const std::string & name)
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