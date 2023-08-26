#include "ErrorPrinter.h"
#include <iostream>

std::map<ScannerErrorType, std::string> ErrorPrinter::scannerErrors = {
    {ScannerErrorTypeFunctionDefinitionError, "Function definition error"},
    {ScannerErrorTypeFunctionNameDefinitionError, "Invalid Function name error"},
    {ScannerErrorTypeFunctionRedefinition, "Function redefinition error"},
    {ScannerErrorTypeUnrecognisedToken, "Unrecognised syntax"},
    {ScannerErrorTypeMoreClosingParenthesis, "Extra ')'"},
    {ScannerErrorTypeMoreOpenParenthesis, "Missing ')'"},
    {ScannerErrorTypeMoreClosingCurlyBrackets, "Extra '{'"},
    {ScannerErrorTypeMoreOpenCurlyBrackets, "Missing '}'"},
    {ScannerErrorTypeStringLiteralError, "Invalid string literal"},
    {ScannerErrorTypeNoOperatorToModify, "No operator to modify"}
};

std::map<RuntimeErrorType, std::string> ErrorPrinter::runtimeErrors = {
    {RuntimeErrorTypeEmptyData, "Empty Data"},
    {RuntimeErrorTypeDivisionByZero, "Division By Zero"},
    {RuntimeErrorTypeZeroPowerZero, "Zero power of zero"},
    {RuntimeErrorTypeArithmetic, "Arithmetic error"},
    {RuntimeErrorTypeOperationAsParameter, "Invalid operation"},
    {RuntimeErrorTypeNoClosingParenthesis, "Missing ')'"},
    {RuntimeErrorTypeEvaluationError, "Missing operation"},
    {RuntimeErrorTypeParameterError, "Invalid Parameter"},
    {RuntimeErrorTypeNotAnOperation, "No operation found"},
    {RuntimeErrorTypeMissingIfCondition, "Missing if condition"},
    {RuntimeErrorTypeInvalidIfSyntax, "Invalid if syntax"},
    {RuntimeErrorTypeMissingLoopCondition, "Missing loop condition"},
    {RuntimeErrorTypeNoOperatorToModify, "No operator to modify"}
};

void ErrorPrinter::report(RuntimeErrorType errorType){
    std::cout << "RuntimeError:" << (runtimeErrors.count(errorType)?runtimeErrors[errorType]:std::to_string(errorType)) << std::endl;
}

void ErrorPrinter::report(int line, ScannerErrorType errorType){
    std::cout << "ScannerError:" << (scannerErrors.count(errorType)?scannerErrors[errorType]:std::to_string(errorType)) << " (line:" << line << ")" << std::endl; 
}
