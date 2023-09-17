#pragma once
#include "../scanner/Scanner.h"
#include "../interpreter/Interpreter.h"

class ErrorPrinter : public IScannerErrorReporter, public IRuntimeErrorReporter{
public:
    void report(int line, RuntimeErrorType errorType) override;

    void report(RuntimeErrorType errorType) override;

    void report(int line, ScannerErrorType errorType) override;

    void resetErrors();

private:
    bool hasSeenError(int line, RuntimeErrorType errorType);

    bool hasSeenError(int line, ScannerErrorType errorType);

    std::unordered_map<int, std::unordered_set<ScannerErrorType>> seenScannerErrors;
    std::unordered_map<int, std::unordered_set<RuntimeErrorType>> seenRuntimeErrors;

    static std::unordered_map<ScannerErrorType, std::string> scannerErrors;
    static std::unordered_map<RuntimeErrorType, std::string> runtimeErrors;
};