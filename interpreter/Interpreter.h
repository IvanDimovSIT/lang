#pragma once
#include <map>
#include <memory>
#include <stack>
#include <thread>
#include "../token/Token.h"
#include "RuntimeErrorType.h"
#include "ProgramSymbols.h"
#include "../scanner/Function.h"

class IRuntimeErrorReporter{
public:
    virtual void report(RuntimeErrorType errorType) = 0;
};

class IInterpreterIO{
public:
    virtual std::unique_ptr<Value> read() = 0;
    virtual void write(Value& value) = 0;
    virtual std::unique_ptr<Value> readText() = 0;
    virtual void writeText(Value& value) = 0;
};

struct LoopReturn{
    std::vector<Token*> condition;
    int loopStart;
    int loopEnd;
};

class Interpreter{
public:
    Interpreter(IRuntimeErrorReporter* errorReporter, IInterpreterIO* interpreterIO);
    bool execute(std::vector<Token*> &tokens, std::map<std::string, Function>& functions, Value& result);

private:
    bool execute(
        std::vector<Token*> &tokens,
        ProgramSymbols& programSymbols,
        Value& left,
        Value& right,
        Value& result);

    void executeOnThread(
        std::vector<Token*> tokens,
        ProgramSymbols& programSymbols,
        Value left,
        Value right);

    inline bool checkForCalculation(
        std::vector<Token*> &tokens,
        int& position,
        ProgramSymbols& programSymbols,
        Value& left,
        Value& right,
        Token*& operation,
        std::unique_ptr<Value>& leftParameter,
        std::unique_ptr<Value>& rightParameter);

    std::unique_ptr<Value> getNextArgument(
        int& position,
        std::vector<Token*> &tokens,
        ProgramSymbols& programSymbols,
        Value& left,
        Value& right,
        bool& hadError);

    // returns false if not an operation
    bool getOperatorOrFunctionParamerters(Token& operation, bool& leftParam, bool& rightParam, ProgramSymbols& programSymbols);

    std::unique_ptr<Value> executeOperationOrFunction(
        Value& leftOfOperator,
        Value& rightOfOperator,
        Token& operation,
        Value& left,
        Value& right,
        ProgramSymbols& programSymbols,
        bool& hadError);

    bool isFunctionWithoutParameters(Token& function, ProgramSymbols& programSymbols);

    bool executeAsync(
        std::vector<Token*> &tokens,
        int& position,
        ProgramSymbols& programSymbols,
        Value& left,
        Value& right);

    inline std::unique_ptr<Value> executeModifier(
        Value& leftParameter,
        std::vector<Token*> &tokens,
        ProgramSymbols& programSymbols,
        Value& left,
        Value& right,
        int position,
        bool& hadError);

    inline void setVariable(const Value& value, const std::string& variableName, ProgramSymbols& programSymbols);
    inline void getVariable(Value& value, const std::string& variableName, ProgramSymbols& programSymbols);

    void initVariables(std::vector<Token*> &tokens, ProgramSymbols& programSymbols);
    void joinThreads(ProgramSymbols& programSymbols);
private:
    IRuntimeErrorReporter* errorReporter;
    IInterpreterIO* interpreterIO;
};