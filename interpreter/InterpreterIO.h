#pragma once
#include "Interpreter.h"

class InterpreterIO : public IInterpreterIO{
public:
    std::unique_ptr<Value> read() override;
    
    void write(const Value& value) override;
    
    std::unique_ptr<Value> readText() override;
    
    void writeText(const Value& value) override;

private:
    static const char CHARS_START = '\t';
    
    static const char CHARS_END = '~';
};
