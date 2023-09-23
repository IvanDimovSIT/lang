#pragma once
#include <unordered_map>
#include <unordered_set>
#include "Function.h"
#include "../token/Token.h"
#include "ScannerErrorType.h"

class IScannerErrorReporter{
public:
    virtual void report(const std::string& source, int position, ScannerErrorType errorType) = 0;
};

class Scanner{
public:
    static bool scan(
        const std::string& source,
        std::vector<Token>& tokens,
        std::unordered_map<std::string, Function>& functions,
        IScannerErrorReporter* errorReporter);

    static bool scan(
        const std::string& source,
        std::vector<Token>& tokens,
        std::unordered_map<std::string, Function>& functions,
        std::unordered_set<std::string>& previousFunctions,
        IScannerErrorReporter* errorReporter);
        
private:
    static bool findFunctionNames(const std::string& source, std::unordered_set<std::string>& functionNames, IScannerErrorReporter* errorReporter);

    static bool extractFunctions(const std::vector<Token>& tokens, std::unordered_map<std::string, Function>& functions);
    
    static void addNextToken(
        const std::string& tokenString,
        std::vector<Token>& tokens,
        std::unordered_set<std::string>& functionNames,
        std::unordered_set<std::string>& previousFunctions,
        const std::string& source,
        int position,
        bool& hadError,
        IScannerErrorReporter* errorReporter);

    static bool matchToken(
        const std::string& tokenString,
        std::vector<Token>& tokens,
        std::unordered_set<std::string>& functionNames,
        std::unordered_set<std::string>& previousFunctions);

    static inline void earlyTokenMatch(
        int& position,
        const std::string& source,
        std::string& currentToken,
        std::vector<Token>& tokens,
        std::unordered_set<std::string>& functionNames,
        std::unordered_set<std::string>& previousFunctions,
        bool& hadError,
        IScannerErrorReporter* errorReporter);

    static void determineFunctionPrameters(Function& function);

    static bool validateParenthesis(const std::string& source, const int sourceLen, IScannerErrorReporter* errorReporter);

    static bool isIdentifierNextToSymbol(const std::string& source, int position);

    static bool validateOperatorModifier(const std::vector<Token>& tokens);

    static void report(IScannerErrorReporter* errorReporter, bool& hadError, const std::string& source, int position, ScannerErrorType error);
};
