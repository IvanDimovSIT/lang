#pragma once
#include <unordered_map>
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
    std::unordered_map<std::string, Function> functions;
    std::unordered_map<std::string, Variable> variables;
    std::vector<std::thread> threads;
    std::mutex IOReadLock;
    std::mutex IOWriteLock;
};
