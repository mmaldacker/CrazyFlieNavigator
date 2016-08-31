#ifndef log_h
#define log_h

#include "crtp.h"
#include "serial.h"
#include "variable.h"

class Log
{
public:
    Log(ArduinoSerial & serial);

    void RequestTableOfContent();
    void RequestVariableNames();

    bool HasTableOfContent();

    void CreateBlock(uint8_t id, const std::vector<std::string> & names);
    void StartBlock(uint8_t id, uint8_t period);
    void ResetBlock();

    void Process(const LogMsg & msg);

    const Variable & GetVariable(const std::string & name);

private:
    ArduinoSerial & serial;
    uint8_t numVariables;
    std::map<uint8_t, Variable> variables;
    std::map<uint8_t, std::vector<Variable>> blocks;
};

#endif
