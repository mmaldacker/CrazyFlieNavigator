#ifndef parameter_h
#define parameter_h

#include "serial.h"
#include "crtp.h"
#include "variable.h"

class Parameter
{
public:
    Parameter(ArduinoSerial & serial);

    void RequestTableOfContent();
    void RequestVariableNames();

    bool HasTableOfContent();

    void PrintVariable(const std::string & name);
    void SetVariable(const std::string & name, float value);

    void Process(const ParameterMsg & msg);

private:
    ArduinoSerial & serial;
    std::map<uint8_t, Variable> variables;
    uint8_t numVariables;

    const Variable & GetVariable(const std::string & name);
};


#endif
