#include <iostream>
#include <fstream>
#include "../interpreter/Interpreter.h"
#include "../scanner/Scanner.h"
#include "../debug/ErrorPrinter.h"
#include "../interpreter/InterpreterIO.h"
#include "../interpreter/FunctionExtractor.h"

bool readFile(const std::string& filepath, std::string& contents)
{
    std::ifstream file(filepath);   
    if(!file.is_open())
        return false;

    std::string line;
    while(getline(file, line)){
        contents += line;
        contents += '\n';
    }

    return true;
}

void printResult(const std::vector<double>& result)
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
    std::cout << "Enter script filepath:" << std::endl;
    getline(std::cin, filepath);
    return filepath.size() > 0;
}

int main(int argc, char** argv)
{
    std::string source = "", filepath="";
    ErrorPrinter errorPrinter;
    InterpreterIO io;

    if(argc <= 1){
        if(!enterFilepath(filepath)){
            std::cout << "Filepath not entered" << std::endl;
            return 1;
        }
    }else{
        filepath = argv[1];
    }

    if(!readFile(filepath, source)){
        std::cout << "Error reading file" << std::endl;
        return 1;
    }

    std::vector<Token> tokens;
    std::map<std::string, Function> functions;
    if(!Scanner::scan(source, tokens, functions, &errorPrinter)){
        return 1;
    }

    std::vector<Token*> exec;
    if(!FunctionExtractor::extractFunctions(tokens, exec)){
        std::cout << "Error finding functions" << std::endl;
        return 1;
    }
    std::vector<double> result;
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