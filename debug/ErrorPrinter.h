#pragma once
#include "../scanner/Scanner.h"
#include "../interpreter/Interpreter.h"

class ErrorPrinter : public IScannerErrorReporter, IRuntimeErrorReporter{
public:
    void report(RuntimeErrorType errorType) override;
    void report(int line, ScannerErrorType errorType) override;
};