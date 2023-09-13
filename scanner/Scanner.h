#pragma once
#include <map>
#include <set>
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
        std::map<std::string, Function>& functions,
        IScannerErrorReporter* errorReporter);

    static bool scanREPL(
        const std::string& programSource,
        std::vector<Token>& tokens,
        std::map<std::string, Function>& functions,
        std::set<std::string>& declaredFunctions,
        IScannerErrorReporter* errorReporter);
private:
    static std::string removeComments(const std::string& source);

    static bool findFunctionNames(const std::string& source, std::set<std::string>& functionNames, IScannerErrorReporter* errorReporter);
    static bool extractFunctions(std::vector<Token>& tokens, std::map<std::string, Function>& functions);
    
    static bool matchToken(const std::string& token, std::vector<Token>& tokens, std::set<std::string>& functionNames, std::set<std::string>& declaredFunctions, int line);
    static bool isSingleCharToken(char token);

    static void determineFunctionPrameters(Function& function);
    
    static std::map<std::string, TokenId> tokenMap;

    static bool validateParenthesis(const std::string& source, const int sourceLen, IScannerErrorReporter* errorReporter);

    static bool validateOperatorModifier(std::vector<Token>& tokens, int line, IScannerErrorReporter* errorReporter);
};
