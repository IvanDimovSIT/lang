#pragma once
#include <map>
#include "../token/Token.h"
#include "RuntimeErrorType.h"
#include "../scanner/Function.h"

class IRuntimeErrorReporter{
public:
    virtual void report(RuntimeErrorType errorType) = 0;
};

class Interpreter{
public:
    static bool execute(std::vector<Token*> &tokens, IRuntimeErrorReporter* errorReporter, std::vector<double>& result);
private:
    static bool execute(
        std::vector<Token*> &tokens,
        std::map<std::string, Function>& functions,
        std::map<std::string, std::vector<double>>& localVariables,
        std::vector<double>& left,
        std::vector<double>& right,
        std::vector<double>& result,
        IRuntimeErrorReporter* errorReporter);

    //return new position
    static int extractUpToClosedParenthesis(std::vector<Token*> &tokens, int currentPosition, std::vector<Token*>& extracted);
};