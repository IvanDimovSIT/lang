#pragma once
#include "../scanner/Preprocessor.h"

class BasePreprocessorErrorPrinter: public IPreprocessorErrorReporter{
public:
    virtual void reportIncludeRecursionLimitReached() override;
};