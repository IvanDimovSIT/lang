#pragma once
#include <map>
#include <vector>
#include <mutex>
#include <thread>
#include "../scanner/Function.h"
#include "../token/Token.h"


struct Variable{
    Value value;
    std::mutex mut;
};

// global program state
struct ProgramState{
    std::map<std::string, Function> functions;
    std::map<std::string, Variable> variables;
    std::vector<std::thread> threads;
    std::mutex IOReadLock;
    std::mutex IOWriteLock;
};
