#pragma once
#include "../scanner/Scanner.h"
#include "BasePreprocessorErrorPrinter.h"
#include "../interpreter/Interpreter.h"

class ErrorPrinter : public IScannerErrorReporter, public IRuntimeErrorReporter, public BasePreprocessorErrorPrinter{
public:
    void report(int line, RuntimeErrorType errorType) override;

    void report(RuntimeErrorType errorType) override;

    void report(int line, ScannerErrorType errorType) override;

    void reportFilepathError(const std::string& containingFile, int line, std::string& filePath) override;
    
    void reportIncorrectIncludeSyntax(const std::string& containingFile, int line) override;

    void resetErrors();

private:
    bool hasSeenError(int line, RuntimeErrorType errorType);

    bool hasSeenError(int line, ScannerErrorType errorType);

    std::unordered_map<int, std::unordered_set<ScannerErrorType>> seenScannerErrors;
    std::unordered_map<int, std::unordered_set<RuntimeErrorType>> seenRuntimeErrors;

    static std::unordered_map<ScannerErrorType, std::string> scannerErrors;
    static std::unordered_map<RuntimeErrorType, std::string> runtimeErrors;
};