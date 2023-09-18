#include "REPL.h"
#include "../debug/REPLPreprocessorErrorPrinter.h"
#include "../scanner/Preprocessor.h"
#include <iostream>

void REPL::run()
{
    ErrorPrinter errorPrinter;
    REPLPreprocessorErrorPrinter preprocessorErrorPrinter;
    InterpreterIO io;
    Interpreter interpreter(&errorPrinter, &io);
    std::vector<Token> tokens;
    std::vector<Token*> tokensRef;
    std::unordered_map<std::string, Function> functions = {};
    std::unordered_set<std::string> functionNames;
    ProgramState programState;

    std::string source;
    do{
        errorPrinter.resetErrors();
        functions = programState.functions;
        tokens.clear();
        tokensRef.clear();

        source = readInput();
        if(isStringExit(source)){
            deleteProgramState(programState);
            return;
        }
        if(!Preprocessor::process(source, source, "", &preprocessorErrorPrinter)){
            continue;
        }
        if(!Scanner::scanREPL(source, tokens, functions, functionNames, &errorPrinter)){
            continue;
        }
        if(!FunctionExtractor::extractFunctions(tokens, tokensRef)){
            std::cout << "Definition error" << std::endl;
            continue;
        }
        addFunctionNames(functionNames, functions);
        copyFunctions(functions, programState);
        Value result;
        if(!interpreter.execute(tokensRef, programState, result)){
            continue;
        }
        printValue(result);
    }while(true);
}

void REPL::countOpen(const std::string& line, int& openCurly, int& openSquare)
{
    for(const auto& i: line){
        switch (i)
        {
        case '{': openCurly++; break;
        case '[': openSquare++; break;
        case '}': openCurly--; break;
        case ']': openSquare--; break;
        default:break;
        }
    }
}

std::string REPL::readInput()
{
    std::string read, source = "";
    int openCurly = 0, openSquare = 0;
    do{
        if(openCurly > 0 || openSquare > 0)
            std::cout << "... ";
        else
            std::cout << ">>> ";
        getline(std::cin, read);
        source += read;
        source += '\n';
        countOpen(read, openCurly, openSquare);
    }while(openCurly > 0 || openSquare > 0);

    return source;
}

void REPL::deleteFunction(Function& function)
{
    for(auto& i:function.body)
        delete i;
    
    function.body.clear();
}


void REPL::copyFunctions(std::unordered_map<std::string, Function>& functions, ProgramState& programState)
{
    for(auto& i: functions){
        Function functionCopy = i.second;
        if(programState.functions.find(i.first) != programState.functions.end()){
            deleteFunction(programState.functions[i.first]);
        }
        for(int i=0; i<functionCopy.body.size(); i++)
            functionCopy.body[i] = new Token(*functionCopy.body[i]);

        programState.functions[i.first] = functionCopy;
    }
}

void REPL::deleteProgramState(ProgramState& programState)
{
    for(auto& i: programState.functions){
        deleteFunction(i.second);
    }
}

void REPL::printValue(Value& v)
{
    const int size = v.size();
    if(size == 1){
        std::cout << v[0] << std::endl;
        return;
    }else if(size == 0){
        return;
    }

    std::cout << "[";
    for(int i=0; i<size-1; i++){
        std::cout << v[i] <<", ";
    }
    std::cout << v[size-1] << "]" << std::endl;
}

bool REPL::isStringExit(const std::string& line)
{
    const int size = line.size();
    for(int i=0; i<size-3; i++){
        if(line[i] == ' ' || line[i] == '\n' || line[i] == '\t')
            continue;
        
        if(line[i] == 'e'){
            return line[i+1] == 'x' && line[i+2] == 'i' && line[i+3] == 't';
        }else{
            return false;
        }
    }

    return false;
}

void REPL::addFunctionNames(std::unordered_set<std::string>& functionNames, std::unordered_map<std::string, Function>& functions){
    for(auto& i:functions)
        functionNames.insert(i.first);
}
