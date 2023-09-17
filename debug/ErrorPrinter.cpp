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

void ErrorPrinter::report(int line, RuntimeErrorType errorType)
{
    if(hasSeenError(line, errorType))
        return;
    
    if(line == IRuntimeErrorReporter::LINE_NOT_FOUND){
        report(errorType);
        return;
    }
    std::cout << "RuntimeError:" << (runtimeErrors.count(errorType)?runtimeErrors[errorType]:std::to_string(errorType))  << " (line:" << line << ")" << std::endl;
}

void ErrorPrinter::report(RuntimeErrorType errorType)
{
    std::cout << "RuntimeError:" << (runtimeErrors.count(errorType)?runtimeErrors[errorType]:std::to_string(errorType)) << std::endl;
}

void ErrorPrinter::report(int line, ScannerErrorType errorType)
{
    if(hasSeenError(line, errorType))
        return;

    std::cout << "ScannerError:" << (scannerErrors.count(errorType)?scannerErrors[errorType]:std::to_string(errorType)) << " (line:" << line << ")" << std::endl; 
}

void ErrorPrinter::resetErrors()
{
    seenScannerErrors.clear();
    seenRuntimeErrors.clear();
}

bool ErrorPrinter::hasSeenError(int line, RuntimeErrorType errorType)
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

bool ErrorPrinter::hasSeenError(int line, ScannerErrorType errorType)
{
    auto errors = seenScannerErrors.find(line);
    if(errors == seenScannerErrors.end()){
        seenScannerErrors[line] = {errorType};
    }else if(errors->second.find(errorType) == errors->second.end()){
        errors->second.insert(errorType);
    }else{
        return true;
    }

    return false;
}
