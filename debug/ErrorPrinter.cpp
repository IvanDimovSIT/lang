#include "ErrorPrinter.h"
#include <iostream>

void ErrorPrinter::report(RuntimeErrorType errorType){
    std::cout << "RuntimeError:" << errorType << std::endl;
}

void ErrorPrinter::report(int line, ScannerErrorType errorType){
    std::cout << "ScannerError:" << errorType << " (line:" << line << ")" << std::endl; 
}
