#pragma once
#include "../scanner/Scanner.h"
#include "BasePreprocessorErrorPrinter.h"
#include "../interpreter/Interpreter.h"

class ErrorPrinter : public IScannerErrorReporter, public IRuntimeErrorReporter, public BasePreprocessorErrorPrinter{
public:
    void report(RuntimeErrorType errorType) override;

    void report(const std::vector<const Token*>& tokens, int errorPosition, RuntimeErrorType errorType) override;

    void report(const std::string& source, int position, ScannerErrorType errorType) override;

    void reportFilepathError(const std::string& containingFile, std::string& filePath) override;
    
    void reportIncorrectIncludeSyntax(const std::string& containingFile, const std::string& line) override;

    void resetErrors();

private:
    bool hasSeenError(const std::string& line, RuntimeErrorType errorType, bool& lineNotSeen);

    bool hasSeenError(const std::string& line, RuntimeErrorType errorType);

    bool hasSeenError(const std::string& line, ScannerErrorType errorType, bool& lineNotSeen);

    std::string generateErrorLine(const std::vector<const Token*>& tokens, int errorPosition, int& tokenPositionInLine);

    std::unordered_map<std::string, std::unordered_set<ScannerErrorType>> seenScannerErrors;
    std::unordered_map<std::string, std::unordered_set<RuntimeErrorType>> seenRuntimeErrors;

    static std::unordered_map<ScannerErrorType, std::string> scannerErrors;
    static std::unordered_map<RuntimeErrorType, std::string> runtimeErrors;

    static const int MAX_REPORT_TOKENS_COUNT = 5;
};
