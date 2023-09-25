#include <iostream>
#include <fstream>
#include "../scanner/Preprocessor.h"
#include "../util/FileReader.h"
#include "../interpreter/Interpreter.h"
#include "../scanner/Scanner.h"
#include "../reporting/ErrorPrinter.h"
#include "../interpreter/InterpreterIO.h"
#include "../interpreter/FunctionExtractor.h"
#include "../util/StringUtil.h"
#include "REPL.h"

void printResult(const Value& result)
{
    if(result.size() == 0){
        std::cout << "No result" << std::endl;
        return;
    }
    std::string resultString = "";
    StringUtil::convertValueToString(result, resultString);
    std::cout << "Result: [" << resultString << ']' << std::endl;
}

bool enterFilepath(std::string& filepath)
{
    std::cout << "Enter script filepath or \"REPL\" to enter Read Execute Print mode:" << std::endl;
    getline(std::cin, filepath);
    return filepath.size() > 0;
}

int main(int argc, char** argv)
{
    std::string source = "", filepath="";

    if(argc <= 1){
        if(!enterFilepath(filepath)){
            std::cout << "Filepath not entered" << std::endl;
            return 1;
        }
    }else{
        filepath = argv[1];
    }

    if(filepath == "REPL"){
        std::cout << "*Entered REPL mode*" << std::endl;
        std::cout << "Type \"exit\" to quit the program" << std::endl;
        std::cout << "Type \"help\" + operator to view a description of the operator" << std::endl;
        REPL::run();
        return 0;
    }

    if(!FileReader::read(filepath, source)){
        std::cout << "Error reading file:\"" << filepath << "\"" << std::endl;
        return 1;
    }

    ErrorPrinter errorPrinter;
    InterpreterIO io;
    std::vector<Token> tokens;

    if(!Preprocessor::process(source, source, filepath, &errorPrinter)){
        return 1;
    }

    std::unordered_map<std::string, Function> functions;
    if(!Scanner::scan(source, tokens, functions, &errorPrinter)){
        return 1;
    }

    std::vector<const Token*> executable;
    if(!FunctionExtractor::extractFunctions(tokens, executable)){
        std::cout << "Error finding functions" << std::endl;
        return 1;
    }
    Value result;
    Interpreter interpreter((IRuntimeErrorReporter*)&errorPrinter, (IInterpreterIO*)&io);
    
    if(!interpreter.execute(executable, functions, result)){
        return 1;
    }
    
    printResult(result);

    return 0;
}