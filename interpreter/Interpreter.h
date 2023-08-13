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

class Interpreter{
public:
    static bool execute(std::vector<Token*> &tokens, std::map<std::string, Function>& functions, std::vector<double>& result, IRuntimeErrorReporter* errorReporter);

private:
    static bool execute(
        std::vector<Token*> &tokens,
        std::map<std::string, Function>& functions,
        std::map<std::string, std::vector<double>>& localVariables,
        std::vector<double>& left,
        std::vector<double>& right,
        std::vector<double>& result,
        IRuntimeErrorReporter* errorReporter);

    //removes function declarations and function bodies
    static bool extractFunctions(std::vector<Token*> &tokens, std::vector<Token*> &dest);

    static int findLastNonEndlineTokenIndex(std::vector<Token*> &tokens);
    
    // uses execute
    static std::unique_ptr<std::vector<double>> evaluate(
        std::vector<Token*> &tokens,
        std::map<std::string, Function>& functions,
        std::map<std::string, std::vector<double>>& localVariables,
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError,
        IRuntimeErrorReporter* errorReporter);


    // examples:
    // extract next value:  3 + 1,2,3
    //            we are here ^   --> extracted -> 1,2,3
    //
    // extract next value:  3 + ( 1,2,3 - VAR )
    //            we are here ^   --> extracted -> ( 1,2,3 - VAR )
    //
    static bool getNextArgument(std::vector<Token*> &tokens, std::vector<Token*> &dest);

    // returns false if not an operation
    static bool getOperatorOrFunctionParamerters(Token& operation, bool& leftParam, bool& rightParam);

    static std::unique_ptr<std::vector<double>> executeOperationOrFunction(
        Token& operation,
        std::vector<double>& left,
        std::vector<double>& right,
        std::map<std::string, Function>& functions,
        std::map<std::string, std::vector<double>>& localVariables,
        bool& hadError,
        IRuntimeErrorReporter* errorReporter);

};