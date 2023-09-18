#include "BasePreprocessorErrorPrinter.h"
#include <iostream>

void BasePreprocessorErrorPrinter::reportIncludeRecursionLimitReached()
{
    std::cout << "PreprocessorError: Recursion limit reached" << std::endl;
}
