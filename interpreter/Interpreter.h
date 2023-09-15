#pragma once
#include <map>
#include <memory>
#include <stack>
#include <thread>
#include "../token/Token.h"
#include "RuntimeErrorType.h"
#include "ProgramState.h"
#include "../scanner/Function.h"

class IRuntimeErrorReporter{
public:
    static const int LINE_NOT_FOUND = -1;
    virtual void report(int line, RuntimeErrorType errorType) = 0;
    virtual void report(RuntimeErrorType errorType) = 0;
};

class IInterpreterIO{
public:
    virtual std::unique_ptr<Value> read() = 0;
    virtual void write(Value& value) = 0;
    virtual std::unique_ptr<Value> readText() = 0;
    virtual void writeText(Value& value) = 0;
};

class Interpreter{
public:
    Interpreter(IRuntimeErrorReporter* errorReporter, IInterpreterIO* interpreterIO);

    bool execute(std::vector<Token*> &tokens, std::map<std::string, Function>& functions, Value& result);

    bool execute(std::vector<Token*> &tokens, ProgramState& programState, Value& result);

private:
    bool execute(
        std::vector<Token*> &tokens,
        ProgramState& programState,
        Value& argumentA,
        Value& argumentB,
        Value& result);

    void executeOnThread(
        std::vector<Token*> tokens,
        ProgramState& programState,
        Value argumentA,
        Value argumentB);

    inline bool checkForCalculation(
        std::vector<Token*> &tokens,
        int& position,
        ProgramState& programState,
        Value& argumentA,
        Value& argumentB,
        Token*& operation,
        std::unique_ptr<Value>& leftParameter,
        std::unique_ptr<Value>& rightParameter);

    std::unique_ptr<Value> getNextArgument(
        int& position,
        std::vector<Token*> &tokens,
        ProgramState& programState,
        Value& argumentA,
        Value& argumentB,
        bool& hadError);

    // returns false if not an operation
    bool getOperatorOrFunctionParamerters(Token& operation, bool& hasLeftParam, bool& hasRightParam, ProgramState& programState);

    std::unique_ptr<Value> executeOperationOrFunction(
        Value& leftOfOperator,
        Value& rightOfOperator,
        Token& operation,
        Value& argumentA,
        Value& argumentB,
        ProgramState& programState,
        bool& hadError);

    bool isFunctionWithoutParameters(Token& function, ProgramState& programState);

    bool executeAsync(
        std::vector<Token*> &tokens,
        int& position,
        ProgramState& programState,
        Value& argumentA,
        Value& argumentB);

    inline std::unique_ptr<Value> executeModifier(
        Value& leftParameter,
        std::vector<Token*> &tokens,
        ProgramState& programState,
        Value& argumentA,
        Value& argumentB,
        int position,
        bool& hadError);

    inline void setVariable(const Value& value, const std::string& variableName, ProgramState& programState);
    
    inline void getVariable(Value& value, const std::string& variableName, ProgramState& programState);

    void initVariables(std::vector<Token*> &tokens, ProgramState& programState);

    void joinThreads(ProgramState& programState);

    int currentLineNumber(std::vector<Token*> &tokens, int position);
    
    inline void report(RuntimeErrorType errorType);

    inline void report(std::vector<Token*> &tokens, int position, RuntimeErrorType errorType);

    inline void endStatement(std::unique_ptr<Value>& lastResult, std::unique_ptr<Value>& leftParameter, std::unique_ptr<Value>& rightParameter);

private:
    IRuntimeErrorReporter* errorReporter;
    IInterpreterIO* interpreterIO;
};