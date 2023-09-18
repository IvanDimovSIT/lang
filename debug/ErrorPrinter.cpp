#include "ErrorPrinter.h"
#include <iostream>

std::unordered_map<ScannerErrorType, std::string> ErrorPrinter::scannerErrors = {
    {ScannerErrorTypeFunctionDefinitionError, "Function definition error"},
    {ScannerErrorTypeFunctionNameDefinitionError, "Invalid Function name error"},
    {ScannerErrorTypeFunctionRedefinition, "Function redefinition error"},
    {ScannerErrorTypeUnrecognisedToken, "Unrecognised syntax"},
    {ScannerErrorTypeMoreClosingParenthesis, "Extra ')'"},
    {ScannerErrorTypeMoreOpenParenthesis, "Missing ')'"},
    {ScannerErrorTypeMoreClosingCurlyBrackets, "Extra '}'"},
    {ScannerErrorTypeMoreOpenCurlyBrackets, "Missing '}'"},
    {ScannerErrorTypeStringLiteralError, "Invalid string literal"},
    {ScannerErrorTypeNoOperatorToModify, "No operator to modify"},
    {ScannerErrorTypeMissingAsyncStart, "Extra ']'"},
    {ScannerErrorTypeMissingAsyncEnd, "Missing ']'"}
};

std::unordered_map<RuntimeErrorType, std::string> ErrorPrinter::runtimeErrors = {
    {RuntimeErrorTypeEmptyData, "Empty Data"},
    {RuntimeErrorTypeDivisionByZero, "Division By Zero"},
    {RuntimeErrorTypeZeroPowerZero, "Zero power of zero"},
    {RuntimeErrorTypeArithmetic, "Arithmetic error"},
    {RuntimeErrorTypeOperationAsParameter, "Invalid operation"},
    {RuntimeErrorTypeNoClosingParenthesis, "Missing ')'"},
    {RuntimeErrorTypeEvaluationError, "Missing operation"},
    {RuntimeErrorTypeParameterError, "Invalid parameter"},
    {RuntimeErrorTypeNotAnOperation, "No operation found"},
    {RuntimeErrorTypeMissingIfCondition, "Missing if condition"},
    {RuntimeErrorTypeInvalidIfSyntax, "Invalid if syntax"},
    {RuntimeErrorTypeMissingLoopCondition, "Missing loop condition"},
    {RuntimeErrorTypeNoOperatorToModify, "No operator to modify"},
    {RuntimeErrorTypeThreadHadError, "Thread Error"},
    {RuntimeErrorTypeOperatorError, "Error executing operator"},
    {RuntimeErrorTypeMissingParameter, "Missing parameter"}
};

void ErrorPrinter::report(RuntimeErrorType errorType)
{
    if(hasSeenError("", errorType))
        return;
    
    
    std::cout << "RuntimeError:" << (runtimeErrors.count(errorType)?runtimeErrors[errorType]:std::to_string(errorType)) << std::endl;
}

void ErrorPrinter::report(const std::string& line, RuntimeErrorType errorType)
{
    bool onNewLine = false;
    if(hasSeenError(line, errorType, onNewLine))
        return;

    if(onNewLine)
        std::cout << "RuntimeError in: " << line << std::endl; 

    std::cout << " - " << (runtimeErrors.count(errorType)?runtimeErrors[errorType]:std::to_string(errorType)) << std::endl; 
}

void ErrorPrinter::report(const std::string& line, ScannerErrorType errorType)
{
    bool onNewLine = false;
    if(hasSeenError(line, errorType, onNewLine))
        return;

    if(onNewLine)
        std::cout << "ScannerError in: " << line << std::endl; 

    std::cout << " - " << (scannerErrors.count(errorType)?scannerErrors[errorType]:std::to_string(errorType)) << std::endl; 
}

void ErrorPrinter::reportFilepathError(const std::string& containingFile, std::string& filePath)
{
    std::cout << "PreprocessorError: Incorrect Filepath \"" << filePath << "\" in \"" << containingFile << "\"" << std::endl; 
}
    
void ErrorPrinter::reportIncorrectIncludeSyntax(const std::string& containingFile, const std::string& line)
{
    std::cout << "PreprocessorError: Incorrect Include Syntax in \"" << containingFile << "\":" << line << std::endl; 
}

void ErrorPrinter::resetErrors()
{
    seenScannerErrors.clear();
    seenRuntimeErrors.clear();
}

bool ErrorPrinter::hasSeenError(const std::string& line, RuntimeErrorType errorType)
{
    auto errors = seenRuntimeErrors.find(line);
    if(errors == seenRuntimeErrors.end()){
        seenRuntimeErrors[line] = {errorType};
    }else if(errors->second.find(errorType) == errors->second.end()){
        errors->second.insert(errorType);
    }else{
        return true;
    }

    return false;
}

bool ErrorPrinter::hasSeenError(const std::string& line, RuntimeErrorType errorType, bool& lineNotSeen)
{
    lineNotSeen = false;
    auto errors = seenRuntimeErrors.find(line);
    if(errors == seenRuntimeErrors.end()){
        lineNotSeen = true;
        seenRuntimeErrors[line] = {errorType};
    }else if(errors->second.find(errorType) == errors->second.end()){
        errors->second.insert(errorType);
    }else{
        return true;
    }

    return false;
}

bool ErrorPrinter::hasSeenError(const std::string& line, ScannerErrorType errorType, bool& lineNotSeen)
{
    lineNotSeen = false;
    auto errors = seenScannerErrors.find(line);
    if(errors == seenScannerErrors.end()){
        lineNotSeen = true;
        seenScannerErrors[line] = {errorType};
    }else if(errors->second.find(errorType) == errors->second.end()){
        errors->second.insert(errorType);
    }else{
        return true;
    }

    return false;
}
