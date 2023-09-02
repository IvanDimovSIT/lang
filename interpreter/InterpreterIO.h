#pragma once
#include "Interpreter.h"

class InterpreterIO : public IInterpreterIO{
public:
    std::unique_ptr<Value> read() override;
    void write(Value& value) override;
    std::unique_ptr<Value> readText() override;
    void writeText(Value& value) override;
};
