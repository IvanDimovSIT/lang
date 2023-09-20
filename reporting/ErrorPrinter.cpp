#include "ErrorPrinter.h"
#include <iostream>
#include "../util/StringUtil.h"

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

void ErrorPrinter::report(const std::vector<Token*>& tokens, int errorPosition, RuntimeErrorType errorType)
{
    static const std::string runtimeError = "RuntimeError in: ";
    static const int runtimeErrorSize = runtimeError.size();

    bool onNewLine = false;
    int tokenPositionInLine;
    std::string line = generateErrorLine(tokens, errorPosition, tokenPositionInLine);
    if(hasSeenError(line, errorType, onNewLine))
        return;

    if(onNewLine){
        std::cout << "RuntimeError in: " << line << std::endl; 
        std::cout << std::string(tokenPositionInLine+runtimeErrorSize, ' ') + '^' << std::endl; 
    }

    std::cout << " - " << (runtimeErrors.count(errorType)?runtimeErrors[errorType]:std::to_string(errorType)) << std::endl; 
}

void ErrorPrinter::report(const std::string& source, int position, ScannerErrorType errorType)
{
    static const std::string scannerError = "ScannerError in: ";
    static const int scannerErrorSize = scannerError.size();
    position--;

    bool onNewLine = false;
    int startPosition;
    std::string line = StringUtil::getLine(source, position, startPosition);
    const int errorPositionInLine = position - startPosition;

    if(hasSeenError(line, errorType, onNewLine))
        return;

    if(onNewLine){
        std::cout << scannerError << line << std::endl; 
        std::cout << std::string(errorPositionInLine+scannerErrorSize, ' ') << '^' << std::endl;
    }

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

std::string ErrorPrinter::generateErrorLine(const std::vector<Token*>& tokens, int errorPosition, int& tokenPositionInLine)
{
    const int size = tokens.size();
    tokenPositionInLine = 0;
    std::string line = "";
    line = tokens[errorPosition]->str;
    
    
    for(int offset=1; offset <= MAX_REPORT_TOKENS_COUNT; offset++){
        if(errorPosition + offset < size && tokens[errorPosition+offset]->str != "\n"){
            line = line + " " + tokens[errorPosition+offset]->str;
        }else 
            break;
    }
    for(int offset=1; offset <= MAX_REPORT_TOKENS_COUNT; offset++){
        if(errorPosition - offset >= 0 && tokens[errorPosition-offset]->str != "\n"){
            line = tokens[errorPosition-offset]->str + " " + line;
            tokenPositionInLine += tokens[errorPosition-offset]->str.size()+1;
        }else 
            break;
    }

    return line;
}

