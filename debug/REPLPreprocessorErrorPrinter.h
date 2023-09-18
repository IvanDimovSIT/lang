#pragma once
#include "BasePreprocessorErrorPrinter.h"

class REPLPreprocessorErrorPrinter : public BasePreprocessorErrorPrinter{
public:
    void reportFilepathError(const std::string& containingFile, std::string& filePath) override;
    
    void reportIncorrectIncludeSyntax(const std::string& containingFile, const std::string& line) override;
};
