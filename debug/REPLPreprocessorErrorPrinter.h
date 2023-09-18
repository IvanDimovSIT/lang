#pragma once
#include "BasePreprocessorErrorPrinter.h"

class REPLPreprocessorErrorPrinter : public BasePreprocessorErrorPrinter{
public:
    void reportFilepathError(const std::string& containingFile, int line, std::string& filePath) override;
    
    void reportIncorrectIncludeSyntax(const std::string& containingFile, int line) override;
};
