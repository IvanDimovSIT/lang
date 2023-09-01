#pragma once
#include <map>
#include <memory>
#include "../token/Token.h"
#include "RuntimeErrorType.h"
#include "../scanner/Function.h"

class IRuntimeErrorReporter{
public:
    virtual void report(RuntimeErrorType errorType) = 0;
};

class IInterpreterIO{
public:
    virtual std::unique_ptr<std::vector<double>> read() = 0;
    virtual void write(std::vector<double>& value) = 0;
    virtual std::unique_ptr<std::vector<double>> readText() = 0;
    virtual void writeText(std::vector<double>& value) = 0;
};

struct LoopReturn{
    std::vector<Token*> condition;
    int loopStart;
    int loopEnd;
};

class Interpreter{
public:
    Interpreter(IRuntimeErrorReporter* errorReporter, IInterpreterIO* interpreterIO);
    bool execute(std::vector<Token*> &tokens, std::map<std::string, Function>& functions, std::vector<double>& result);

private:
    bool execute(
        std::vector<Token*> &tokens,
        std::map<std::string, Function>& functions,
        std::map<std::string, std::vector<double>>& localVariables,
        std::vector<double>& left,
        std::vector<double>& right,
        std::vector<double>& result);

    inline bool checkForCalculation(
        std::vector<Token*> &tokens,
        int& position,
        std::map<std::string, Function>& functions,
        std::map<std::string, std::vector<double>>& localVariables,
        std::vector<double>& left,
        std::vector<double>& right,
        Token*& operation,
        std::unique_ptr<std::vector<double>>& leftParameter,
        std::unique_ptr<std::vector<double>>& rightParameter);

    std::unique_ptr<std::vector<double>> getNextArgument(
        int& position,
        std::vector<Token*> &tokens,
        std::map<std::string, Function>& functions,
        std::map<std::string, std::vector<double>>& localVariables,
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError);

    // returns false if not an operation
    bool getOperatorOrFunctionParamerters(Token& operation, bool& leftParam, bool& rightParam, std::map<std::string, Function>& functions);

    std::unique_ptr<std::vector<double>> executeOperationOrFunction(
        std::vector<double>& leftOfOperator,
        std::vector<double>& rightOfOperator,
        Token& operation,
        std::vector<double>& left,
        std::vector<double>& right,
        std::map<std::string, Function>& functions,
        std::map<std::string, std::vector<double>>& localVariables,
        bool& hadError);

    bool isFunctionWithoutParameters(Token& function, std::map<std::string, Function>& functions);

    inline std::unique_ptr<std::vector<double>> executeModifier(
        std::vector<double>& leftParameter,
        std::vector<Token*> &tokens,
        std::map<std::string, Function>& functions,
        std::map<std::string, std::vector<double>>& localVariables,
        std::vector<double>& left,
        std::vector<double>& right,
        int position,
        bool& hadError);

private:
    IRuntimeErrorReporter* errorReporter;
    IInterpreterIO* interpreterIO;

};