#pragma once
#include <map>
#include <vector>
#include "../scanner/Function.h"
#include "../token/Token.h"

struct ProgramSymbols{
    std::map<std::string, Function> functions;
    std::map<std::string, Value> variables;
};
