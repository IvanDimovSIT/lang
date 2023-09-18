#include <iostream>
#include <fstream>
#include "../scanner/Preprocessor.h"
#include "../util/FileReader.h"
#include "../interpreter/Interpreter.h"
#include "../scanner/Scanner.h"
#include "../debug/ErrorPrinter.h"
#include "../interpreter/InterpreterIO.h"
#include "../interpreter/FunctionExtractor.h"
#include "REPL.h"

void printResult(const Value& result)
{
    std::cout << "Result: [";
    const int size = result.size();
    for(int i=0; i<size-1; i++){
        std::cout << result[i] <<", ";
    }
    std::cout << result[size-1] << "]" << std::endl;
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
        std::cout << "*Entered REPL mode*\nType \"exit\" to quit the program" << std::endl;
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

    std::vector<Token*> exec;
    if(!FunctionExtractor::extractFunctions(tokens, exec)){
        std::cout << "Error finding functions" << std::endl;
        return 1;
    }
    Value result;
    Interpreter interpreter((IRuntimeErrorReporter*)&errorPrinter, (IInterpreterIO*)&io);
    
    if(!interpreter.execute(exec, functions, result)){
        return 1;
    }

    if(result.size() > 0){
        printResult(result);
    }else{
        std::cout << "No result" << std::endl;
    }

    return 0;
}