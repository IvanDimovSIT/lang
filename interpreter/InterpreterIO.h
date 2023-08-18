#pragma once
#include "Interpreter.h"

class InterpreterIO : IInterpreterIO{
public:
    std::unique_ptr<std::vector<double>> read() override;
    void write(std::vector<double>& value) override;
};
