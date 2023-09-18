#pragma once
#include "../scanner/Preprocessor.h"

class REPLPreprocessorErrorPrinter : public IPreprocessorErrorReporter{
public:
    void reportFilepathError(const std::string& containingFile, int line, std::string& filePath) override;
    
    void reportIncorrectIncludeSyntax(const std::string& containingFile, int line) override;
};
