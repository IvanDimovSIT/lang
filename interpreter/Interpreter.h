#pragma once
#include <unordered_map>
#include <memory>
#include <stack>
#include <thread>
#include "../token/Token.h"
#include "RuntimeErrorType.h"
#include "ProgramState.h"
#include "../scanner/Function.h"

class IRuntimeErrorReporter{
public:
    virtual void report(const std::vector<const Token*>& tokens, int errorPosition, RuntimeErrorType errorType) = 0;

    virtual void report(RuntimeErrorType errorType) = 0;
};

class IInterpreterIO{
public:
    virtual std::unique_ptr<Value> read() = 0;
    
    virtual void write(const Value& value) = 0;
    
    virtual std::unique_ptr<Value> readText() = 0;
    
    virtual void writeText(const Value& value) = 0;
};

class Interpreter{
public:
    Interpreter(IRuntimeErrorReporter* errorReporter, IInterpreterIO* interpreterIO);

    bool execute(const std::vector<const Token*> &tokens, const std::unordered_map<std::string, Function>& functions, Value& result);

    bool execute(const std::vector<const Token*> &tokens, ProgramState& programState, Value& result);

private:
    bool execute(
        const std::vector<const Token*> &tokens,
        ProgramState& programState,
        const Value& argumentA,
        const Value& argumentB,
        Value& result);

    void executeOnThread(
        const std::vector<const Token*> tokens,
        ProgramState& programState,
        const Value argumentA,
        const Value argumentB);

    inline bool checkForCalculation(
        const std::vector<const Token*> &tokens,
        int& position,
        ProgramState& programState,
        const Value& argumentA,
        const Value& argumentB,
        const Token*& operation,
        std::unique_ptr<Value>& leftParameter,
        std::unique_ptr<Value>& rightParameter);

    inline bool getArgumentsAndOperation(
        int& position,
        const std::vector<const Token*> &tokens,
        ProgramState& programState,
        std::unique_ptr<Value>& leftParameter,
        std::unique_ptr<Value>& rightParameter,
        const Value& argumentA,
        const Value& argumentB,
        const Token*& operation);

    std::unique_ptr<Value> getNextArgument(
        int& position,
        const std::vector<const Token*> &tokens,
        ProgramState& programState,
        const Value& argumentA,
        const Value& argumentB,
        bool& hadError);

    //returns statement end position
    int executeStatement(
        const std::vector<const Token*> &tokens,
        int position,
        ProgramState& programState,
        const Value& argumentA,
        const Value& argumentB,
        Value& result,
        bool& hadError);

    // returns false if not an operation
    bool getOperatorOrFunctionParamerters(const Token& operation, bool& hasLeftParam, bool& hasRightParam, ProgramState& programState);

    std::unique_ptr<Value> executeOperationOrFunction(
        const Value& leftOfOperator,
        const Value& rightOfOperator,
        const Token& operation,
        const Value& argumentA,
        const Value& argumentB,
        ProgramState& programState,
        bool& hadError);

    bool isFunctionWithoutParameters(const Token& function, ProgramState& programState);

    bool executeAsync(
        const std::vector<const Token*> &tokens,
        int& position,
        ProgramState& programState,
        const Value& argumentA,
        const Value& argumentB);

    inline std::unique_ptr<Value> executeModifier(
        Value& leftParameter,
        const std::vector<const Token*> &tokens,
        ProgramState& programState,
        const Value& argumentA,
        const Value& argumentB,
        int position,
        bool& hadError);

    inline void setVariable(const Value& value, const std::string& variableName, ProgramState& programState);
    
    inline void getVariable(Value& value, const std::string& variableName, ProgramState& programState);

    void joinThreads(ProgramState& programState);
    
    void report(RuntimeErrorType errorType);

    void report(const std::vector<const Token*> &tokens, int position, RuntimeErrorType errorType);

    inline void endStatement(std::unique_ptr<Value>& lastResult, std::unique_ptr<Value>& leftParameter, std::unique_ptr<Value>& rightParameter);

    inline std::unique_ptr<Value> readNumbers(ProgramState& programState);

    inline std::unique_ptr<Value> readText(ProgramState& programState);
    
    inline void writeNumbers(ProgramState& programState, Value& value);
    
    inline void writeText(ProgramState& programState, Value& value);
    
private:
    IRuntimeErrorReporter* errorReporter;
    IInterpreterIO* interpreterIO;
};