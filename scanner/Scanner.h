#pragma once
#include <unordered_map>
#include <unordered_set>
#include "Function.h"
#include "../token/Token.h"
#include "ScannerErrorType.h"

class IScannerErrorReporter{
public:
    virtual void report(int line, ScannerErrorType errorType) = 0;
};

class Scanner{
public:
    static bool scan(
        const std::string& programSource,
        std::vector<Token>& tokens,
        std::unordered_map<std::string, Function>& functions,
        IScannerErrorReporter* errorReporter);

    static bool scanREPL(
        const std::string& programSource,
        std::vector<Token>& tokens,
        std::unordered_map<std::string, Function>& functions,
        std::unordered_set<std::string>& declaredFunctionNames,
        IScannerErrorReporter* errorReporter);
        
private:
    static std::string removeComments(const std::string& source);

    static bool findFunctionNames(const std::string& source, std::unordered_set<std::string>& functionNames, IScannerErrorReporter* errorReporter);

    static bool extractFunctions(std::vector<Token>& tokens, std::unordered_map<std::string, Function>& functions);
    
    static bool matchToken(
        const std::string& tokenString,
        std::vector<Token>& tokens,
        std::unordered_set<std::string>& functionNames,
        std::unordered_set<std::string>& previousDeclaredFunctionNames,
        int line);

    static void determineFunctionPrameters(Function& function);

    static bool validateParenthesis(const std::string& source, const int sourceLen, IScannerErrorReporter* errorReporter);

    static bool validateOperatorModifier(std::vector<Token>& tokens, int line, IScannerErrorReporter* errorReporter);
};
