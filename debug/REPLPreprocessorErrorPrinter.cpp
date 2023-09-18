#include "REPLPreprocessorErrorPrinter.h"
#include <iostream>

void REPLPreprocessorErrorPrinter::reportFilepathError(const std::string& containingFile, int line, std::string& filePath)
{
    if(containingFile != ""){
        std::cout << "PreprocessorError: Incorrect Filepath \"" << filePath << "\" in \"" << containingFile << "\"" << std::endl; 
    }else{
        std::cout << "PreprocessorError: Incorrect Filepath \"" << filePath << "\"" << std::endl; 
    }
}
    
void REPLPreprocessorErrorPrinter::reportIncorrectIncludeSyntax(const std::string& containingFile, int line)
{
    if(containingFile != ""){
        std::cout << "PreprocessorError: Incorrect Include Syntax (line:" << line << ") in \"" << containingFile << "\"" << std::endl; 
    }else{
        std::cout << "PreprocessorError: Incorrect Include Syntax (line:" << line << ")" << std::endl; 
    }
}
