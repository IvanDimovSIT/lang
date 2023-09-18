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
        const std::string& source,
        std::vector<Token>& tokens,
        std::unordered_map<std::string, Function>& functions,
        IScannerErrorReporter* errorReporter);

    static bool scanREPL(
        const std::string& source,
        std::vector<Token>& tokens,
        std::unordered_map<std::string, Function>& functions,
        std::unordered_set<std::string>& previousFunctions,
        IScannerErrorReporter* errorReporter);
        
private:
    static bool findFunctionNames(const std::string& source, std::unordered_set<std::string>& functionNames, IScannerErrorReporter* errorReporter);

    static bool extractFunctions(std::vector<Token>& tokens, std::unordered_map<std::string, Function>& functions);
    
    static void addNextToken(
        const std::string& tokenString,
        std::vector<Token>& tokens,
        std::unordered_set<std::string>& functionNames,
        std::unordered_set<std::string>& previousFunctions,
        int line,
        bool& hadError,
        IScannerErrorReporter* errorReporter);

    static bool matchToken(
        const std::string& tokenString,
        std::vector<Token>& tokens,
        std::unordered_set<std::string>& functionNames,
        std::unordered_set<std::string>& previousFunctions,
        int line);

    static void determineFunctionPrameters(Function& function);

    static bool validateParenthesis(const std::string& source, const int sourceLen, IScannerErrorReporter* errorReporter);

    static bool validateOperatorModifier(std::vector<Token>& tokens, int line, IScannerErrorReporter* errorReporter);

    static void report(IScannerErrorReporter* errorReporter, int line, ScannerErrorType error);
};
